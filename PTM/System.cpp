#include <map>
#include <string>
#include <vector>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Parameter.h"
#include "System.h"
#include "Machine.h"
using namespace CppUtils;
using namespace TileGameLib;

#define OP(x) Op[#x] = &x

void InitCommands()
{
	OP(NOP);
	OP(EXIT);
	OP(ABORT);
}

void NOP()
{
	// No operation
}

void EXIT()
{
	Exit = true;
}

void ABORT()
{
	Parameter* arg = Arg();
	if (arg)
		Abort(arg->StringValue);
	else
		Abort("Program aborted");
}
