#pragma once
#include <map>
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

extern std::string Title;
extern int* Memory;
extern int MemSize;
extern std::map<std::string, int> Ptr;
extern TWindow* Wnd;

void InitCommands();

void NOP();
void EXIT();
void ABORT();
void ALLOC();
void CSTR();
void SET();
void PTR();
void MSGBOX();
void TITLE();
void HALT();
void WINDOW();
void REFR();
void OUT();
void ADD();
void CMP();
void JMP();
void JE();
void JNE();
void JG();
void JGE();
void JL();
void JLE();
