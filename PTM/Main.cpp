#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "GlobalDefs.h"
#include "Program.h"
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
#include "Boot.h"
using namespace CppUtils;
using namespace TileGameLib;

int main(int argc, char* argv[]) {

	InitCommands();

	bool bootOk = LoadBootFile();
	if (!bootOk)
		return 1;

	Program* prog = new Program();

	if (!Boot.Autorun.empty()) {
		prog->Load(Boot.Autorun);
		if (prog->Lines.empty()) {
			MsgBox::Error(APP_NAME, String::Format(Error.InvalidProgramFile, Boot.Autorun.c_str()));
			delete prog;
			return 1;
		}
		if (!prog->Errors.empty()) {
			MsgBox::Error(APP_NAME, prog->Errors[0]);
			delete prog;
			return 1;
		}
		InitMachine(prog);
		RunMachine();
		DestroyMachine();
	}

	delete prog;
	return 0;
}
