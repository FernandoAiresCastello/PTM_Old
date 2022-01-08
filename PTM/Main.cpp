#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "GlobalDefs.h"
#include "Program.h"
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
#include "Boot.h"
using namespace CppUtils;
using namespace TileGameLib;

int main(int argc, char* argv[]) {

	InitCommands();

	bool bootOk = LoadBootFile();
	if (!bootOk)
		return 1;

	Program* prog = new Program();

	if (!Boot.Autorun.empty()) {
		prog->Load(Boot.Autorun);
		if (prog->Validate()) {
			InitMachine(prog);
			RunMachine();
			DestroyMachine();
		}
		else {
			delete prog;
			return 1;
		}
	}

	return 0;
}
