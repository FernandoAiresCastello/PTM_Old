#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
#include "Machine.h"
#include "System.h"
using namespace CppUtils;
using namespace TileGameLib;

int main(int argc, char* argv[]) {
	
	if (argc < 2) {
		MsgBox::Error("PTM", "Missing program file path");
		return 1;
	}

	std::string path = argv[1];
	if (!File::Exists(path)) {
		MsgBox::Error("PTM", "Program file not found: " + path);
		return 1;
	}

	InitCommands();

	Program* prog = new Program();
	prog->Load(path);
	
	if (prog->Lines.empty()) {
		MsgBox::Error("PTM", "Program has no executable code");
		delete prog;
		return 1;
	}
	
	if (!prog->Errors.empty()) {
		std::string errors = "";
		for (auto& err : prog->Errors) {
			errors.append(err + "\n");
		}
		MsgBox::Error("PTM", errors);
		return 1;
	}

	InitMachine(prog);
	RunMachine();
	DestroyMachine();
	
	delete prog;
	return 0;
}
