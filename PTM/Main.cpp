#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "GlobalDefs.h"
#include "Program.h"
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
#include "BootMenu.h"
using namespace CppUtils;
using namespace TileGameLib;

int main(int argc, char* argv[]) {

	InitCommands();
	CreateWindow();

	Program* prog = new Program();

	if (argc > 1) {
		std::string programFile = argv[1];
		if (File::Exists(programFile)) {
			prog->Load(programFile);
		}
		else {
			MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, programFile));
			delete prog;
			return 1;
		}
	}
	else {
		std::string programFile = ShowBootMenu(Wnd.Ptr);
		if (!programFile.empty()) {
			prog->Load(programFile);
		}
		else {
			delete prog;
			return 0;
		}
	}

	if (prog->Validate()) {
		Wnd.Ptr->SetTitle("");
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
