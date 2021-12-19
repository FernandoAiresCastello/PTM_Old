#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

#define APP_NAME "PTM"
#define BOOT_FILENAME "boot"

int main(int argc, char* argv[]) {
	
	if (!File::Exists(BOOT_FILENAME)) {
		MsgBox::Error(APP_NAME, Error.BootFileNotFound);
		return 1;
	}
	auto bootFile = File::ReadLines(BOOT_FILENAME);
	if (bootFile.size() < 1) {
		MsgBox::Error(APP_NAME, Error.InvalidBootFile);
		return 1;
	}
	std::string bootProgFile = bootFile[0];
	if (!File::Exists(bootProgFile)) {
		MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, bootProgFile.c_str()));
		return 1;
	}

	InitCommands();
	Program* prog = new Program();
	prog->Load(bootProgFile);
	
	if (prog->Lines.empty()) {
		MsgBox::Error(APP_NAME, String::Format(Error.InvalidProgramFile, bootProgFile.c_str()));
		delete prog;
		return 1;
	}
	if (!prog->Errors.empty()) {
		MsgBox::Error(APP_NAME, prog->Errors[0]);
		return 1;
	}

	InitMachine(prog);
	RunMachine();
	DestroyMachine();
	
	delete prog;
	return 0;
}
