#include "System.h"

std::string BaseDir = "";
SDL_Thread* MachineThread = nullptr;
std::string TempSrcCode = "";
std::string TempSrcCodeFile = "";
std::map<std::string, Variable> Vars;
const Uint8* Keyboard = nullptr;
int CmpResult = 0;
SystemWindow Wnd;
SDL_Event Event = { 0 };
TSound* Snd = nullptr;
int DefaultPixelWidth = 0;
int DefaultPixelHeight = 0;

void InitSystem()
{
	Snd = new TSound();
	InitSystemVars();
	ShowWindow();
}

void OnExit()
{
	delete Snd;
	Snd = nullptr;
	delete Prog;
	Prog = nullptr;

	HideWindow();
	SDL_Delay(200);
	delete Wnd.Ptr;
	Wnd.Ptr = nullptr;

	File::Delete(TempSrcCodeFile);
}

void OnReset()
{
	Reset = false;

	delete Snd;
	Snd = nullptr;
	delete Prog;
	Prog = nullptr;

	Vars.clear();
	InitSystemVars();
	CmpResult = 0;
	Event = { 0 };
	Snd->StopMainSound();
	Snd->StopSubSound();

	if (Wnd.Ptr) {
		Wnd.Pal->InitDefault();
		Wnd.Chr->InitDefault();
		Wnd.Ptr->SetTitle("");
		Wnd.Ptr->SetBackColor(0);
		Wnd.Ptr->SetPixelSize(DefaultPixelWidth, DefaultPixelWidth);
		Wnd.Ptr->Clear();
		Wnd.Ptr->Update();
	}
}

void InitSystemVars()
{
	SetSystemVar("COLS", Wnd.Ptr->GetCols());
	SetSystemVar("ROWS", Wnd.Ptr->GetRows());
	SetSystemVar("CHR_SIZE", Wnd.Ptr->GetCharset()->GetSize());
	SetSystemVar("PAL_SIZE", Wnd.Ptr->GetPalette()->GetSize());

	// Keyboard
	SetSystemVar("KEY_UP", SDL_SCANCODE_UP);
	SetSystemVar("KEY_DOWN", SDL_SCANCODE_DOWN);
	SetSystemVar("KEY_LEFT", SDL_SCANCODE_LEFT);
	SetSystemVar("KEY_RIGHT", SDL_SCANCODE_RIGHT);
	SetSystemVar("KEY_SPACE", SDL_SCANCODE_SPACE);
	SetSystemVar("KEY_RETURN", SDL_SCANCODE_RETURN);
	SetSystemVar("KEY_ESC", SDL_SCANCODE_ESCAPE);
	SetSystemVar("KEY_TAB", SDL_SCANCODE_TAB);
	SetSystemVar("KEY_BS", SDL_SCANCODE_BACKSPACE);
	SetSystemVar("KEY_INS", SDL_SCANCODE_INSERT);
	SetSystemVar("KEY_DEL", SDL_SCANCODE_DELETE);
	SetSystemVar("KEY_HOME", SDL_SCANCODE_HOME);
	SetSystemVar("KEY_END", SDL_SCANCODE_END);
	SetSystemVar("KEY_PGUP", SDL_SCANCODE_PAGEUP);
	SetSystemVar("KEY_PGDN", SDL_SCANCODE_PAGEDOWN);
	SetSystemVar("KEY_F1", SDL_SCANCODE_F1);
	SetSystemVar("KEY_F2", SDL_SCANCODE_F2);
	SetSystemVar("KEY_F3", SDL_SCANCODE_F3);
	SetSystemVar("KEY_F4", SDL_SCANCODE_F4);
	SetSystemVar("KEY_F5", SDL_SCANCODE_F5);
	SetSystemVar("KEY_F6", SDL_SCANCODE_F6);
	SetSystemVar("KEY_F7", SDL_SCANCODE_F7);
	SetSystemVar("KEY_F8", SDL_SCANCODE_F8);
	SetSystemVar("KEY_F9", SDL_SCANCODE_F9);
	SetSystemVar("KEY_F10", SDL_SCANCODE_F10);
	SetSystemVar("KEY_F11", SDL_SCANCODE_F11);
	SetSystemVar("KEY_F12", SDL_SCANCODE_F12);
	SetSystemVar("KEY_1", SDL_SCANCODE_1);
	SetSystemVar("KEY_2", SDL_SCANCODE_2);
	SetSystemVar("KEY_3", SDL_SCANCODE_3);
	SetSystemVar("KEY_4", SDL_SCANCODE_4);
	SetSystemVar("KEY_5", SDL_SCANCODE_5);
	SetSystemVar("KEY_6", SDL_SCANCODE_6);
	SetSystemVar("KEY_7", SDL_SCANCODE_7);
	SetSystemVar("KEY_8", SDL_SCANCODE_8);
	SetSystemVar("KEY_9", SDL_SCANCODE_9);
	SetSystemVar("KEY_0", SDL_SCANCODE_0);
	SetSystemVar("KEY_Q", SDL_SCANCODE_Q);
	SetSystemVar("KEY_W", SDL_SCANCODE_W);
	SetSystemVar("KEY_E", SDL_SCANCODE_E);
	SetSystemVar("KEY_R", SDL_SCANCODE_R);
	SetSystemVar("KEY_T", SDL_SCANCODE_T);
	SetSystemVar("KEY_Y", SDL_SCANCODE_Y);
	SetSystemVar("KEY_U", SDL_SCANCODE_U);
	SetSystemVar("KEY_I", SDL_SCANCODE_I);
	SetSystemVar("KEY_O", SDL_SCANCODE_O);
	SetSystemVar("KEY_P", SDL_SCANCODE_P);
	SetSystemVar("KEY_A", SDL_SCANCODE_A);
	SetSystemVar("KEY_S", SDL_SCANCODE_S);
	SetSystemVar("KEY_D", SDL_SCANCODE_D);
	SetSystemVar("KEY_F", SDL_SCANCODE_F);
	SetSystemVar("KEY_G", SDL_SCANCODE_G);
	SetSystemVar("KEY_H", SDL_SCANCODE_H);
	SetSystemVar("KEY_J", SDL_SCANCODE_J);
	SetSystemVar("KEY_K", SDL_SCANCODE_K);
	SetSystemVar("KEY_L", SDL_SCANCODE_L);
	SetSystemVar("KEY_Z", SDL_SCANCODE_Z);
	SetSystemVar("KEY_X", SDL_SCANCODE_X);
	SetSystemVar("KEY_C", SDL_SCANCODE_C);
	SetSystemVar("KEY_V", SDL_SCANCODE_V);
	SetSystemVar("KEY_B", SDL_SCANCODE_B);
	SetSystemVar("KEY_N", SDL_SCANCODE_N);
	SetSystemVar("KEY_M", SDL_SCANCODE_M);
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

void CreateWindow(int pixelWidth, int pixelHeight, int cols, int rows)
{
	if (Wnd.Ptr) {
		Abort(Error.WindowAlreadyOpen);
		return;
	}

	DefaultPixelWidth = pixelWidth;
	DefaultPixelHeight = pixelHeight;

	Wnd.Ptr = TWindow::CreateWithPixelSizeAndTileGrid(pixelHeight, pixelHeight, cols, rows);
	Wnd.Pal = Wnd.Ptr->GetPalette();
	Wnd.Chr = Wnd.Ptr->GetCharset();
	
	Wnd.Ptr->SetBackColor(0);
	Wnd.Ptr->Clear();
	Wnd.Ptr->Update();
}

void ShowWindow()
{
	Wnd.Ptr->Show();
}

void HideWindow()
{
	Wnd.Ptr->Hide();
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
	if (Wnd.UpdateRequest > 0) {
		Wnd.Ptr->Update();
		Wnd.UpdateRequest = 0;
	}
}

void ProcessGlobalEventsInMainThread()
{
	SDL_PollEvent(&Event);
	
	Keyboard = SDL_GetKeyboardState(nullptr);

	if (Event.type == SDL_QUIT) {
		NewProgram = "";
		Exit = true;
	}
	else if (Event.type == SDL_KEYDOWN) {
		SDL_Keycode key = Event.key.keysym.sym;
		if (TKey::Alt()) {
			if (key == SDLK_RETURN && Wnd.Ptr) {
				Wnd.Ptr->ToggleFullscreen();
				Wnd.Ptr->Update();
			}
			else if (TKey::Ctrl() && key == SDLK_r) {
				Reset = true;
				SDL_Delay(100);
			}
			else if (TKey::Ctrl() && key == SDLK_x) {
				NewProgram = "";
				Exit = true;
			}
		}
	}
}

/*#############################################################################
 
						  - COMMAND IMPLEMENTATION -

				 WARNING! All commands run in the MACHINE thread

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
void SRC()
{
	Argc(1);
	auto file = BaseDir + ArgString();
	if (!File::Exists(file)) {
		Abort(String::Format(Error.ProgramFileNotFound, file.c_str()));
		return;
	}
	TempSrcCode += File::ReadText(file) + "\n";
}
void COMPILE()
{
	Argc(0);
	auto filename = Util::RandomHex(8);
	TempSrcCodeFile = BaseDir + filename;
	File::WriteText(TempSrcCodeFile, TempSrcCode);

	TempSrcCode = "";
	NewProgram = filename;
	Exit = true;
}
void EXIT()
{
	Argc(0);
	Exit = true;
}
void HALT()
{
	Argc(0);
	while (!Exit && !Reset) {
		Wnd.UpdateRequest = 1;
	}
}
void RUN()
{
	Argc(1);
	NewProgram = ArgString();
	Exit = true;
}
void RESET()
{
	Argc(0);
	Reset = true;
}
void CONST()
{
	Argc(2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Const = true;

	if (Arg(1)->Type == ParameterType::NumberLiteral) {
		auto value = Arg(1)->NumberValue;
		var.Type = VariableType::Number;
		var.Number = value;
	}
	else if (Arg(1)->Type == ParameterType::StringLiteral) {
		auto value = Arg(1)->StringValue;
		var.Type = VariableType::String;
		var.String = value;
	}
	else if (Arg(1)->Type == ParameterType::CharLiteral) {
		auto value = Arg(1)->NumberValue;
		var.Type = VariableType::Number;
		var.Number = value;
	}
	else {
		Abort(Error.IllegalConstType);
		return;
	}

	Vars[id] = var;
}
void VAR()
{
	Argc(1, 2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::Number;

	if (Argc() == 1)
		var.Number = 0;
	else if (Argc() == 2)
		var.Number = ArgNumber();

	Vars[id] = var;
}
void VAR$()
{
	Argc(1, 2);
	auto id = ArgVariableName(false);
	Variable var;
	var.Type = VariableType::String;
	
	if (Argc() == 1)
		var.String = "";
	else if (Argc() == 2)
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
		AssertVariableNotConst(id);
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
void GRID()
{
	Argc(1);
	Wnd.Grid = ArgNumber();
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

	Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y, transparent, Wnd.Grid);
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
			Wnd.Ptr->DrawTile(tile, fgc, bgc, x, y, transparent, true);
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
	
	for (int y = 0; y < Wnd.Ptr->GetRows(); y++) {
		for (int x = 0; x < Wnd.Ptr->GetCols(); x++) {
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
void PIXR()
{
	Argc(2);
	int w = ArgNumber() + 1;
	int h = ArgNumber() + 1;

	Wnd.Ptr->SetPixelSize(w, h);

	SetSystemVar("COLS", Wnd.Ptr->GetCols());
	SetSystemVar("ROWS", Wnd.Ptr->GetRows());
}
void DRAW()
{
	Argc(0);
	Wnd.UpdateRequest = 1;
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
void CHK()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumber(id);
	SDL_Keycode scancode = ArgNumber();
	Vars[id].Number = Keyboard[scancode] ? 1 : 0;
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
void LOAD()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeString(id);
	auto path = BaseDir + ArgString();
	AssertFileExists(path);
	auto data = File::ReadText(path);
	Vars[id].String = data;
}
void BLOAD()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumberArray(id);
	auto path = BaseDir + ArgString();
	AssertFileExists(path);
	auto data = File::ReadBytes(path);
	Vars[id].NumberArray.clear();

	for (auto value : data)
		Vars[id].NumberArray.push_back((int)value);
}
void SAVE()
{
	Argc(2);
	auto data = ArgString();
	auto filePath = BaseDir + ArgString();
	File::WriteText(filePath, data);
}
void BSAVE()
{
	Argc(2);
	auto id = ArgVariableName(true);
	AssertVariableIsTypeNumberArray(id);
	auto path = BaseDir + ArgString();
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

	//=== COMPILER ===
	Op["SRC"] = &SRC;			// Add PTML source code file
	Op["COMPILE"] = &COMPILE;	// Compile source files

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
	Op["CONST"] = &CONST;		// Declare a constant
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
	Op["PIXR"] = &PIXR;			// Set pixel resolution
	Op["DRAW"] = &DRAW;			// Update the screen
	Op["CLS"] = &CLS;			// Clear screen and set background color
	Op["GRID"] = &GRID;			// Enable/disable snapping tiles to a grid
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
	Op["CHK"] = &CHK;			// Check if key is pressed

	//=== SOUND ===
	Op["SNDOFF"] = &SNDOFF;		// Stop all currently playing sounds
	Op["WAVE"] = &WAVE;			// Set waveform type
	Op["VOL"] = &VOL;			// Set sound volume
	Op["BEEP"] = &BEEP;			// Play a single beep
	Op["PLAY"] = &PLAY;			// Play notes from a sound string (once)
	Op["LPLAY"] = &LPLAY;		// Play notes from a sound string (loop)

	//=== FILESYSTEM ===
	Op["LOAD"] = &LOAD;			// Read file into string
	Op["BLOAD"] = &BLOAD;		// Read file bytes into number array
	Op["SAVE"] = &SAVE;			// Write string to file
	Op["BSAVE"] = &BSAVE;		// Write bytes from number array to file

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
