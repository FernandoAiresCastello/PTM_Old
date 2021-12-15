#pragma once
#include <string>
#include "ParameterType.h"

class Parameter
{
public:
	ParameterType Type = ParameterType::Invalid;
	std::string StringValue = "";
	int NumberValue = 0;
	std::string VariableArrayIndex = "";
	int ArrayIndex = 0;
};
