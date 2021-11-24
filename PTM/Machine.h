#pragma once
#include <SDL.h>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Program.h"
using namespace CppUtils;
using namespace TileGameLib;

void InitMachine(Program& prog);
void DestroyMachine();
void RunMachine();
