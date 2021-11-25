#pragma once
#include <map>
#include <vector>
#include <string>
#include <TileGameLib.h>

using namespace TileGameLib;

extern std::string Title;
extern int* Memory;
extern int MemSize;
extern std::map<std::string, int> VarPtr;
extern TWindow* Wnd;

void InitCommands();

void NOP();
void EXIT();
void ABORT();
void ALLOC();
void CSTR();
void SET();
void PTR();
void INC();
void DEC();
void MSGBOX();
void TITLE();
void WINDOW();
void HALT();
