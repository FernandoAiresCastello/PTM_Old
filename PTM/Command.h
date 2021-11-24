#pragma once
#include <string>
#include <vector>
#include "Parameter.h"

class Command
{
public:
	std::string Name;
	std::vector<Parameter> Params;
};
