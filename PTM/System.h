#pragma once
#include <map>
#include <vector>
#include <string>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "System.h"
#include "Parameter.h"
#include "System.h"
#include "Interpreter.h"
#include "ErrorMessages.h"
#include "Variable.h"
using namespace CppUtils;
using namespace TileGameLib;

enum class OutputMode { Free, Tiled };

extern std::map<std::string, Variable> Vars;
extern int KeyPressed;

struct SystemWindow {
	std::string Title = "";
	std::string OldTitle = "";
	TWindow* Ptr = nullptr;
	bool CreationRequested = false;
	int BufWidth = 0;
	int BufHeight = 0;
	int WndWidth = 0;
	int WndHeight = 0;
	OutputMode OutMode = OutputMode::Free;
	bool AutoUpdate = true;
	int FullScreenRequest = -1;
	TPalette* Pal = nullptr;
	TCharset* Chr = nullptr;
};
extern SystemWindow Wnd;

void InitSystem();
void DestroySystem();
void CreateWindow();
void UpdateWindow();
void InitCommands();
