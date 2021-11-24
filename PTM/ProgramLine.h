#pragma once
#include <string>
#include <vector>
#include "Command.h"

class ProgramLine
{
public:
	int SrcLineNr;
	std::string SrcCode;
	Command Cmd;
};
