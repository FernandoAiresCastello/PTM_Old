#include "System.h"

#define OP(x)	Op[#x] = &x

std::map<std::string, Variable> Vars;
int CmpResult = 0;
SystemWindow Wnd;

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
void NUM()
{
	Argc(2);
	std::string id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::Number;
	var.Number = ArgNumber();
	Vars[id] = var;
}
void STR()
{
	Argc(2);
	std::string id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::String;
	var.String = ArgString();
	Vars[id] = var;
}
void NUM_ARRAY()
{
	Argc(1);
	std::string id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::NumberArray;
	Vars[id] = var;
}
void STR_ARRAY()
{
	Argc(1);
	std::string id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::StringArray;
	Vars[id] = var;
}
void SET()
{
	Argc(2);
	if (Arg(0)->Type == ParameterType::Identifier) {
		std::string id = ArgVariableName(true);
		Variable& var = Vars[id];
		if (var.Type == VariableType::Number)
			var.Number = ArgNumber();
		else if (var.Type == VariableType::String)
			var.String = ArgString();
		else
			Abort(Error.TypeMismatch);
	}
	else if (Arg(0)->Type == ParameterType::ArrayIndexLiteral) {
		Parameter* arg = Arg();
		std::string& id = arg->StringValue;
		AssertVariable(id, true);
		Variable& var = Vars[id];
		int index = arg->ArrayIndex;
		AssertArrayIndex(id, index);
		if (var.Type == VariableType::NumberArray)
			var.NumberArray[index] = ArgNumber();
		else if (var.Type == VariableType::StringArray)
			var.StringArray[index] = ArgString();
		else
			Abort(Error.TypeMismatch);
	}
	else if (Arg(0)->Type == ParameterType::ArrayIndexVariable) {
		Parameter* arg0 = Arg(0);
		Parameter* arg1 = Arg(1);
		std::string& id = arg0->StringValue;
		AssertVariable(id, true);
		Variable& var = Vars[id];
		std::string& arrIxId = arg1->StringValue;

		if (Vars[arrIxId].Type != VariableType::NumberArray && Vars[arrIxId].Type != VariableType::StringArray) {
			Abort(Error.TypeMismatch);
			return;
		}
		if (Vars[arg0->VariableArrayIndex].Type != VariableType::Number) {
			Abort(Error.TypeMismatch);
			return;
		}

		int index0 = Vars[arg0->VariableArrayIndex].Number;
		int index1 = Vars[arg1->VariableArrayIndex].Number;
		AssertArrayIndex(id, index0);
		AssertArrayIndex(id, index1);

		if (var.Type == VariableType::NumberArray)
			var.NumberArray[index0] = var.NumberArray[index1];
		else if (var.Type == VariableType::StringArray)
			var.StringArray[index0] = var.StringArray[index1];
		else
			Abort(Error.TypeMismatch);
	}
	else {
		Abort(Error.SyntaxError);
	}
}
void PUSH()
{
	Argc(2);
	std::string id = ArgVariableName(true);
	Variable& var = Vars[id];
	if (var.Type == VariableType::NumberArray)
		var.NumberArray.push_back(ArgNumber());
	else if (var.Type == VariableType::StringArray)
		var.StringArray.push_back(ArgString());
	else
		Abort(Error.TypeMismatch);
}
void COUNT()
{
	Argc(2);
	std::string idResult = ArgVariableName(true);
	std::string idArray = ArgVariableName(true);

	if (Vars[idResult].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return;
	}

	if (Vars[idArray].Type == VariableType::StringArray) {
		Vars[idResult].Number = Vars[idArray].StringArray.size();
	}
	else if (Vars[idArray].Type == VariableType::NumberArray) {
		Vars[idResult].Number = Vars[idArray].NumberArray.size();
	}
	else {
		Abort(Error.TypeMismatch);
		return;
	}
}
void MSGB()
{
	Argc(1);
	std::string msg = ArgString();
	MsgBox::Info(Wnd.Title, msg);
}
void TITLE()
{
	Argc(1);
	Wnd.Title = ArgString();
}
void HALT()
{
	Argc(0);
	while (!Exit);
}
void WINDOW()
{
	Argc(4);
	Wnd.BufWidth = ArgNumber();
	Wnd.BufHeight = ArgNumber();
	Wnd.WndWidth = ArgNumber();
	Wnd.WndHeight = ArgNumber();
	Wnd.CreationRequested = true;

	while (!Wnd.Ptr)
		SDL_Delay(1);
}
void OUTM()
{
	Argc(1);
	Wnd.OutMode = (OutputMode)ArgNumber();
}
void OUT()
{
	Argc(6);
	int tile = ArgNumber();
	int x = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int y = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	if (transparent)
		Wnd.Ptr->DrawTileTransparent(tile, fgc, bgc, x, y);
	else
		Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y);
}
void OUTS()
{
	Argc(6);
	std::string str = ArgString();
	int x = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int y = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	int tile = 0;
	for (int i = 0; i < str.length(); i++) {
		tile = str[i];
		if (transparent)
			Wnd.Ptr->DrawTileTransparent(tile, fgc, bgc, x, y);
		else
			Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y);

		x += TChar::Width;
	}
}
void ADD()
{
	Argc(2);
	std::string id = ArgVariableName(true);
	int value = ArgNumber();

	if (Vars[id].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return;
	}

	Vars[id].Number = Vars[id].Number + value;
}
void CMP()
{
	Argc(2);
	std::string id = ArgVariableName(true);
	int value = ArgNumber();

	if (Vars[id].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return;
	}

	CmpResult = Vars[id].Number - value;
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
void DISP()
{
	Argc(1);
	Wnd.AutoUpdate = ArgNumber() > 0;
}
void CLS()
{
	Argc(1);
	Wnd.Ptr->SetBackColor(ArgNumber());
	Wnd.Ptr->Clear();
}
void PAL()
{
	Argc(2);
	int ix = ArgNumber();
	int rgb = ArgNumber();
	Wnd.Ptr->GetPalette()->Set(ix, rgb);
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
	Wnd.Ptr->GetCharset()->Set(ix, r1, r2, r3, r4, r5, r6, r7, r8);
}
void RND()
{
	Argc(3);
	std::string id = ArgVariableName(true);
	int min = ArgNumber();
	int max = ArgNumber();
	int rnd = Util::Random(min, max);
	if (Vars[id].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return;
	}
	Vars[id].Number = rnd;
}
void IN()
{
	Argc(1);
	std::string id = ArgVariableName(true);
	if (Vars[id].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return;
	}
	Vars[id].Number = KeyPressed;
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
	//=== VARIABLES ===
	OP(NUM);	// Declare variable as number
	OP(STR);	// Declare variable as string
	Op["NUM[]"] = &NUM_ARRAY;	// Declare variable as number array
	Op["STR[]"] = &STR_ARRAY;	// Declare variable as string array
	OP(SET);	// Set value to variable
	OP(PUSH);	// Push value into array
	OP(COUNT);	// Get number of items in array
	OP(RND);	// Set random number value variable

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

	//=== WINDOW ===
	OP(WINDOW); // Open window
	OP(TITLE);	// Set window title

	//=== GRAPHICS ===
	OP(DISP);	// Enable or disable screen
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
