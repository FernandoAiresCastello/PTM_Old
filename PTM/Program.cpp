#include <CppUtils.h>
#include "Program.h"
using namespace CppUtils;

void Program::Load(std::string path)
{
	auto file = File::ReadLines(path);
	
	Lines.clear();
	Labels.clear();
	Errors.clear();

	for (int srcLineNr = 0; srcLineNr < file.size(); srcLineNr++) {
		std::string srcLine = String::Trim(file[srcLineNr]);
		if (srcLine != "") {
			if (String::StartsWith(srcLine, ';')) {
				continue;
			}
			else if (String::EndsWith(srcLine, ':')) {
				std::string label = String::GetFirstChars(srcLine, srcLine.length() - 1);
				Labels[label] = Lines.size();
			}
			else {
				Lines.push_back(Parse(srcLineNr + 1, srcLine));
			}
		}
	}
}

ProgramLine Program::Parse(int srcLineNr, std::string& srcLine)
{
	ProgramLine line;
	line.SrcLineNr = srcLineNr;
	line.SrcCode = srcLine;
	std::string err = "";

	int ixFirstSpace = srcLine.find(' ');
	if (ixFirstSpace > 0) {
		line.Cmd.Name = String::Trim(srcLine.substr(0, ixFirstSpace));
		std::string args = String::Trim(srcLine.substr(ixFirstSpace));
		bool paramsOk = ParseParams(args, line.Cmd.Params);
		if (!paramsOk) {
			err = "Invalid parameter";
		}
	}
	else {
		line.Cmd.Name = srcLine;
	}

	if (err != "")
		Errors.push_back(String::Format("%s at line %i: %s", err.c_str(), srcLineNr, srcLine.c_str()));

	return line;
}

bool Program::ParseParams(std::string& args, std::vector<Parameter>& params)
{
	bool ok = true;

	for (auto& arg : SplitArgs(args)) {
		Parameter param;

		if (String::StartsAndEndsWith(arg, '"')) {
			param.Type = ParameterType::StringLiteral;
			arg = String::RemoveFirstAndLast(arg);
		}
		else if (String::IsEnclosedBy(arg, '[', ']')) {
			param.Type = ParameterType::PointerAccess;
			arg = String::RemoveFirstAndLast(arg);
		}
		else if (String::StartsWithNumber(arg)) {
			param.Type = ParameterType::NumberLiteral;
		}
		else if (isalpha(arg[0])) {
			param.Type = ParameterType::Identifier;
		}
		else {
			param.Type = ParameterType::Invalid;
			ok = false;
		}

		param.StringValue = arg;
		param.NumberValue = String::ToInt(arg);
		params.push_back(param);
	}

	return ok;
}

std::vector<std::string> Program::SplitArgs(std::string& args)
{
	std::vector<std::string> arglist;
	std::string arg = "";
	bool quote = false;

	for (int i = 0; i < args.length(); i++) {
		char ch = args[i];
		if (ch == '"') {
			quote = !quote;
			arg.push_back('"');
		}
		else if (ch == ' ' && !quote) {
			arg = String::Trim(arg);
			if (arg != "") {
				arglist.push_back(arg);
				arg = "";
			}
		}
		else {
			arg.push_back(ch);
		}

		if (i == args.length() - 1) {
			arglist.push_back(arg);
		}
	}

	return arglist;
}
