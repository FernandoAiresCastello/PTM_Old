#include "System.h"

#define OP(x)	Op[#x] = &x

std::string Title = "";
int* Memory = nullptr;
int MemSize = 0;
std::map<std::string, int> Addr;
TWindow* Wnd = nullptr;
int CmpResult = 0;
int DataPtr = 0;
int NextMemAddr = 0;
enum class OutputMode { Free, Tiled };
OutputMode OutMode = OutputMode::Free;
bool WindowCreationRequested;
int RequestedWindowWBuf;
int RequestedWindowHBuf;
int RequestedWindowWWnd;
int RequestedWindowHWnd;

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
void SET()
{
	Argc(2);
	std::string id = ArgIdentifier(false);
	int value = ArgNumber();
	if (Addr.find(id) == Addr.end())
		Addr[id] = NextMemAddr;

	Memory[Addr[id]] = value;
	NextMemAddr++;
}
void SETP()
{
	Argc(2);
	std::string id = ArgIdentifier(true);
	int value = ArgNumber();
	Memory[Memory[Addr[id]]] = value;
}
void CSTR()
{
	Argc(3);
	std::string id = ArgIdentifier(false);
	int length = ArgNumber();
	std::string str = ArgString();

	if (Addr.find(id) == Addr.end())
		Addr[id] = NextMemAddr;

	int addr = NextMemAddr;
	for (int i = 0; i < length; i++)
		Memory[addr++] = 0;

	addr = NextMemAddr;
	for (int i = 0; i < str.length() && i < length; i++)
		Memory[addr++] = str[i];

	Memory[addr++] = 0;
	NextMemAddr += length + 1;
}
void MSGB()
{
	Argc(1);
	std::string msg = ArgString();
	MsgBox::Info(Title, msg);
}
void TITLE()
{
	Argc(1);
	Title = ArgString();
}
void HALT()
{
	Argc(0);
	while (!Exit);
}
void WINDOW()
{
	Argc(4);
	RequestedWindowWBuf = ArgNumber();
	RequestedWindowHBuf = ArgNumber();
	RequestedWindowWWnd = ArgNumber();
	RequestedWindowHWnd = ArgNumber();
	WindowCreationRequested = true;

	while (!Wnd)
		SDL_Delay(1);
}
void OUTM()
{
	Argc(1);
	OutMode = (OutputMode)ArgNumber();
}
void OUT()
{
	Argc(6);
	int tile = ArgNumber();
	int x = OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int y = OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	if (transparent)
		Wnd->DrawTileTransparent(tile, fgc, bgc, x, y);
	else
		Wnd->DrawTile(tile, fgc, bgc, x, y);
}
void OUTS()
{
	Argc(6);
	std::string str = ArgString();
	int x = OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int y = OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	int tile = 0;
	for (int i = 0; i < str.length(); i++) {
		tile = str[i];
		if (transparent)
			Wnd->DrawTileTransparent(tile, fgc, bgc, x, y);
		else
			Wnd->DrawTile(tile, fgc, bgc, x, y);

		x += TChar::Width;
	}
}
void ADD()
{
	Argc(2);
	std::string id = ArgIdentifier(true);
	int value = ArgNumber();
	int curValue = Memory[Addr[id]];
	Memory[Addr[id]] = curValue + value;
}
void CMP()
{
	Argc(2);
	std::string id = ArgIdentifier(true);
	int value = ArgNumber();
	int curValue = Memory[Addr[id]];
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
void CALL()
{
	Argc(1);
	Call(ArgLabel());
}
void RET()
{
	Argc(0);
	Return();
}
void CLS()
{
	Argc(1);
	Wnd->SetBackColor(ArgNumber());
	Wnd->Clear();
}
void PAL()
{
	Argc(2);
	int ix = ArgNumber();
	int rgb = ArgNumber();
	Wnd->GetPalette()->Set(ix, rgb);
}
void CHR()
{
	Argc(9);
	int ix = ArgNumber();
	int r1 = ArgNumber();
	int r2 = ArgNumber();
	int r3 = ArgNumber();
	int r4 = ArgNumber();
	int r5 = ArgNumber();
	int r6 = ArgNumber();
	int r7 = ArgNumber();
	int r8 = ArgNumber();
	Wnd->GetCharset()->Set(ix, r1, r2, r3, r4, r5, r6, r7, r8);
}
void DATA()
{
	Argc(8);
	int v1 = ArgNumber();
	int v2 = ArgNumber();
	int v3 = ArgNumber();
	int v4 = ArgNumber();
	int v5 = ArgNumber();
	int v6 = ArgNumber();
	int v7 = ArgNumber();
	int v8 = ArgNumber();

	Memory[DataPtr++] = v1;
	Memory[DataPtr++] = v2;
	Memory[DataPtr++] = v3;
	Memory[DataPtr++] = v4;
	Memory[DataPtr++] = v5;
	Memory[DataPtr++] = v6;
	Memory[DataPtr++] = v7;
	Memory[DataPtr++] = v8;
}
void DATP()
{
	Argc(1);
	DataPtr = ArgNumber();
}
void RND()
{
	Argc(3);
	std::string id = ArgIdentifier(true);
	int min = ArgNumber();
	int max = ArgNumber();
	int rnd = Util::Random(min, max);
	Memory[Addr[id]] = rnd;
}
void IN()
{
	Argc(1);
	std::string id = ArgIdentifier(true);
	Memory[Addr[id]] = KeyPressed;
	KeyPressed = 0;
}

void PAUSE()
{
	Argc(1);
	int ms = ArgNumber();
	SDL_Delay(ms);
}

void InitCommands()
{
	//=== MEMORY ===
	OP(ALLOC);	// Set memory size
	OP(SET);	// Set value into memory address
	OP(SETP);	// Set value into memory address through pointer
	OP(CSTR);	// Insert string literal into memory
	OP(DATP);	// Set data pointer
	OP(DATA);	// Insert values starting at the data pointer
	OP(RND);	// Set random value into memory address

	//=== PROGRAM FLOW ===
	OP(EXIT);	// Exit program normally
	OP(HALT);	// Stop program execution
	OP(JMP);	// Jump
	OP(JE);		// Jump if equal
	OP(JNE);	// Jump if not equal
	OP(JG);		// Jump if greater
	OP(JGE);	// Jump if greater or equal
	OP(JL);		// Jump if less
	OP(JLE);	// Jump if less or equal
	OP(CALL);	// Call
	OP(RET);	// Return
	OP(PAUSE);	// Pause program execution

	//=== MATH ===
	OP(ADD);	// Add to memory value
	
	//=== COMPARE ===
	OP(CMP);	// Compare with memory value
	
	//=== DEBUG ===
	OP(MSGB);	// Show message box
	OP(ABORT);	// Exit program with error

	//=== GRAPHICS / WINDOW ===
	OP(WINDOW); // Open window
	OP(TITLE);	// Set window title
	OP(CLS);	// Clear screen
	OP(OUTM);	// Select output mode
	OP(OUT);	// Output tile to screen
	OP(OUTS);	// Output tile string to screen
	OP(PAL);	// Set palette color
	OP(CHR);	// Set charset data

	//=== INPUT ===
	OP(IN);	// Get key pressed

	//=== MISC ===
	OP(NOP);	// No operation
}
