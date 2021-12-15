#include <stack>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

Program* Prog;
ProgramLine* CurLine;
int IxCurLine;
bool Exit;
std::map<std::string, void(*)()> Op;
ErrorMessages Error;
std::vector<Parameter>* Args;
int IxArg;
bool Branch;
SDL_Event Event;
std::stack<int> CallStack;
int KeyPressed;

void InitMachine(Program* prog)
{
	Util::Randomize();

	Exit = false;
	Prog = prog;
	CurLine = nullptr;
	IxCurLine = 0;
	Branch = false;
	Args = nullptr;
	IxArg = 0;
	KeyPressed = 0;
}

void DestroyMachine()
{
	delete Wnd;
	Wnd = nullptr;
	delete[] Memory;
	Memory = nullptr;
}

void RunMachine()
{
	SDL_CreateThread(RunMachineThread, "RunMachineThread", nullptr);

	while (!Exit) {
		ProcessGlobalEvents();
		if (WindowCreationRequested) {
			if (Wnd) {
				Abort(Error.WindowAlreadyOpen);
			}
			else {
				WindowCreationRequested = false;
				Wnd = new TWindow(RequestedWindowWBuf, RequestedWindowHBuf, RequestedWindowWWnd, RequestedWindowHWnd, false);
			}
		}
		if (Wnd) {
			UpdateWindow();
		}
	}
}

int RunMachineThread(void* dummy)
{
	while (!Exit) {
		CurLine = &Prog->Lines[IxCurLine];
		Args = &CurLine->Cmd.Params;
		IxArg = 0;

		if (Op.find(CurLine->Cmd.Operation) != Op.end())
			Op[CurLine->Cmd.Operation]();
		else
			Abort(Error.UnknownCommand);

		if (!Exit) {
			if (Branch)
				Branch = false;
			else
				IxCurLine++;

			if (IxCurLine >= Prog->Lines.size())
				Abort(Error.ProgEndWithoutExit);
		}
	}

	return 0;
}

void UpdateWindow()
{
	Wnd->Update();
	Wnd->SetTitle(Title);
}

bool IsValidOpcode(std::string& opcode)
{
	return Op.find(opcode) != Op.end();
}

void Abort(std::string msg, bool printInfo)
{
	if (printInfo) {
		MsgBox::Error(Title, String::Format("%s at line %i:\n\n%s",
			msg.c_str(), CurLine->SrcLineNr, CurLine->SrcCode.c_str()));
	}
	else {
		MsgBox::Error(Title, msg);
	}

	Exit = true;
	exit(1);
}

void ProcessGlobalEvents()
{
	Event = { 0 };
	SDL_PollEvent(&Event);

	if (Event.type == SDL_QUIT) {
		Exit = true;
	}
	else if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_RETURN && TKey::Alt() && Wnd) {
		Wnd->ToggleFullscreen();
	}
	else if (Event.type == SDL_KEYDOWN) {
		KeyPressed = Event.key.keysym.sym;
	}
}

void Jump(int ixProgLine)
{
	IxCurLine = ixProgLine;
	Branch = true;
}

void Call(int ixProgLine)
{
	CallStack.push(IxCurLine + 1);
	IxCurLine = ixProgLine;
	Branch = true;
}

void Return()
{
	if (CallStack.empty()) {
		Abort(Error.CallStackEmpty);
		return;
	}
	IxCurLine = CallStack.top();
	CallStack.pop();
	Branch = true;
}

void Argc(int expectedArgCount)
{
	if (Args->size() != expectedArgCount) {
		Abort(String::Format(Error.InvalidArgCount, expectedArgCount, Args->size()));
		return;
	}
}

Parameter* Arg()
{
	if (IxArg >= 0 && IxArg < Args->size()) {
		Parameter* arg = &Args->at(IxArg);
		IxArg++;
		return arg;
	}

	Abort(Error.ArgIndexOutOfRange);
	return nullptr;
}

std::string ArgStringLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::StringLiteral) {
		Abort(Error.StringLiteralExpected);
		return 0;
	}
	return arg->StringValue;
}

int ArgNumberLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::NumberLiteral) {
		Abort(Error.NumberLiteralExpected);
		return 0;
	}
	return arg->NumberValue;
}

int ArgNumber()
{
	Parameter* arg = Arg();
	
	if (arg->Type == ParameterType::NumberLiteral)
		return arg->NumberValue;
	if (arg->Type == ParameterType::CharLiteral)
		return arg->NumberValue;
	else if (arg->Type == ParameterType::Identifier)
		return Peek(arg->StringValue);
	else if (arg->Type == ParameterType::Address)
		return GetAddress(arg->StringValue);
	else
		Abort(Error.SyntaxError);
	
	return 0;
}

std::string ArgIdentifier(bool assertExists)
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::Identifier) {
		Abort(Error.IdentifierExpected);
		return 0;
	}
	if (assertExists)
		AssertIdentifier(arg->StringValue);

	return arg->StringValue;
}

std::string ArgString()
{
	Parameter* arg = Arg();

	if (arg->Type == ParameterType::StringLiteral) {
		return arg->StringValue;
	}
	else if (arg->Type == ParameterType::NumberLiteral) {
		return arg->StringValue;
	}
	else if (arg->Type == ParameterType::Identifier) {
		return String::ToString(Peek(arg->StringValue));
	}
	else if (arg->Type == ParameterType::Address) {
		return String::ToString(GetAddress(arg->StringValue));
	}
	else if (arg->Type == ParameterType::StringPointer) {
		std::string str = "";
		int addr = GetAddress(arg->StringValue);
		int ch = -1;
		while (ch != 0) {
			if (addr >= MemSize) {
				Abort(Error.MemoryAddrOutOfBounds);
				return "";
			}
			ch = Memory[addr++];
			if (ch != 0) {
				str.push_back(ch);
			}
		}
		return str;
	}
	else {
		Abort(Error.SyntaxError);
	}
	return "";
}

int Peek(std::string& identifier)
{
	if (Addr.find(identifier) == Addr.end()) {
		Abort(String::Format(Error.IdentifierNotFound, identifier.c_str()));
		return 0;
	}
	return Memory[Addr[identifier]];
}

int GetAddress(std::string& identifier)
{
	if (Addr.find(identifier) == Addr.end()) {
		Abort(String::Format(Error.IdentifierNotFound, identifier.c_str()));
		return -1;
	}
	return Addr[identifier];
}

void AssertIdentifier(std::string& identifier)
{
	if (Addr.find(identifier) == Addr.end())
		Abort(String::Format(Error.IdentifierNotFound, identifier.c_str()));
}

int ArgLabel()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::Identifier) {
		Abort(Error.LabelExpected);
		return -1;
	}
	std::string& label = arg->StringValue;
	if (Prog->Labels.find(label) == Prog->Labels.end()) {
		Abort(String::Format(Error.LabelNotFound, label.c_str()));
		return -1;
	}
	return Prog->Labels[label];
}
