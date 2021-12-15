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
using namespace CppUtils;
using namespace TileGameLib;

extern std::string Title;
extern int* Memory;
extern int MemSize;
extern std::map<std::string, int> Addr;
extern TWindow* Wnd;
extern bool WindowCreationRequested;
extern int RequestedWindowWBuf;
extern int RequestedWindowHBuf;
extern int RequestedWindowWWnd;
extern int RequestedWindowHWnd;

void InitCommands();
