#pragma once
#include <string>
#include <vector>
#include "Parameter.h"

class Command
{
public:
	std::string Operation;
	std::vector<Parameter> Params;
};
