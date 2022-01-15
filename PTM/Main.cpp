#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "GlobalDefs.h"
#include "Program.h"
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

int main(int argc, char* argv[]) {

	InitCommands();
	Program* prog = new Program();

	if (argc > 1) {
		std::string programFile = argv[1];
		if (File::Exists(programFile)) {
			prog->Load(programFile);
		}
		else {
			MsgBox::Error(APP_NAME, "Program file not found:\n\n" + programFile);
			delete prog;
			return 1;
		}
	}
	else {
		MsgBox::Error(APP_NAME, "Program file not specified");
		delete prog;
		return 1;
	}

	if (prog->Validate()) {
		InitMachine(prog);
		RunMachine();
		DestroyMachine();
	}
	else {
		delete prog;
		return 1;
	}

	return 0;
}
