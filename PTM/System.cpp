#include "System.h"
#include "Boot.h"

std::map<std::string, Variable> Vars;
int KeyPressed = 0;
int CmpResult = 0;
SystemWindow Wnd;
TSound* Snd = nullptr;

void InitSystem()
{
	Snd = new TSound();

	Wnd.BufWidth = Boot.ResWidth;
	Wnd.BufHeight = Boot.ResHeight;
	Wnd.WndWidth = Boot.WndWidth;
	Wnd.WndHeight = Boot.WndHeight;
	Wnd.CreationRequested = true;
}

void ResetSystem()
{
	Vars.clear();
	KeyPressed = 0;
	CmpResult = 0;
	Snd->StopMainSound();
	Snd->StopSubSound();
}

void DestroySystem()
{
	delete Wnd.Ptr;
	Wnd.Ptr = nullptr;
	delete Snd;
	Snd = nullptr;
}

void CreateWindow()
{
	if (Wnd.Ptr) {
		Abort(Error.WindowAlreadyOpen);
		return;
	}

	Wnd.CreationRequested = false;
	Wnd.Ptr = new TWindow(
		Wnd.BufWidth, Wnd.BufHeight,
		Wnd.WndWidth, Wnd.WndHeight, false);

	Wnd.Pal = Wnd.Ptr->GetPalette();
	Wnd.Chr = Wnd.Ptr->GetCharset();
}

void UpdateWindow()
{
	if (!Wnd.Ptr)
		return;

	if (Wnd.Title != Wnd.OldTitle) {
		Wnd.OldTitle = Wnd.Title;
		Wnd.Ptr->SetTitle(Wnd.Title);
	}
	if (Wnd.FullScreenRequest >= 0) {
		Wnd.Ptr->SetFullscreen(Wnd.FullScreenRequest);
		Wnd.FullScreenRequest = -1;
	}
	if (Wnd.AutoUpdate) {
		Wnd.Ptr->Update();
	}
	else {
		SDL_Delay(1);
	}
}

void Print(std::string str, int x, int y)
{
	x *= TChar::Width;
	y *= TChar::Height;

	const auto px = x;
	auto tile = 0;

	for (int i = 0; i < str.length(); i++) {
		tile = str[i];
		AssertTileIndex(tile);

		if (i < str.length() - 1 && tile == '\\') {
			i++;
			if (str[i] == 'n') {
				x = px;
				y += TChar::Height;
			}
		}
		else {
			Wnd.Ptr->DrawTileTransparent(tile, 15, 0, x, y);
			x += TChar::Width;
		}
	}
}

void Delay(int cycles)
{
	for (int i = 0; i < cycles; i++) {
		ProcessGlobalEvents();
		Wnd.Ptr->Update();
		if (Exit)
			break;

		SDL_Delay(1);
	}
}

void ShowIntro()
{
	if (!Boot.ShowIntro)
		return;

	Wnd.Ptr->SetBackColor(0x00);
	Wnd.Ptr->Clear();
	Wnd.Ptr->Update();
	Delay(50);

	std::string intro = "PTM 0.1";
	int x = Wnd.Ptr->Cols / 2 - intro.length() / 2;
	int y = Wnd.Ptr->Rows / 2 - 1;

	Print(intro, x, y);
	Wnd.Ptr->Update();
	Snd->PlaySubSound("c6 50 d6 70 c6 80 d6 120");
	Delay(256);

	Wnd.Ptr->SetBackColor(0x00);
	Wnd.Ptr->Clear();
	Wnd.Ptr->Update();
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
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::Number;
	var.Number = ArgNumber();
	Vars[id] = var;
}
void STR()
{
	Argc(2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::String;
	var.String = ArgString();
	Vars[id] = var;
}
void NUM_ARRAY()
{
	Argc(1);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::NumberArray;
	Vars[id] = var;
}
void STR_ARRAY()
{
	Argc(1);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::StringArray;
	Vars[id] = var;
}
void SET()
{
	Argc(2);
	if (Arg(0)->Type == ParameterType::Identifier) {
		auto id = ArgVariableName(true);
		auto& var = Vars[id];
		if (var.Type == VariableType::Number)
			var.Number = ArgNumber();
		else if (var.Type == VariableType::String)
			var.String = ArgString();
		else
			Abort(Error.TypeMismatch);
	}
	else if (Arg(0)->Type == ParameterType::ArrayIndexLiteral) {
		auto* arg = Arg();
		auto& id = arg->StringValue;
		AssertVariable(id, true);
		auto& var = Vars[id];
		auto index = arg->ArrayIndex;
		AssertArrayIndex(id, index);
		if (var.Type == VariableType::NumberArray)
			var.NumberArray[index] = ArgNumber();
		else if (var.Type == VariableType::StringArray)
			var.StringArray[index] = ArgString();
		else
			Abort(Error.TypeMismatch);
	}
	else if (Arg(0)->Type == ParameterType::ArrayIndexVariable) {
		auto* arg0 = Arg(0);
		auto* arg1 = Arg(1);
		auto& id = arg0->StringValue;
		AssertVariable(id, true);
		auto& var = Vars[id];
		auto& arrIxId = arg1->StringValue;

		if (Vars[arrIxId].Type != VariableType::NumberArray && Vars[arrIxId].Type != VariableType::StringArray) {
			Abort(Error.TypeMismatch);
			return;
		}
		if (Vars[arg0->VariableArrayIndex].Type != VariableType::Number) {
			Abort(Error.TypeMismatch);
			return;
		}

		auto index0 = Vars[arg0->VariableArrayIndex].Number;
		AssertArrayIndex(id, index0);
		auto index1 = Vars[arg1->VariableArrayIndex].Number;
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
	auto id = ArgVariableName(true);
	auto& var = Vars[id];
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
	auto idResult = ArgVariableName(true);
	AssertVariableIsTypeNumber(idResult);
	auto idArray = ArgVariableName(true);

	if (Vars[idArray].Type == VariableType::StringArray)
		Vars[idResult].Number = Vars[idArray].StringArray.size();
	else if (Vars[idArray].Type == VariableType::NumberArray)
		Vars[idResult].Number = Vars[idArray].NumberArray.size();
	else
		Abort(Error.TypeMismatch);
}
void MSGB()
{
	Argc(1);
	auto msg = ArgString();
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

	if (Wnd.Ptr) {
		Abort(Error.WindowAlreadyOpen);
		return;
	}

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
	auto tile = ArgNumber();
	auto x = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	auto y = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	auto fgc = ArgNumber();
	auto bgc = ArgNumber();
	auto transparent = ArgNumber() <= 0;
	
	AssertTileIndex(tile);
	AssertPaletteIndex(fgc);
	AssertPaletteIndex(bgc);

	if (transparent)
		Wnd.Ptr->DrawTileTransparent(tile, fgc, bgc, x, y);
	else
		Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y);
}
void OUTS()
{
	Argc(6);
	auto str = ArgString();
	auto x = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	auto y = Wnd.OutMode == OutputMode::Free ? ArgNumber() : ArgNumber() * TChar::Width;
	auto fgc = ArgNumber();
	auto bgc = ArgNumber();
	auto transparent = ArgNumber() <= 0;
	const auto px = x;

	AssertPaletteIndex(fgc);
	AssertPaletteIndex(bgc);

	int tile = 0;
	for (int i = 0; i < str.length(); i++) {
		tile = str[i];
		AssertTileIndex(tile);

		if (i < str.length() - 1 && tile == '\\') {
			i++;
			if (str[i] == 'n') {
				x = px;
				y += TChar::Height;
			}
		}
		else {
			if (transparent)
				Wnd.Ptr->DrawTileTransparent(tile, fgc, bgc, x, y);
			else
				Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y);

			x += TChar::Width;
		}
	}
}
void FLS()
{
	Argc(4);
	auto tile = ArgNumber();
	auto fgc = ArgNumber();
	auto bgc = ArgNumber();
	auto transparent = ArgNumber() <= 0;

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
	auto id = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = Vars[id].Number + value;
}
void CMP()
{
	Argc(2);
	auto id = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(id);
	CmpResult = Vars[id].Number - value;
}
void JP()
{
	Argc(1);
	Jump(ArgLabel());
}
void JP_E()
{
	Argc(1);
	if (CmpResult == 0)
		Jump(ArgLabel());
}
void JP_NE()
{
	Argc(1);
	if (CmpResult != 0)
		Jump(ArgLabel());
}
void JP_G()
{
	Argc(1);
	if (CmpResult > 0)
		Jump(ArgLabel());
}
void JP_GE()
{
	Argc(1);
	if (CmpResult >= 0)
		Jump(ArgLabel());
}
void JP_L()
{
	Argc(1);
	if (CmpResult < 0)
		Jump(ArgLabel());
}
void JP_LE()
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
void CALL_E()
{
	Argc(1);
	if (CmpResult == 0)
		Call(ArgLabel());
}
void CALL_NE()
{
	Argc(1);
	if (CmpResult != 0)
		Call(ArgLabel());
}
void CALL_G()
{
	Argc(1);
	if (CmpResult > 0)
		Call(ArgLabel());
}
void CALL_GE()
{
	Argc(1);
	if (CmpResult >= 0)
		Call(ArgLabel());
}
void CALL_L()
{
	Argc(1);
	if (CmpResult < 0)
		Call(ArgLabel());
}
void CALL_LE()
{
	Argc(1);
	if (CmpResult <= 0)
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
	auto ix = ArgNumber();
	auto rgb = ArgNumber();
	Wnd.Pal->Set(ix, rgb);
}
void CHR()
{
	Argc(9);
	auto ix = ArgNumber();
	auto r1 = ArgNumber();
	auto r2 = ArgNumber();
	auto r3 = ArgNumber();
	auto r4 = ArgNumber();
	auto r5 = ArgNumber();
	auto r6 = ArgNumber();
	auto r7 = ArgNumber();
	auto r8 = ArgNumber();
	Wnd.Chr->Set(ix, r1, r2, r3, r4, r5, r6, r7, r8);
}
void LDCHR()
{
	Argc(1);
	auto file = ArgString();
	AssertFileExists(file);
	Wnd.Chr->LoadFromImage(file);
}
void LDPAL()
{
	Argc(3);
	auto file = ArgString();
	auto wSwatch = ArgNumber();
	auto hSwatch = ArgNumber();
	AssertFileExists(file);
	Wnd.Pal->LoadFromImage(file, wSwatch, hSwatch);
}
void PRTSCN()
{
	Argc(1);
	auto file = ArgString();
	Wnd.Ptr->SaveScreenshot(file);
}
void COLS()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = Wnd.Ptr->Cols;
}
void ROWS()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = Wnd.Ptr->Rows;
}
void RND()
{
	Argc(3);
	auto id = ArgVariableName(true);
	auto min = ArgNumber();
	auto max = ArgNumber();
	auto rnd = Util::Random(min, max);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = rnd;
}
void IN()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = KeyPressed;
	KeyPressed = 0;
}
void SSND()
{
	Argc(1);
	auto value = ArgNumber();
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
	auto type = ArgNumber();
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
	auto freq = ArgNumber();
	auto length = ArgNumber();
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
	auto ms = ArgNumber();
	SDL_Delay(ms);
}
void FREAD()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto path = ArgString();
	AssertFileExists(path);
	auto data = File::ReadText(path);
	Vars[id].String = data;
}
void FWRITE()
{
	Argc(2);
	auto data = ArgString();
	auto filePath = ArgString();
	File::WriteText(filePath, data);
}
void FDEL()
{
	Argc(1);
	auto path = ArgString();
	AssertFileExists(path);
	File::Delete(path);
}
void InitCommands()
{
	//=== MISC ===
	Op["NOP"] = &NOP;			// No operation

	//=== VARIABLES ===
	Op["NUM"] = &NUM;			// Declare variable as number
	Op["STR"] = &STR;			// Declare variable as string
	Op["NUM[]"] = &NUM_ARRAY;	// Declare variable as number array
	Op["STR[]"] = &STR_ARRAY;	// Declare variable as string array
	Op["SET"] = &SET;			// Set value to variable
	Op["PUSH"] = &PUSH;			// Push value into array
	Op["COUNT"] = &COUNT;		// Get number of items in array
	Op["RND"] = &RND;			// Set random number value variable

	//=== PROGRAM FLOW ===
	Op["EXIT"] = &EXIT;			// Exit program normally
	Op["HALT"] = &HALT;			// Stop program execution
	Op["JP"] = &JP;				// Jump
	Op["JP=" ] = &JP_E;			// Jump if equal
	Op["JP!="] = &JP_NE;		// Jump if not equal
	Op["JP>" ] = &JP_G;			// Jump if greater
	Op["JP>="] = &JP_GE;		// Jump if greater or equal
	Op["JP<" ] = &JP_L;			// Jump if less
	Op["JP<="] = &JP_LE;		// Jump if less or equal
	Op["CALL"] = &CALL;			// Call
	Op["CALL="] = &CALL_E;		// Call if equal
	Op["CALL!="] = &CALL_NE;	// Call if not equal
	Op["CALL>"] = &CALL_G;		// Call if greater
	Op["CALL>="] = &CALL_GE;	// Call if greater or equal
	Op["CALL<"] = &CALL_L;		// Call if less
	Op["CALL<="] = &CALL_LE;	// Call if less or equal
	Op["RET"] = &RET;			// Return
	Op["PAUSE"] = &PAUSE;		// Pause program execution
	
	//=== MATH ===
	Op["ADD"] = &ADD;			// Add to number variable
	
	//=== COMPARE ===
	Op["CMP"] = &CMP;			// Compare value with number variable
	
	//=== DEBUG ===
	Op["MSGB"] = &MSGB;			// Show message box

	//=== WINDOW ===
	Op["WINDOW"] = &WINDOW;		// Open window
	Op["TITLE"] = &TITLE;		// Set window title

	//=== GRAPHICS ===
	Op["FWND"] = &FWND;			// Enable/disable fullscreen mode
	Op["UPDS"] = &UPDS;			// Enable/disable automatic screen update
	Op["CLS"] = &CLS;			// Clear screen
	Op["OUTM"] = &OUTM;			// Select output mode
	Op["OUT"] = &OUT;			// Output tile to screen
	Op["OUTS"] = &OUTS;			// Output string of tiles to screen
	Op["FLS"] = &FLS;			// Fill screen with same tile
	Op["PAL"] = &PAL;			// Set palette color
	Op["CHR"] = &CHR;			// Set charset data
	Op["LDCHR"] = &LDCHR;		// Load charset data from image file
	Op["LDPAL"] = &LDPAL;		// Load palette data from image file
	Op["PRTSCN"] = &PRTSCN;		// Save screenshot
	Op["COLS"] = &COLS;			// Get screen column count
	Op["ROWS"] = &ROWS;			// Get screen row count

	//=== INPUT ===
	Op["IN"] = &IN;				// Get key pressed

	//=== SOUND ===
	Op["SSND"] = &SSND;			// Stop all currently playing sounds
	Op["SWFT"] = &SWFT;			// Set waveform type
	Op["VOL"] = &VOL;			// Set sound volume
	Op["BEEP"] = &BEEP;			// Play a single beep
	Op["PLAY"] = &PLAY;			// Play notes from a sound string (once)
	Op["LPLAY"] = &LPLAY;		// Play notes from a sound string (loop)

	//=== FILESYSTEM ===
	Op["FREAD"] = &FREAD;		// Read file into string
	Op["FWRITE"] = &FWRITE;		// Write string to file
	Op["FDEL"] = &FDEL;			// Delete file
}
