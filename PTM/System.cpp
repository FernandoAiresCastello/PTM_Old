#include <map>
#include <string>
#include <vector>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "System.h"
#include "Parameter.h"
#include "System.h"
#include "Interpreter.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

std::string Title = "";
int* Memory = nullptr;
int MemSize = 0;
std::map<std::string, int> Ptr;
TWindow* Wnd = nullptr;
int CmpResult = 0;

#define OP(x)	Op[#x] = &x

void InitCommands()
{
	OP(NOP);	// No operation
	OP(EXIT);	// Exit program normally
	OP(ABORT);	// Exit program with error
	OP(ALLOC);	// Set memory size
	OP(PTR);	// Define named pointer to memory address
	OP(SET);	// Set value into memory address
	OP(CSTR);	// Insert string literal starting at specified address
	OP(MSGBOX);	// Show message box
	OP(TITLE);	// Set program title
	OP(HALT);	// Stop program execution until window is closed
	OP(WINDOW); // Open window
	OP(REFR);	// Refresh screen
	OP(OUT);	// Output tile to screen
	OP(ADD);	// Add to memory value
	OP(CMP);	// Compare with memory value
	OP(JMP);	// Jump
	OP(JE);		// Jump if equal
	OP(JNE);	// Jump if not equal
	OP(JG);		// Jump if greater
	OP(JGE);	// Jump if greater or equal
	OP(JL);		// Jump if less
	OP(JLE);	// Jump if less or equal
}

void NOP()
{
	Argc(0);
}
void EXIT()
{
	Argc(0);
	Exit = true;
}
void ABORT()
{
	Argc(1);
	Exit = true;
	std::string error = ArgStringLiteral();
	Abort(error, false);
}
void ALLOC()
{
	Argc(1);
	int size = ArgNumberLiteral();
	MemSize = size;
	Memory = new int[size];
	for (int i = 0; i < size; i++)
		Memory[i] = 0;
}
void PTR()
{
	Argc(2);
	std::string id = ArgIdentifier();
	int addr = ArgNumber();
	Ptr[id] = addr;
}
void SET()
{
	Argc(2);
	std::string id = ArgIdentifier();
	int value = ArgNumber();
	Memory[Ptr[id]] = value;
}
void CSTR()
{
	Argc(2);
	std::string id = ArgIdentifier();
	std::string str = ArgString();
	int addr = Ptr[id];

	for (int i = 0; i < str.length(); i++) {
		Memory[addr] = str[i];
		addr++;
	}
}
void MSGBOX()
{
	Argc(1);
	std::string msg = ArgString();
	MsgBox::Info(Title, msg);
}
void TITLE()
{
	Argc(1);
	Title = ArgString();
	if (Wnd)
		Wnd->SetTitle(Title);
}
void HALT()
{
	Argc(0);
	while (!Exit)
		ProcessGlobalEvents();
}
void WINDOW()
{
	Argc(4);
	int wBuf = ArgNumber();
	int hBuf = ArgNumber();
	int wWnd = ArgNumber();
	int hWnd = ArgNumber();

	Wnd = new TWindow(wBuf, hBuf, wWnd, hWnd, false);
	Wnd->SetTitle(Title);
}
void REFR()
{
	if (Wnd)
		Wnd->Update();
}
void OUT()
{
	Argc(6);
	int x = ArgNumber();
	int y = ArgNumber();
	int tile = ArgNumber();
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	if (transparent)
		Wnd->DrawTileTransparent(tile, fgc, bgc, x, y);
	else
		Wnd->DrawTile(tile, fgc, bgc, x, y);
}
void ADD()
{
	Argc(2);
	std::string id = ArgIdentifier();
	int value = ArgNumber();
	int curValue = Memory[Ptr[id]];
	Memory[Ptr[id]] = curValue + value;
}
void CMP()
{
	Argc(2);
	std::string id = ArgIdentifier();
	int value = ArgNumber();
	int curValue = Memory[Ptr[id]];
	CmpResult = curValue - value;
}
void JMP()
{
	Argc(1);
	Jump(ArgLabel());
}
void JE()
{
	Argc(1);
	if (CmpResult == 0)
		Jump(ArgLabel());
}
void JNE()
{
	Argc(1);
	if (CmpResult != 0)
		Jump(ArgLabel());
}
void JG()
{
	Argc(1);
	if (CmpResult > 0)
		Jump(ArgLabel());
}
void JGE()
{
	Argc(1);
	if (CmpResult >= 0)
		Jump(ArgLabel());
}
void JL()
{
	Argc(1);
	if (CmpResult < 0)
		Jump(ArgLabel());
}
void JLE()
{
	Argc(1);
	if (CmpResult <= 0)
		Jump(ArgLabel());
}
