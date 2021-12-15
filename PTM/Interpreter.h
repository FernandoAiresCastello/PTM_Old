#pragma once
#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

extern bool Exit;
extern std::map<std::string, void(*)()> Op;
extern ErrorMessages Error;
extern int KeyPressed;

void InitMachine(Program* prog);
void DestroyMachine();
void RunMachine();
int RunMachineThread(void* dummy);
void UpdateWindow();
bool IsValidOpcode(std::string& opcode);
void Abort(std::string msg, bool printInfo = true);
void ProcessGlobalEvents();
void Jump(int ixProgLine);
void Call(int ixProgLine);
void Return();
void Argc(int expectedArgCount);
Parameter* Arg();
std::string ArgStringLiteral();
int ArgNumberLiteral();
int ArgNumber();
std::string ArgIdentifier(bool assertExists);
std::string ArgString();
int Peek(std::string& identifier);
int GetAddress(std::string& identifier);
void AssertIdentifier(std::string& identifier);
int ArgLabel();
