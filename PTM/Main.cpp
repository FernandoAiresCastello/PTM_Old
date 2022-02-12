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

#define AUTORUN_FILE1 "autorun.ptml"
#define AUTORUN_FILE2 "Autorun.ptml"

int main(int argc, char* argv[]) {
	
	InitCommands();
	CreateWindow(4, 4, 43, 24);
	
	bool bootMenu = false;
	bool destroyWindowAndExit = false;

	while (true) {
		Program* prog = new Program();

		if (!NewProgram.empty()) { // A program has requested to run a different program
			prog->Load(NewProgram);
			NewProgram = "";
		}
		else if (argc > 1) { // User provided a program file as argument
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
			if (File::Exists(AUTORUN_FILE1)) {
				prog->Load(AUTORUN_FILE1);
			}
			if (File::Exists(AUTORUN_FILE2)) {
				prog->Load(AUTORUN_FILE2);
			}
			else { // Autorun file not found
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
		else { // Program is invalid
			DestroyWindow();
			delete prog;
			return EXIT_FAILURE;
		}

		// Normal exit point
		if (destroyWindowAndExit) {
			DestroyWindow();
			return EXIT_SUCCESS;
		}
	}
	
	// Application should never reach this point
	MsgBox::Error(APP_NAME, Error.UnexpectedError);
	return EXIT_FAILURE;
}
