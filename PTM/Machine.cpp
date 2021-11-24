#include "Machine.h"

bool Running;
Program Prog;

void InitMachine(Program& prog)
{
	Running = false;
	Prog = prog;
}

void DestroyMachine()
{
	// close window, delete stuff, etc
}

void RunMachine()
{
	Running = true;

	while (Running) {

	}
}
