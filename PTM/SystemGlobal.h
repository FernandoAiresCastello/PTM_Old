#pragma once
#include <map>
#include <string>
#include <vector>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Parameter.h"
#include "System.h"
#include "Machine.h"

using namespace CppUtils;
using namespace TileGameLib;

#define OP(x)	Op[#x] = &x
#define REQ(x)	if (!RequireArgCount(x)) return;
#define ARGC	Args->size()
