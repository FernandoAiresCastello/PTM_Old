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

	bool bootMenu = false;

	do {
		if (!bootMenu)
			CreateWindow();

		Program* prog = new Program();

		if (argc > 1) {
			bootMenu = false;
			std::string programFile = argv[1];
			if (File::Exists(programFile)) {
				prog->Load(programFile);
			}
			else {
				MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, programFile.c_str()));
				delete prog;
				return 1;
			}
		}
		else {
			bootMenu = true;
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
			DestroyMachine(!bootMenu);
		}
		else {
			delete prog;
			return 1;
		}

	} while (bootMenu);

	return 0;
}
