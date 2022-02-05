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
	CreateWindow(960, 720, 3, 3);
	
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
			if (File::Exists("autorun")) {
				prog->Load("autorun");
			}
			else { // No "autorun" found
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
			Wnd.Ptr->Hide();
			SDL_Delay(100);
			DestroyWindow();
			return EXIT_SUCCESS;
		}
	}
	
	// Application should never reach this point
	MsgBox::Error(APP_NAME, Error.UnexpectedError);
	return EXIT_FAILURE;
}
