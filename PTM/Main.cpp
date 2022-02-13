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
	
	std::string ptmExePath = argv[0];
	int ixLastBackslash = String::FindLast(ptmExePath, '\\');
	BaseDir = String::GetFirstChars(ptmExePath, ixLastBackslash + 1);
	
	InitCommands();
	CreateWindow(4, 4, 43, 24);
	
	bool shouldExit = false;

	while (true) {
		Program* prog = new Program();

		if (!NewProgram.empty()) { // A program has requested to run a different program
			bool loadOk = prog->Load(BaseDir + NewProgram);
			if (!loadOk) {
				OnExit();
				return EXIT_FAILURE;
			}
		}
		else if (argc > 1) { // User provided a program file as argument
			std::string programFile = argv[1];
			bool loadOk = prog->Load(BaseDir + programFile);
			if (!loadOk) {
				OnExit();
				return EXIT_FAILURE;
			}
		}
		else { // User started machine without providing a program file
			if (File::Exists(BaseDir + AUTORUN_FILE1)) {
				prog->Load(BaseDir + AUTORUN_FILE1);
			}
			if (File::Exists(BaseDir + AUTORUN_FILE2)) {
				prog->Load(BaseDir + AUTORUN_FILE2);
			}
			else { // Autorun file not found
				MsgBox::Error(APP_NAME, Error.ProgramFileNotSpecified);
				OnExit();
				return EXIT_FAILURE;
			}
		}

		if (prog->Validate()) {
			InitSystem();
			InitInterpreter(prog);
			RunMainThread(); // Does NOT return until program execution ends
			shouldExit = !Reset && NewProgram.empty();
		}
		else { // Program is invalid
			OnExit();
			return EXIT_FAILURE;
		}

		// Normal exit point
		if (shouldExit) {
			OnExit();
			return EXIT_SUCCESS;
		}

		OnReset();
	}
	
	// Application should never reach this point
	MsgBox::Error(APP_NAME, Error.UnexpectedError);
	OnExit();
	return EXIT_FAILURE;
}
