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
bool IsValidOpcode(std::string& opcode);
void Abort(std::string msg, bool printInfo = true);
void ProcessGlobalEvents();
void Jump(int ixProgLine);
void Call(int ixProgLine);
void Return();
void Argc(int expectedArgCount);
Parameter* Arg();
Parameter* Arg(int index);
std::string ArgStringLiteral();
int ArgNumberLiteral();
int ArgNumber();
std::string ArgVariableName(bool assertExists);
std::string ArgString();
int ArgLabel();
int GetNumberFromVariable(std::string& identifier);
int GetNumberFromArrayAtIndex(std::string& identifier, int index);
int GetNumberFromArrayAtVarIndex(std::string& idVariable, std::string& idIndex);
std::string GetStringFromVariable(std::string& identifier);
std::string GetStringFromArrayAtIndex(std::string& identifier, int index);
std::string GetStringFromArrayAtVarIndex(std::string& idVariable, std::string& idIndex);
void AssertVariable(std::string& identifier, bool exists);
void AssertArrayIndex(std::string& identifier, int index);
void AssertFileExists(std::string& path);