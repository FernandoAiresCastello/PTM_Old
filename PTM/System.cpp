#include "System.h"

std::map<std::string, Variable> Vars;
int KeyPressed = 0;
int CmpResult = 0;
SystemWindow Wnd;
TSound* Snd = nullptr;
int DefaultPixelWidth = 0;
int DefaultPixelHeight = 0;

void InitSystem()
{
	Snd = new TSound();
	InitSystemVars();
}

void InitSystemVars()
{
	SetSystemVar("SCR.COLS", Wnd.Ptr->ScreenWidth / (Wnd.Ptr->GetPixelWidth() * 8));
	SetSystemVar("SCR.ROWS", Wnd.Ptr->ScreenHeight / (Wnd.Ptr->GetPixelHeight() * 8));

	SetSystemVar("KB.UP", SDLK_UP);
	SetSystemVar("KB.DOWN", SDLK_DOWN);
	SetSystemVar("KB.LEFT", SDLK_LEFT);
	SetSystemVar("KB.RIGHT", SDLK_RIGHT);
	SetSystemVar("KB.SPACE", SDLK_SPACE);
	SetSystemVar("KB.RETURN", SDLK_RETURN);
	SetSystemVar("KB.ESC", SDLK_ESCAPE);
	SetSystemVar("KB.TAB", SDLK_TAB);
	SetSystemVar("KB.BS", SDLK_BACKSPACE);
	SetSystemVar("KB.INS", SDLK_INSERT);
	SetSystemVar("KB.DEL", SDLK_DELETE);
	SetSystemVar("KB.HOME", SDLK_HOME);
	SetSystemVar("KB.END", SDLK_END);
	SetSystemVar("KB.PGUP", SDLK_PAGEUP);
	SetSystemVar("KB.PGDN", SDLK_PAGEDOWN);
	SetSystemVar("KB.F1", SDLK_F1);
	SetSystemVar("KB.F2", SDLK_F2);
	SetSystemVar("KB.F3", SDLK_F3);
	SetSystemVar("KB.F4", SDLK_F4);
	SetSystemVar("KB.F5", SDLK_F5);
	SetSystemVar("KB.F6", SDLK_F6);
	SetSystemVar("KB.F7", SDLK_F7);
	SetSystemVar("KB.F8", SDLK_F8);
	SetSystemVar("KB.F9", SDLK_F9);
	SetSystemVar("KB.F10", SDLK_F10);
	SetSystemVar("KB.F11", SDLK_F11);
	SetSystemVar("KB.F12", SDLK_F12);
	SetSystemVar("KB.1", SDLK_1);
	SetSystemVar("KB.2", SDLK_2);
	SetSystemVar("KB.3", SDLK_3);
	SetSystemVar("KB.4", SDLK_4);
	SetSystemVar("KB.5", SDLK_5);
	SetSystemVar("KB.6", SDLK_6);
	SetSystemVar("KB.7", SDLK_7);
	SetSystemVar("KB.8", SDLK_8);
	SetSystemVar("KB.9", SDLK_9);
	SetSystemVar("KB.0", SDLK_0);
	SetSystemVar("KB.Q", SDLK_q);
	SetSystemVar("KB.U", SDLK_u);
	SetSystemVar("KB.E", SDLK_e);
	SetSystemVar("KB.R", SDLK_r);
	SetSystemVar("KB.T", SDLK_t);
	SetSystemVar("KB.Y", SDLK_y);
	SetSystemVar("KB.U", SDLK_u);
	SetSystemVar("KB.I", SDLK_i);
	SetSystemVar("KB.O", SDLK_o);
	SetSystemVar("KB.P", SDLK_p);
	SetSystemVar("KB.A", SDLK_a);
	SetSystemVar("KB.S", SDLK_s);
	SetSystemVar("KB.D", SDLK_d);
	SetSystemVar("KB.F", SDLK_f);
	SetSystemVar("KB.G", SDLK_g);
	SetSystemVar("KB.H", SDLK_h);
	SetSystemVar("KB.J", SDLK_j);
	SetSystemVar("KB.K", SDLK_k);
	SetSystemVar("KB.L", SDLK_l);
	SetSystemVar("KB.Z", SDLK_z);
	SetSystemVar("KB.X", SDLK_x);
	SetSystemVar("KB.C", SDLK_c);
	SetSystemVar("KB.V", SDLK_v);
	SetSystemVar("KB.B", SDLK_b);
	SetSystemVar("KB.N", SDLK_n);
	SetSystemVar("KB.M", SDLK_m);
}

void SetSystemVar(std::string name, int value)
{
	Variable var;
	var.Type = VariableType::Number;
	var.Number = value;
	var.String = String::ToString(value);
	Vars["$" + String::ToUpper(name)] = var;
}

void SetSystemVar(std::string name, std::string value)
{
	Variable var;
	var.Type = VariableType::String;
	var.String = value;
	var.Number = String::ToInt(value);
	Vars["$" + String::ToUpper(name)] = var;
}

void ResetSystem()
{
	Vars.clear();
	InitSystemVars();
	KeyPressed = 0;
	CmpResult = 0;
	Snd->StopMainSound();
	Snd->StopSubSound();

	if (Wnd.Ptr) {
		Wnd.Pal->InitDefault();
		Wnd.Chr->InitDefault();
		Wnd.Ptr->SetTitle("");
		Wnd.Ptr->SetBackColor(0);
		Wnd.Ptr->SetPixelSize(DefaultPixelWidth, DefaultPixelHeight);
		Wnd.Ptr->Clear();
		Wnd.Ptr->Update();
	}
}

void DestroySystem()
{
	delete Snd;
	Snd = nullptr;
}

void DestroyWindow()
{
	delete Wnd.Ptr;
	Wnd.Ptr = nullptr;
}

void CreateWindow(int width, int height, int pixelWidth, int pixelHeight)
{
	if (Wnd.Ptr) {
		Abort(Error.WindowAlreadyOpen);
		return;
	}

	DefaultPixelWidth = pixelWidth;
	DefaultPixelHeight = pixelHeight;

	Wnd.Ptr = new TWindow(width, height, width, height, false);
	Wnd.Ptr->SetPixelSize(pixelWidth, pixelHeight);
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

	Wnd.Ptr->Update();
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
			Wnd.Ptr->DrawTile(tile, 15, 0, x, y, true, true);
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
	Wnd.Ptr->SetBackColor(0x00);
	Wnd.Ptr->Clear();
	Wnd.Ptr->Update();
	Delay(50);

	std::string intro = APP_VERSION;
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

/*#############################################################################
 
							COMMAND IMPLEMENTATION

#############################################################################*/

void NOP()
{
	Argc(0);
}
void SYS()
{
	Argc(1);
	auto fn = String::ToLower(ArgString());
	Abort(Error.SystemFunctionNotFound);
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
void RUN()
{
	Argc(1);
	Exit = true;
	NewProgram = ArgString();
}
void RESET()
{
	Argc(0);
	Exit = true;
	NewProgram = Prog->GetFilePath();
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
		if (String::StartsWith(id, '$')) {
			Abort(Error.IllegalVariableName);
			return;
		}
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
void OUTM()
{
	Argc(1);
	Wnd.OutMode = (OutputMode)ArgNumber();
}
void OUT()
{
	Argc(6);
	auto tile = ArgNumber();
	auto x = ArgNumber();
	auto y = ArgNumber();
	auto fgc = ArgNumber();
	auto bgc = ArgNumber();
	auto transparent = ArgNumber() <= 0;
	
	AssertTileIndex(tile);
	AssertPaletteIndex(fgc);
	AssertPaletteIndex(bgc);

	Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y, transparent, Wnd.OutMode == OutputMode::Tiled);
}
void OUTS()
{
	Argc(6);
	auto str = ArgString();
	auto x = ArgNumber();
	auto y = ArgNumber();
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
				y++;
			}
		}
		else {
			Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y, transparent, Wnd.OutMode == OutputMode::Tiled);
			x++;
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
			Wnd.Ptr->DrawTile(tile, fgc, bgc, x * TChar::Width, y * TChar::Height, transparent, true);
		}
	}
}
void ADD()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto b = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(a);
	AssertVariableIsTypeNumber(b);
	Vars[a].Number = Vars[b].Number + value;
}
void SUB()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto b = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(a);
	AssertVariableIsTypeNumber(b);
	Vars[a].Number = Vars[b].Number - value;
}
void MUL()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto b = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(a);
	AssertVariableIsTypeNumber(b);
	Vars[a].Number = Vars[b].Number * value;
}
void DIV()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto b = ArgVariableName(true);
	auto value = ArgNumber();
	if (value == 0) {
		Abort("Division by zero");
		return;
	}
	AssertVariableIsTypeNumber(a);
	AssertVariableIsTypeNumber(b);
	Vars[a].Number = Vars[b].Number / value;
}
void MOD()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto b = ArgVariableName(true);
	auto value = ArgNumber();
	if (value == 0) {
		Abort("Division by zero");
		return;
	}
	AssertVariableIsTypeNumber(a);
	AssertVariableIsTypeNumber(b);
	Vars[a].Number = Vars[b].Number % value;
}
void SQRT()
{
	Argc(3);
	auto a = ArgVariableName(true);
	auto value = ArgNumber();
	AssertVariableIsTypeNumber(a);
	Vars[a].Number = floor(sqrt(value));
}
void INC()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number++;
}
void DEC()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number--;
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
void FSCR()
{
	Argc(1);
	Wnd.FullScreenRequest = ArgNumber() > 0;
}
void GFXM()
{
	Argc(1);
	int mode = ArgNumber();
	int w = 0;
	int h = 0;

	switch (mode) {
		case 0: w = 2; h = 2; break;
		case 1: w = 2; h = 3; break;
		case 2: w = 4; h = 3; break;
		case 3: w = 8; h = 4; break;
		case 4: w = 4; h = 8; break;
		default:
			Abort(String::Format(Error.InvalidGraphicsMode, mode));
			return;
	}

	Wnd.Ptr->SetPixelSize(w, h);

	SetSystemVar("SCR.COLS", Wnd.Ptr->ScreenWidth / (Wnd.Ptr->GetPixelWidth() * 8));
	SetSystemVar("SCR.ROWS", Wnd.Ptr->ScreenHeight / (Wnd.Ptr->GetPixelHeight() * 8));
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
	Argc(3);
	auto ix = ArgNumber();
	auto row = ArgNumber();
	auto data = ArgNumber();
	Wnd.Chr->Set(ix, row, data);
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
void INPM()
{
	Argc(1);
	Wnd.InMode = (InputMode)ArgNumber();
}
void INP()
{
	Argc(1);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	Vars[id].Number = KeyPressed;
	KeyPressed = 0;
}
void SNDOFF()
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
void READ()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto path = ArgString();
	AssertFileExists(path);
	auto data = File::ReadText(path);
	Vars[id].String = data;
}
void READ_ARRAY()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumberArray(id);
	auto path = ArgString();
	AssertFileExists(path);
	auto data = File::ReadBytes(path);
	Vars[id].NumberArray.clear();

	for (auto value : data)
		Vars[id].NumberArray.push_back((int)value);
}
void WRITE()
{
	Argc(2);
	auto data = ArgString();
	auto filePath = ArgString();
	File::WriteText(filePath, data);
}
void WRITE_ARRAY()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumberArray(id);
	auto path = ArgString();
	File::WriteBytes(path, Vars[id].NumberArray);
}
void DEL()
{
	Argc(1);
	auto path = ArgString();
	AssertFileExists(path);
	File::Delete(path);
}
void STRCMP()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto expectedValue = ArgString();
	auto actualValue = Vars[id].String;

	if (expectedValue == actualValue)
		CmpResult = 0;
	else
		CmpResult = -1;
}
void STRCAT()
{
	Argc(3);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto str1 = ArgString();
	auto str2 = ArgString();
	Vars[id].String = str1 + str2;
}
void STRLEN()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	auto str = ArgString();
	Vars[id].Number = str.length();
}

void InitCommands()
{
	//=== MISC ===
	Op["NOP"] = &NOP;			// No operation
	Op["SYS"] = &SYS;			// Call internal system function

	//=== VARIABLES ===
	Op["VAR"] = &NUM;			// Declare variable as number
	Op["VAR$"] = &STR;			// Declare variable as string
	Op["VAR[]"] = &NUM_ARRAY;	// Declare variable as number array
	Op["VAR$[]"] = &STR_ARRAY;	// Declare variable as string array
	Op["SET"] = &SET;			// Set value to variable
	Op["PUSH"] = &PUSH;			// Push value into array
	Op["COUNT"] = &COUNT;		// Get number of items in array
	Op["RND"] = &RND;			// Get random number
	Op["CMP"] = &CMP;			// Compare two numeric values

	//=== PROGRAM FLOW ===
	Op["EXIT"] = &EXIT;			// End program and close window
	Op["HALT"] = &HALT;			// End program but keep window open
	Op["JP"] = &JP;				// Jump
	Op["JP="] = &JP_E;			// Jump if equal
	Op["JP!="] = &JP_NE;		// Jump if not equal
	Op["JP>"] = &JP_G;			// Jump if greater
	Op["JP>="] = &JP_GE;		// Jump if greater or equal
	Op["JP<"] = &JP_L;			// Jump if less
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
	Op["RESET"] = &RESET;		// Reset machine
	Op["RUN"] = &RUN;			// Reset machine with a different program
	
	//=== MATH ===
	Op["ADD"] = &ADD;			// Add
	Op["SUB"] = &SUB;			// Subtract
	Op["MUL"] = &MUL;			// Multiply
	Op["DIV"] = &DIV;			// Divide
	Op["MOD"] = &MOD;			// Divide and get remainder
	Op["SQRT"] = &SQRT;			// Square root
	Op["INC"] = &INC;			// Increment by 1
	Op["DEC"] = &DEC;			// Decrement by 1
	
	//=== DEBUG ===
	Op["MSGB"] = &MSGB;			// Show message box

	//=== WINDOW / GRAPHICS ===
	Op["TITLE"] = &TITLE;		// Set window title
	Op["FSCR"] = &FSCR;			// Enable/disable fullscreen mode
	Op["GFXM"] = &GFXM;			// Set graphics mode
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

	//=== INPUT ===
	Op["INPM"] = &INPM;			// Set input mode
	Op["INP"] = &INP;			// Get key pressed

	//=== SOUND ===
	Op["SNDOFF"] = &SNDOFF;		// Stop all currently playing sounds
	Op["SWFT"] = &SWFT;			// Set waveform type
	Op["VOL"] = &VOL;			// Set sound volume
	Op["BEEP"] = &BEEP;			// Play a single beep
	Op["PLAY"] = &PLAY;			// Play notes from a sound string (once)
	Op["LPLAY"] = &LPLAY;		// Play notes from a sound string (loop)

	//=== FILESYSTEM ===
	Op["READ"] = &READ;			// Read file into string
	Op["READ[]"] = &READ_ARRAY;	// Read file bytes into number array
	Op["WRITE"] = &WRITE;		// Write string to file
	Op["WRIT[]"] = &WRITE_ARRAY;// Write bytes from number array to file
	Op["DEL"] = &DEL;			// Delete file

	//=== STRING ===
	Op["STRCMP"] = &STRCMP;		// Compare value with string variable
	Op["STRCAT"] = &STRCAT;		// Concatenate two strings
	Op["STRLEN"] = &STRLEN;		// Get length of string
}
