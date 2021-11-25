#pragma once
#include <map>
#include <vector>
#include <string>

extern std::string Title;
extern int* Memory;
extern int MemSize;
extern std::map<std::string, int> VarPtr;

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
