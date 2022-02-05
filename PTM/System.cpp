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
	SetSystemVar("COLS", Wnd.Ptr->ScreenWidth / (Wnd.Ptr->GetPixelWidth() * 8));
	SetSystemVar("ROWS", Wnd.Ptr->ScreenHeight / (Wnd.Ptr->GetPixelHeight() * 8));
	SetSystemVar("CHR_SIZE", Wnd.Ptr->GetCharset()->GetSize());
	SetSystemVar("PAL_SIZE", Wnd.Ptr->GetPalette()->GetSize());

	SetSystemVar("KEY_UP", SDLK_UP);
	SetSystemVar("KEY_DOWN", SDLK_DOWN);
	SetSystemVar("KEY_LEFT", SDLK_LEFT);
	SetSystemVar("KEY_RIGHT", SDLK_RIGHT);
	SetSystemVar("KEY_SPACE", SDLK_SPACE);
	SetSystemVar("KEY_RETURN", SDLK_RETURN);
	SetSystemVar("KEY_ESC", SDLK_ESCAPE);
	SetSystemVar("KEY_TAB", SDLK_TAB);
	SetSystemVar("KEY_BS", SDLK_BACKSPACE);
	SetSystemVar("KEY_INS", SDLK_INSERT);
	SetSystemVar("KEY_DEL", SDLK_DELETE);
	SetSystemVar("KEY_HOME", SDLK_HOME);
	SetSystemVar("KEY_END", SDLK_END);
	SetSystemVar("KEY_PGUP", SDLK_PAGEUP);
	SetSystemVar("KEY_PGDN", SDLK_PAGEDOWN);
	SetSystemVar("KEY_F1", SDLK_F1);
	SetSystemVar("KEY_F2", SDLK_F2);
	SetSystemVar("KEY_F3", SDLK_F3);
	SetSystemVar("KEY_F4", SDLK_F4);
	SetSystemVar("KEY_F5", SDLK_F5);
	SetSystemVar("KEY_F6", SDLK_F6);
	SetSystemVar("KEY_F7", SDLK_F7);
	SetSystemVar("KEY_F8", SDLK_F8);
	SetSystemVar("KEY_F9", SDLK_F9);
	SetSystemVar("KEY_F10", SDLK_F10);
	SetSystemVar("KEY_F11", SDLK_F11);
	SetSystemVar("KEY_F12", SDLK_F12);
	SetSystemVar("KEY_1", SDLK_1);
	SetSystemVar("KEY_2", SDLK_2);
	SetSystemVar("KEY_3", SDLK_3);
	SetSystemVar("KEY_4", SDLK_4);
	SetSystemVar("KEY_5", SDLK_5);
	SetSystemVar("KEY_6", SDLK_6);
	SetSystemVar("KEY_7", SDLK_7);
	SetSystemVar("KEY_8", SDLK_8);
	SetSystemVar("KEY_9", SDLK_9);
	SetSystemVar("KEY_0", SDLK_0);
	SetSystemVar("KEY_Q", SDLK_q);
	SetSystemVar("KEY_U", SDLK_u);
	SetSystemVar("KEY_E", SDLK_e);
	SetSystemVar("KEY_R", SDLK_r);
	SetSystemVar("KEY_T", SDLK_t);
	SetSystemVar("KEY_Y", SDLK_y);
	SetSystemVar("KEY_U", SDLK_u);
	SetSystemVar("KEY_I", SDLK_i);
	SetSystemVar("KEY_O", SDLK_o);
	SetSystemVar("KEY_P", SDLK_p);
	SetSystemVar("KEY_A", SDLK_a);
	SetSystemVar("KEY_S", SDLK_s);
	SetSystemVar("KEY_D", SDLK_d);
	SetSystemVar("KEY_F", SDLK_f);
	SetSystemVar("KEY_G", SDLK_g);
	SetSystemVar("KEY_H", SDLK_h);
	SetSystemVar("KEY_J", SDLK_j);
	SetSystemVar("KEY_K", SDLK_k);
	SetSystemVar("KEY_L", SDLK_l);
	SetSystemVar("KEY_Z", SDLK_z);
	SetSystemVar("KEY_X", SDLK_x);
	SetSystemVar("KEY_C", SDLK_c);
	SetSystemVar("KEY_V", SDLK_v);
	SetSystemVar("KEY_B", SDLK_b);
	SetSystemVar("KEY_N", SDLK_n);
	SetSystemVar("KEY_M", SDLK_m);
}

void SetSystemVar(std::string name, int value)
{
	Variable var;
	var.Type = VariableType::Number;
	var.Number = value;
	var.String = String::ToString(value);
	Vars[SYS_VAR_PREFIX + String::ToUpper(name)] = var;
}

void SetSystemVar(std::string name, std::string value)
{
	Variable var;
	var.Type = VariableType::String;
	var.String = value;
	var.Number = String::ToInt(value);
	Vars[SYS_VAR_PREFIX + String::ToUpper(name)] = var;
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
	std::string file = ArgString();
	
	if (File::Exists(file)) {
		NewProgram = file;
		Exit = true;
	}
	else {
		NewProgram = "";
		Abort(String::Format(Error.ProgramFileNotFound, file.c_str()));
	}
}
void RESET()
{
	Argc(0);
	Exit = true;
	NewProgram = Prog->GetFilePath();
}
void VAR()
{
	Argc(2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::Number;
	var.Number = ArgNumber();
	Vars[id] = var;
}
void VAR$()
{
	Argc(2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::String;
	var.String = ArgString();
	Vars[id] = var;
}
void VAR_A()
{
	Argc(1);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::NumberArray;
	Vars[id] = var;
}
void VAR$_A()
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
void POP()
{
	Argc(2);
	auto id = ArgVariableName(true);
	int index = ArgNumber();
	AssertArrayIndex(id, index);

	if (Vars[id].Type == VariableType::StringArray) {
		auto it = Vars[id].StringArray.begin();
		Vars[id].StringArray.erase(it + index, it + index + 1);
	}
	else if (Vars[id].Type == VariableType::NumberArray) {
		auto it = Vars[id].NumberArray.begin();
		Vars[id].NumberArray.erase(it + index, it + index + 1);
	}
	else
		Abort(Error.TypeMismatch);
}
void POPA()
{
	Argc(1);
	auto id = ArgVariableName(true);
	if (Vars[id].Type == VariableType::StringArray)
		Vars[id].StringArray.clear();
	else if (Vars[id].Type == VariableType::NumberArray)
		Vars[id].NumberArray.clear();
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
		case 6: w = 1; h = 1; break;
		case 5: w = 2; h = 2; break;
		case 4: w = 3; h = 3; break;
		case 3: w = 4; h = 3; break;
		case 2: w = 3; h = 5; break;
		case 1: w = 5; h = 3; break;
		case 0: w = 5; h = 5; break;

		default:
			Abort(String::Format(Error.InvalidGraphicsMode, mode));
			return;
	}

	Wnd.Ptr->SetPixelSize(w, h);

	SetSystemVar("COLS", Wnd.Ptr->ScreenWidth / (Wnd.Ptr->GetPixelWidth() * 8));
	SetSystemVar("ROWS", Wnd.Ptr->ScreenHeight / (Wnd.Ptr->GetPixelHeight() * 8));
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
void CHRL()
{
	Argc(3);
	auto ix = ArgNumber();
	auto row = ArgNumber();
	auto data = ArgNumber();

	if (ix >= Wnd.Chr->GetSize()) {
		int blanksToAdd = ix - Wnd.Chr->GetSize() + 1;
		Wnd.Chr->AddBlank(blanksToAdd);
		SetSystemVar("CHR_SIZE", Wnd.Ptr->GetCharset()->GetSize());
	}

	Wnd.Chr->Set(ix, row, data);
}
void CHR()
{
	Argc(9);
	auto ix = ArgNumber();
	auto line0 = ArgNumber();
	auto line1 = ArgNumber();
	auto line2 = ArgNumber();
	auto line3 = ArgNumber();
	auto line4 = ArgNumber();
	auto line5 = ArgNumber();
	auto line6 = ArgNumber();
	auto line7 = ArgNumber();
	
	if (ix >= Wnd.Chr->GetSize()) {
		int blanksToAdd = Wnd.Chr->GetSize() - ix + 1;
		Wnd.Chr->AddBlank(blanksToAdd);
		SetSystemVar("CHR_SIZE", Wnd.Ptr->GetCharset()->GetSize());
	}

	Wnd.Chr->Set(ix, line0, line1, line2, line3, line4, line5, line6, line7);
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
void WAVE()
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
void READ_A()
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
void WRITE_A()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumberArray(id);
	auto path = ArgString();
	File::WriteBytes(path, Vars[id].NumberArray);
}
void CMPS()
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
void ADDS()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto str = ArgString();
	Vars[id].String = Vars[id].String + str;
}
void LEN()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	auto str = ArgString();
	Vars[id].Number = str.length();
}
void SPLIT()
{
	Argc(3);
	auto arr = ArgVariableName(true);
	AssertVariableIsTypeStringArray(arr);
	auto str = ArgString();
	auto delim = ArgString();
	
	auto parts = String::Split(str, delim, true);
	Vars[arr].StringArray = parts;
}
void TRIM()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto str = ArgString();
	Vars[id].String = String::Trim(str);
}
void GETC()
{
	Argc(3);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	auto str = ArgString();
	auto index = ArgNumber();
	if (index >= 0 && index < str.length())
		Vars[id].Number = str[index];
	else
		Abort(Error.StringIndexOutOfRange);
}
void SETC()
{
	Argc(3);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto index = ArgNumber();
	auto ch = ArgNumber();
	if (index >= 0 && index < Vars[id].String.length())
		Vars[id].String[index] = ch;
	else
		Abort(Error.StringIndexOutOfRange);
}
void ADDC()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto ch = ArgNumber();
	Vars[id].String.append(1, ch);
}
void INSC()
{
	Argc(3);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto index = ArgNumber();
	auto ch = ArgNumber();
	if (index >= 0 && index <= Vars[id].String.length())
		Vars[id].String.insert(Vars[id].String.begin() + index, ch);
	else
		Abort(Error.StringIndexOutOfRange);
}
void DELC()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto index = ArgNumber();
	if (index >= 0 && index < Vars[id].String.length()) {
		auto it = Vars[id].String.begin();
		Vars[id].String.erase(it + index, it + index + 1);
	}
	else
		Abort(Error.StringIndexOutOfRange);
}

void InitCommands()
{
	//=== MISC ===
	Op["NOP"] = &NOP;			// No operation
	Op["SYS"] = &SYS;			// Call internal system function

	//=== DEBUG ===
	Op["MSGB"] = &MSGB;			// Show message box

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

	//=== VARIABLES ===
	Op["VAR"] = &VAR;			// Declare variable as number
	Op["VAR$"] = &VAR$;			// Declare variable as string
	Op["VAR[]"] = &VAR_A;		// Declare variable as number array
	Op["VAR$[]"] = &VAR$_A;		// Declare variable as string array
	Op["SET"] = &SET;			// Set value to variable

	// === ARRAYS ===
	Op["PUSH"] = &PUSH;			// Push value into array
	Op["POP"] = &POP;			// Remove item from array index
	Op["POPA"] = &POPA;			// Remove all items from array
	Op["COUNT"] = &COUNT;		// Get number of items in array

	// === NUMBERS ===
	Op["RND"] = &RND;			// Get random number
	Op["CMP"] = &CMP;			// Compare two numeric values

	//=== MATH ===
	Op["ADD"] = &ADD;			// Add
	Op["SUB"] = &SUB;			// Subtract
	Op["MUL"] = &MUL;			// Multiply
	Op["DIV"] = &DIV;			// Divide
	Op["MOD"] = &MOD;			// Divide and get remainder
	Op["SQRT"] = &SQRT;			// Square root
	Op["INC"] = &INC;			// Increment by 1
	Op["DEC"] = &DEC;			// Decrement by 1

	//=== GRAPHICS ===
	Op["TITLE"] = &TITLE;		// Set window title
	Op["FSCR"] = &FSCR;			// Enable/disable fullscreen mode
	Op["GFXM"] = &GFXM;			// Set graphics mode
	Op["CLS"] = &CLS;			// Clear screen and set background color
	Op["OUTM"] = &OUTM;			// Select output mode
	Op["OUT"] = &OUT;			// Output tile to screen
	Op["OUTS"] = &OUTS;			// Output string of tiles to screen
	Op["FLS"] = &FLS;			// Fill screen with same tile
	Op["PAL"] = &PAL;			// Set palette color
	Op["CHR"] = &CHR;			// Set charset data
	Op["CHRL"] = &CHRL;			// Set charset data line
	Op["LDCHR"] = &LDCHR;		// Load charset data from image file
	Op["LDPAL"] = &LDPAL;		// Load palette data from image file
	Op["PRTSCN"] = &PRTSCN;		// Save screen image to file

	//=== INPUT ===
	Op["INPM"] = &INPM;			// Set input mode
	Op["INP"] = &INP;			// Get key pressed

	//=== SOUND ===
	Op["SNDOFF"] = &SNDOFF;		// Stop all currently playing sounds
	Op["WAVE"] = &WAVE;			// Set waveform type
	Op["VOL"] = &VOL;			// Set sound volume
	Op["BEEP"] = &BEEP;			// Play a single beep
	Op["PLAY"] = &PLAY;			// Play notes from a sound string (once)
	Op["LPLAY"] = &LPLAY;		// Play notes from a sound string (loop)

	//=== FILESYSTEM ===
	Op["READ"] = &READ;			// Read file into string
	Op["READ[]"] = &READ_A;		// Read file bytes into number array
	Op["WRITE"] = &WRITE;		// Write string to file
	Op["WRIT[]"] = &WRITE_A;	// Write bytes from number array to file

	//=== STRINGS ===
	Op["CMPS"] = &CMPS;			// Compare strings
	Op["ADDS"] = &ADDS;			// Append string
	Op["LEN"] = &LEN;			// Get length of string
	Op["SPLIT"] = &SPLIT;		// Split string into array
	Op["TRIM"] = &TRIM;			// Trim string
	Op["GETC"] = &GETC;			// Get character at index
	Op["SETC"] = &SETC;			// Set character at index
	Op["ADDC"] = &ADDC;			// Append character
	Op["INSC"] = &INSC;			// Insert character at index
	Op["DELC"] = &DELC;			// Delete character at index
}
