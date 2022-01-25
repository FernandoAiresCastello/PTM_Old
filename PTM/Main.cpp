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
	CreateWindow(800, 600);
	
	bool bootMenu = false;
	bool destroyWindowAndExit = false;

	while (true) {
		Program* prog = new Program();

		if (!NewProgram.empty()) { // A program has requested to run a different program
			prog->Load(NewProgram);
			NewProgram = "";
		}
		else if (argc > 1) { // User provided a program file as argument, will run it automatically
			std::string programFile = argv[1];
			if (File::Exists(programFile)) {
				prog->Load(programFile);
			}
			else {
				MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, programFile.c_str()));
				DestroyWindow();
				delete prog;
				return EXIT_FAILURE;
			}
		}
		else { // User started machine without providing a program file
			auto files = File::List(".", "*.ptml", false);

			if (files.empty()) { // There are no program files around, will abort immediately
				MsgBox::Error(APP_NAME, Error.NoProgramFound);
				DestroyWindow();
				delete prog;
				return EXIT_FAILURE;
			}
			else if (files.size() == 1) { // There is only 1 program file, will run it automatically
				prog->Load(files[0]);
			}
			else { // There are multiple program files, start boot menu
				bootMenu = true;
				std::string programFile = ShowBootMenu(Wnd.Ptr, files);
				if (!programFile.empty()) {
					prog->Load(programFile);
				}
				else {
					DestroyWindow();
					delete prog;
					return EXIT_SUCCESS;
				}
			}
		}

		if (prog->Validate()) {
			InitMachine(prog);
			RunMachine();
			DestroyMachine();
			destroyWindowAndExit = !bootMenu && NewProgram.empty();
		}
		else {
			DestroyWindow();
			delete prog;
			return EXIT_FAILURE;
		}

		if (destroyWindowAndExit) {
			DestroyWindow();
			return EXIT_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}
