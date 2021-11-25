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
	OP(CSTR);
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
	ResolveArg(Arg(0), true);
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
void CSTR()
{
	Req(2);
	if (Arg(1)->Type != ParameterType::StringLiteral) {
		Abort("String literal expected");
		return;
	}
	ResolveArg(Arg(0));
	int addr = Arg(0)->NumberValue;
	AssertMemAddr(addr);
	ResolveArg(Arg(1));
	for (int i = 0; i < Arg(1)->StringValue.length(); i++) {
		char ch = Arg(1)->StringValue[i];
		Memory[addr] = ch;
		addr++;
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
	std::string arg = Arg(0)->StringValue;
	ResolveArg(Arg(0), true);
	MsgBox::Info(Title, Arg(0)->StringValue);
}
void TITLE()
{
	Req(1);
	ResolveArg(Arg(0), true);
	Title = Arg(0)->StringValue;
}
