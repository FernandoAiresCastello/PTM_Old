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

bool IsValidOpcode(std::string& opcode)
{
	return Op.find(opcode) != Op.end();
}

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
	// close window, delete stuff, etc
}

void RunMachine()
{
	while (!Exit) {
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

void Abort(std::string msg)
{
	MsgBox::Error(Title, String::Format("%s at line %i:\n\n%s",
		msg.c_str(), CurLine->SrcLineNr, CurLine->SrcCode.c_str()));

	Exit = true;
	exit(1);
}

bool Req(int count)
{
	if (Args->size() == count)
		return true;
	
	Abort(String::Format("Illegal argument count (expected %i, got %i)", count, Args->size()));
	return false;
}

Parameter* Arg(int index)
{
	if (index >= Args->size()) {
		Abort("Missing arguments");
		return nullptr;
	}
	else {
		return &(*Args)[index];
	}
}

void AssertPtrExists(std::string ident)
{
	if (VarPtr.find(ident) == VarPtr.end()) {
		Abort(String::Format("Undefined identifier \"%s\"", ident.c_str()));
	}
}

void AssertMemAddr(int addr)
{
	if (addr < 0 || addr >= MemSize) {
		Abort("Memory address out of range");
	}
}

void AssertArgType(Parameter* param, ParameterType type) {
	if (param->Type != type) {
		Abort("Invalid argument type");
		return;
	}
}

void ResolveArg(Parameter* param, bool nullTermString)
{
	if (nullTermString) {
		if (param->Type == ParameterType::Identifier) {
			AssertPtrExists(param->StringValue);
			param->NumberValue = VarPtr[param->StringValue];
			param->StringValue = "";
			char ch = -1;
			int addr = param->NumberValue;
			while (true) {
				ch = (char)Memory[addr];
				if (ch == 0)
					break;
				param->StringValue.push_back(ch);
				addr++;
			}
		}
		else if (param->Type == ParameterType::PointerAccess) {
			param->NumberValue = Memory[VarPtr[param->StringValue]];
			param->StringValue = String::ToString(param->NumberValue);
		}
	}
	else if (param->Type == ParameterType::Identifier) {
		AssertPtrExists(param->StringValue);
		param->NumberValue = VarPtr[param->StringValue];
		param->StringValue = String::ToString(param->NumberValue);
	}
	else if (param->Type == ParameterType::PointerAccess) {
		AssertPtrExists(param->StringValue);
		param->NumberValue = Memory[VarPtr[param->StringValue]];
		param->StringValue = String::ToString(param->NumberValue);
	}
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
		Wnd->Update();
	}
}
