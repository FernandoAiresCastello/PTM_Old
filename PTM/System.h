#pragma once
#include <map>
#include <vector>
#include <string>
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

enum class OutputMode { Free, Tiled };
enum class InputMode { Continuous, Paused };

extern std::map<std::string, Variable> Vars;
extern int KeyPressed;

struct SystemWindow {
	std::string Title = "";
	std::string OldTitle = "";
	TWindow* Ptr = nullptr;
	OutputMode OutMode = OutputMode::Free;
	InputMode InMode = InputMode::Continuous;
	int FullScreenRequest = -1;
	TPalette* Pal = nullptr;
	TCharset* Chr = nullptr;
};
extern SystemWindow Wnd;

void InitSystem();
void InitSystemVars();
void AddSystemVar(std::string name, int value);
void AddSystemVar(std::string name, std::string value);
void ResetSystem();
void DestroySystem();
void DestroyWindow();
void CreateWindow(int width, int height);
void UpdateWindow();
void ShowIntro();
void InitCommands();
