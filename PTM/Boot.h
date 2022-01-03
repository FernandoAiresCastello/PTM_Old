#pragma once
#include <string>
#include <CppUtils.h>
#include "GlobalDefs.h"
using namespace CppUtils;

struct BootFile {
	std::string Autorun = "";
	bool ShowIntro = true;
	int ResWidth = 0;
	int ResHeight = 0;
	int WndWidth = 0;
	int WndHeight = 0;
};
extern BootFile Boot;

bool LoadBootFile();
