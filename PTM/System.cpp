#include "System.h"
#include "SystemGlobal.h"

std::string Title = "";
int* Memory = nullptr;
int MemSize = 0;
std::map<std::string, int> VarPtr;

void InitCommands()
{
	OP(NOP);
	OP(EXIT);
	OP(ABORT);
	OP(ALLOC);
	OP(SET);
	OP(PTR);
	OP(MSGBOX);
	OP(TITLE);
}
void NOP()
{
	Req(0);
}
void EXIT()
{
	Req(0);
	Exit = true;
}
void ABORT()
{
	Req(1);
	ResolveArg(Arg(0));
	Abort(Arg(0)->StringValue);
}
void ALLOC()
{
	Req(1);
	ResolveArg(Arg(0));
	int size = Arg(0)->NumberValue;
	MemSize = size;
	Memory = new int[size];
	for (int i = 0; i < size; i++) {
		Memory[i] = 0;
	}
}
void SET()
{
	Req(2);
	ResolveArg(Arg(1));
	std::string& identifier = Arg(0)->StringValue;
	AssertPtrExists(identifier);
	AssertMemAddr(Arg(1)->NumberValue);
	Memory[VarPtr[identifier]] = Arg(1)->NumberValue;
}
void PTR()
{
	Req(2);
	ResolveArg(Arg(1));
	std::string ident = Arg(0)->StringValue;
	AssertMemAddr(Arg(1)->NumberValue);
	VarPtr[ident] = Arg(1)->NumberValue;
}
void MSGBOX()
{
	Req(1);
	ResolveArg(Arg(0));
	std::string& message = Arg(0)->StringValue;
	MsgBox::Info(Title, message);
}
void TITLE()
{
	Req(1);
	ResolveArg(Arg(0));
	Title = Arg(0)->StringValue;
}
