#pragma once
#include <string>
#include "ParameterType.h"

class Parameter
{
public:
	ParameterType Type;
	std::string StringValue;
	int NumberValue;
};
