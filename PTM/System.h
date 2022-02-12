#pragma once
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "GlobalDefs.h"
#include "System.h"
#include "Parameter.h"
#include "System.h"
#include "Interpreter.h"
#include "ErrorMessages.h"
#include "Variable.h"
using namespace CppUtils;
using namespace TileGameLib;

#define SYS_VAR_PREFIX "SYS."

extern std::map<std::string, Variable> Vars;
extern const Uint8* Keyboard;

struct SystemWindow {
	std::string Title = "";
	std::string OldTitle = "";
	TWindow* Ptr = nullptr;
	bool Grid = 0;
	int FullScreenRequest = -1;
	int UpdateRequest = 0;
	TPalette* Pal = nullptr;
	TCharset* Chr = nullptr;
};
extern SystemWindow Wnd;

void InitSystem();
void InitSystemVars();
void SetSystemVar(std::string name, int value);
void SetSystemVar(std::string name, std::string value);
void ResetSystem();
void DestroySystem();
void DestroyWindow();
void CreateWindow(int pixelWidth, int pixelHeight, int cols, int rows);
void ShowWindow();
void HideWindow();
void UpdateWindow();
void ProcessGlobalEventsInMainThread();
void InitCommands();
