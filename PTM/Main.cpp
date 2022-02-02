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
	CreateWindow(1024, 768, 2, 2);
	
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
			if (File::Exists("autorun")) {
				prog->Load("autorun");
			}
			else {
				MsgBox::Error(APP_NAME, Error.ProgramFileNotSpecified);
				DestroyWindow();
				delete prog;
				return EXIT_FAILURE;
			}
		}

		if (prog->Validate()) {
			InitInterpreter(prog);
			RunInterpreter();
			DestroyInterpreter();
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
