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

void InitMachine(Program* prog);
void DestroyMachine();
void RunMachine();
bool IsValidOpcode(std::string& opcode);
void Abort(std::string msg, bool printInfo = true);
void ProcessGlobalEvents();
void Argc(int expectedArgCount);
Parameter* Arg();
std::string ArgStringLiteral();
int ArgNumberLiteral();
int ArgNumber();
std::string ArgIdentifier();
std::string ArgString();
int Peek(std::string& identifier);
int GetAddress(std::string& identifier);
