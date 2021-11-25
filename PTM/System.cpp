#include "System.h"
#include "SystemGlobal.h"

std::string Title = "";
int* Memory = nullptr;
int MemSize = 0;
std::map<std::string, int> VarPtr;
TWindow* Wnd = nullptr;

void InitCommands()
{
	OP(NOP);	// No operation
	OP(EXIT);	// Exit program normally
	OP(ABORT);	// Exit program with error
	OP(ALLOC);	// Set memory size
	OP(CSTR);	// Insert string literal starting at specified address
	OP(SET);	// Set value into memory address
	OP(PTR);	// Define named pointer to memory address
	OP(INC);	// Increment
	OP(DEC);	// Decrement
	OP(MSGBOX);	// Show message box
	OP(TITLE);	// Set program title
	OP(WINDOW); // Open window
	OP(HALT);	// Stop program execution until window is closed
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
	AssertArgType(Arg(0), ParameterType::NumberLiteral);
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
	AssertArgType(Arg(0), ParameterType::Identifier);
	AssertArgType(Arg(1), ParameterType::StringLiteral);
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
	AssertArgType(Arg(0), ParameterType::Identifier);
	ResolveArg(Arg(1));
	std::string ident = Arg(0)->StringValue;
	AssertMemAddr(Arg(1)->NumberValue);
	VarPtr[ident] = Arg(1)->NumberValue;
}
void INC()
{
	Req(1);
	if (Arg(0)->Type == ParameterType::PointerAccess) {
		std::string ident = Arg(0)->StringValue;
		int addr = VarPtr[ident];
		AssertMemAddr(addr);
		Memory[addr] = Memory[addr] + 1;
	}
	else if (Arg(0)->Type == ParameterType::Identifier) {
		std::string ident = Arg(0)->StringValue;
		int addr = VarPtr[ident];
		AssertMemAddr(addr);
		VarPtr[ident] = addr + 1;
	}
	else {
		Abort("Invalid parameter type");
	}
}
void DEC()
{
	Req(1);
	if (Arg(0)->Type == ParameterType::PointerAccess) {
		std::string ident = Arg(0)->StringValue;
		int addr = VarPtr[ident];
		AssertMemAddr(addr);
		Memory[addr] = Memory[addr] - 1;
	}
	else if (Arg(0)->Type == ParameterType::Identifier) {
		std::string ident = Arg(0)->StringValue;
		int addr = VarPtr[ident];
		AssertMemAddr(addr);
		VarPtr[ident] = addr - 1;
	}
	else {
		Abort("Invalid parameter type");
	}
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
	if (Wnd) {
		Wnd->SetTitle(Title);
	}
}
void WINDOW()
{
	Req(3);
	ResolveArg(Arg(0));
	ResolveArg(Arg(1));
	ResolveArg(Arg(2));

	int width = Arg(0)->NumberValue;
	int height = Arg(1)->NumberValue;
	int zoom = Arg(2)->NumberValue;

	Wnd = new TWindow(width, height, zoom, false);
	Wnd->SetTitle(Title);
}

void HALT()
{
	Req(0);
	while (!Exit) {
		ProcessGlobalEvents();
	}
}
