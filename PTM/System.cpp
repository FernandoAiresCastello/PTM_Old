#include "System.h"

#define OP(x)	Op[#x] = &x

std::map<std::string, Variable> Vars;
int CmpResult = 0;
SystemWindow Wnd;
TSound* Snd = nullptr;

void InitSystem()
{
	Snd = new TSound();
}

void DestroySystem()
{
	delete Wnd.Ptr;
	Wnd.Ptr = nullptr;
	delete Snd;
	Snd = nullptr;
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
void HALT()
{
	Argc(0);
	while (!Exit);
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
void FLS()
{
	Argc(4);
	int tile = ArgNumber();
	int fgc = ArgNumber();
	int bgc = ArgNumber();
	bool transparent = ArgNumber() <= 0;

	for (int y = 0; y < Wnd.Ptr->Rows; y++) {
		for (int x = 0; x < Wnd.Ptr->Cols; x++) {
			if (transparent)
				Wnd.Ptr->DrawTileTransparent(tile, fgc, bgc, x * TChar::Width, y * TChar::Height);
			else
				Wnd.Ptr->DrawTile(tile, fgc, bgc, x * TChar::Width, y * TChar::Height);
		}
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
void JP()
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
void FWND()
{
	Argc(1);
	Wnd.FullScreenRequest = ArgNumber() > 0;
}
void UPDS()
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
void SSND()
{
	Argc(1);
	int value = ArgNumber();
	if (value == 0) {
		Snd->StopMainSound();
		Snd->StopSubSound();
	}
	else if (value == 1) {
		Snd->StopMainSound();
	}
	else if (value == 2) {
		Snd->StopSubSound();
	}
	else {
		Abort(String::Format(Error.IllegalArgumentValue, String::ToString(value).c_str()));
	}
}
void SWFT()
{
	Argc(1);
	int type = ArgNumber();
	if (type < 0 || type > (int)TSoundType::Noise) 
		Abort(String::Format(Error.IllegalArgumentValue, String::ToString(type).c_str()));
	else
		Snd->SetType((TSoundType)type);
}
void VOL()
{
	Argc(1);
	Snd->SetVolume(ArgNumber());
}
void BEEP()
{
	Argc(2);
	int freq = ArgNumber();
	int length = ArgNumber();
	Snd->Beep(freq, length);
}
void PLAY()
{
	Argc(1);
	Snd->PlaySubSound(ArgString());
}
void LPLAY()
{
	Argc(1);
	Snd->PlayMainSound(ArgString());
}
void PAUSE()
{
	Argc(1);
	int ms = ArgNumber();
	SDL_Delay(ms);
}
void READ()
{
	Argc(2);
	std::string id = ArgVariableName(true);
	if (Vars[id].Type != VariableType::String) {
		Abort(Error.TypeMismatch);
		return;
	}
	std::string path = ArgString();
	AssertFileExists(path);
	std::string data = File::ReadText(path);
	Vars[id].String = data;
}
void WRITE()
{
	Argc(2);
	std::string data = ArgString();
	std::string filePath = ArgString();
	File::WriteText(filePath, data);
}
void DEL()
{
	Argc(1);
	std::string path = ArgString();
	AssertFileExists(path);
	File::Delete(path);
}
void InitCommands()
{
	//=== MISC ===
	OP(NOP);	// No operation

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
	OP(JP);		// Jump
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
	OP(ADD);	// Add to number variable
	
	//=== COMPARE ===
	OP(CMP);	// Compare value with number variable
	
	//=== DEBUG ===
	OP(MSGB);	// Show message box

	//=== WINDOW ===
	OP(WINDOW); // Open window
	OP(TITLE);	// Set window title

	//=== GRAPHICS ===
	OP(FWND);	// Enable/disable fullscreen mode
	OP(UPDS);	// Enable/disable automatic screen update
	OP(CLS);	// Clear screen
	OP(OUTM);	// Select output mode
	OP(OUT);	// Output tile to screen
	OP(OUTS);	// Output string of tiles to screen
	OP(FLS);	// Fill screen with same tile
	OP(PAL);	// Set palette color
	OP(CHR);	// Set charset data

	//=== INPUT ===
	OP(IN);		// Get key pressed

	//=== SOUND ===
	OP(SSND);	// Stop all currently playing sounds
	OP(SWFT);	// Set waveform type
	OP(VOL);	// Set sound volume
	OP(BEEP);	// Play a single beep
	OP(PLAY);	// Play notes from a sound string (once)
	OP(LPLAY);	// Play notes from a sound string (loop)

	//=== FILESYSTEM ===
	OP(READ);	// Read file into string
	OP(WRITE);	// Write string to file
	OP(DEL);	// Delete file
}
