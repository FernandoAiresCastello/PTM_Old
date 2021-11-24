#pragma once
#include <map>
#include <vector>
#include <string>
#include "ProgramLine.h"

class Program
{
public:
	std::vector<ProgramLine> Lines;
	std::map<std::string, int> Labels;
	std::vector<std::string> Errors;

	void Load(std::string path);

private:
	ProgramLine Parse(int srcLineNr, std::string& srcline);
	bool ParseParams(std::string& args, std::vector<Parameter>& params);
	std::vector<std::string> SplitArgs(std::string& args);
};
