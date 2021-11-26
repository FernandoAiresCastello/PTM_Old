#include <CppUtils.h>
#include <TileGameLib.h>
#include "Machine.h"
#include "System.h"
using namespace CppUtils;
using namespace TileGameLib;

Program* Prog;
ProgramLine* CurLine;
int IxCurLine;
bool Exit;
std::string Error;
std::map<std::string, void(*)()> Op;
std::vector<Parameter>* Args;
int IxArg;

void InitMachine(Program* prog)
{
	Exit = false;
	Prog = prog;
	CurLine = nullptr;
	IxCurLine = 0;
	Args = nullptr;
	IxArg = 0;
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
	while (!Exit) {
		ProcessGlobalEvents();
		CurLine = &Prog->Lines[IxCurLine];
		Args = &CurLine->Cmd.Params;
		IxArg = 0;

		if (Op.find(CurLine->Cmd.Operation) != Op.end()) {
			Op[CurLine->Cmd.Operation]();
		}
		else {
			Abort("Invalid command");
		}
		if (!Exit) {
			IxCurLine++;
			if (IxCurLine >= Prog->Lines.size())
				Abort("Program ended without EXIT");
		}
	}
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
	SDL_Event e;
	SDL_PollEvent(&e);

	if (e.type == SDL_QUIT) {
		Exit = true;
	}
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && TKey::Alt()) {
		Wnd->ToggleFullscreen();
	}

	if (Wnd) {
		Wnd->Update();
	}
}

void Argc(int expectedArgCount)
{
	if (Args->size() != expectedArgCount) {
		Abort(String::Format("Expected %i arguments but got %i", expectedArgCount, Args->size()));
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

	Abort("Argument index out of range");
	return nullptr;
}

std::string ArgStringLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::StringLiteral) {
		Abort("Expected a string literal");
		return 0;
	}
	return arg->StringValue;
}

int ArgNumberLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::NumberLiteral) {
		Abort("Expected a number literal");
		return 0;
	}
	return arg->NumberValue;
}

int ArgNumber()
{
	Parameter* arg = Arg();
	
	if (arg->Type == ParameterType::NumberLiteral) {
		return arg->NumberValue;
	}
	else if (arg->Type == ParameterType::Identifier) {
		return Peek(arg->StringValue);
	}
	else if (arg->Type == ParameterType::Address) {
		return GetAddress(arg->StringValue);
	}
	else {
		Abort("Syntax error");
	}
	return 0;
}

std::string ArgIdentifier()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::Identifier) {
		Abort("Expected an identifier");
		return 0;
	}
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
				Abort("Memory address out of bounds");
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
		Abort("Syntax error");
	}
	return "";
}

int Peek(std::string ptr)
{
	if (VarPtr.find(ptr) == VarPtr.end()) {
		Abort("Undefined identifier " + ptr);
		return 0;
	}

	return Memory[VarPtr[ptr]];
}

int GetAddress(std::string ptr)
{
	if (VarPtr.find(ptr) == VarPtr.end()) {
		Abort("Undefined identifier " + ptr);
		return 0;
	}

	return VarPtr[ptr];
}
