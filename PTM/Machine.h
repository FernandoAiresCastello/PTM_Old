#pragma once
#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
using namespace CppUtils;
using namespace TileGameLib;

extern bool Exit;
extern std::string Error;
extern std::map<std::string, void(*)()> Op;
extern std::vector<Parameter>* Args;
extern int IxArg;

bool IsValidOpcode(std::string& opcode);
void InitMachine(Program* prog);
void DestroyMachine();
void RunMachine();
void Abort(std::string msg);
Parameter* Arg();
bool RequireArgCount(int count);
