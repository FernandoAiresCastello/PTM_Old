#include "System.h"
#include "SystemGlobal.h"

void InitCommands()
{
	OP(NOP);
	OP(EXIT);
	OP(ABORT);
	OP(MSGBOX);
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
	REQ(0);

	Parameter* arg = ARG;
	if (arg)
		Abort(arg->StringValue);
	else
		Abort("Program aborted");
}
void MSGBOX()
{
	REQ(1);

	if (ARGC == 1) {
		std::string message = ARG->StringValue;
		MsgBox::Info("", message);
	}
	else if (ARGC == 2) {
		std::string title = ARG->StringValue;
		std::string message = ARG->StringValue;
		MsgBox::Info(title, message);
	}
}
