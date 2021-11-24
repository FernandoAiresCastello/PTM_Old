#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
#include "Machine.h"
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

	Program prog;
	prog.Load(path);
	
	if (!prog.Errors.empty()) {
		std::string errors = "";
		for (auto& err : prog.Errors) {
			errors.append(err + "\n");
		}
		MsgBox::Error("PTM", String::Format(
			"Program %s contains errors:\n\n%s", path.c_str(), errors.c_str()));

		return 1;
	}

	InitMachine(prog);
	RunMachine();
	DestroyMachine();

	return 0;
}
