#pragma once
#include <string>
#include <vector>
#include "VariableType.h"

class Variable
{
public:
	VariableType Type = VariableType::Invalid;
	bool Const = false;
	int Number = 0;
	std::string String = "";
	std::vector<int> NumberArray;
	std::vector<std::string> StringArray;
};
