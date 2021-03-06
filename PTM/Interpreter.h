#pragma once
#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
#include "ErrorMessages.h"
using namespace CppUtils;
using namespace TileGameLib;

extern bool Exit;
extern bool Reset;
extern std::map<std::string, void(*)()> Op;
extern ErrorMessages Error;
extern Program* Prog;
extern std::string NewProgram;

void RunMainThread();
int RunMachineThread(void* dummy);

void InitInterpreter(Program* prog);
bool IsValidOpcode(std::string& opcode);
void Abort(std::string msg, bool printInfo = true);
void Jump(int ixProgLine);
void Call(int ixProgLine);
void Return();
int Argc();
void Argc(int argc);
void Argc(int minArgc, int maxArgc);
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
void AssertVariableNotConst(std::string& identifier);
void AssertVariableIsTypeNumber(std::string& identifier);
void AssertVariableIsTypeString(std::string& identifier);
void AssertVariableIsTypeNumberArray(std::string& identifier);
void AssertVariableIsTypeStringArray(std::string& identifier);
void AssertArrayIndex(std::string& identifier, int index);
void AssertFileExists(std::string& path);
void AssertTileIndex(int index);
void AssertPaletteIndex(int index);
