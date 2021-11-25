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
		if (!Error.empty()) {
			MsgBox::Error("PTM", String::Format("%s at line %i: %s",
				Error.c_str(), CurLine->SrcLineNr, CurLine->SrcCode.c_str()));
		}
	}
}

void Abort(std::string msg)
{
	Exit = true;
	Error = msg;
}

Parameter* Arg()
{
	if (IxArg >= Args->size()) {
		Abort("Missing arguments");
		return nullptr;
	}
	else {
		Parameter* param = &(*Args)[IxArg];
		IxArg++;
		return param;
	}
}
