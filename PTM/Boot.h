#pragma once
#include <string>
#include <CppUtils.h>
#include "GlobalDefs.h"
using namespace CppUtils;

struct BootFile {
	std::string Autorun = "";
};
extern BootFile Boot;

bool LoadBootFile();
