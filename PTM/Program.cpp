#include <CppUtils.h>
#include "Program.h"
#include "Interpreter.h"
using namespace CppUtils;

void Program::Load(std::string& path)
{
	auto file = File::ReadLines(path);
	
	Lines.clear();
	Labels.clear();
	Errors.clear();

	for (int srcLineNr = 0; srcLineNr < file.size(); srcLineNr++) {
		std::string srcLine = String::Trim(file[srcLineNr]);

		bool quote = false;
		for (int i = 0; i < srcLine.length(); i++) {
			char ch = srcLine[i];
			if (ch == '"') {
				quote = !quote;
			}
			else if (ch == ';' && !quote) {
				srcLine = String::Trim(srcLine.substr(0, i));
				break;
			}
		}

		if (srcLine != "") {
			if (String::EndsWith(srcLine, ':')) {
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

	int ixFirstSpace = srcLine.find_first_of(" \t");
	if (ixFirstSpace > 0) {
		line.Cmd.Operation = String::Trim(srcLine.substr(0, ixFirstSpace));
		std::string args = String::Trim(srcLine.substr(ixFirstSpace));
		bool paramsOk = ParseParams(args, line.Cmd.Params);
		if (!paramsOk) {
			err = Error.SyntaxError;
		}
	}
	else {
		line.Cmd.Operation = srcLine;
	}

	if (!IsValidOpcode(line.Cmd.Operation))
		err = Error.UnknownCommand;
	if (err != "")
		Errors.push_back(String::Format("%s at line %i:\n\n%s", err.c_str(), srcLineNr, srcLine.c_str()));

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
		if (String::StartsAndEndsWith(arg, '\'')) {
			param.Type = ParameterType::CharLiteral;
			arg = String::RemoveFirstAndLast(arg);
		}
		else if (String::StartsWith(arg, '$')) {
			param.Type = ParameterType::StringPointer;
			arg = String::RemoveFirst(arg);
		}
		else if (String::StartsWith(arg, '&')) {
			param.Type = ParameterType::Address;
			arg = String::RemoveFirst(arg);
		}
		else if (String::StartsWithNumber(arg) || arg[0] == '-' || arg[0] == '+') {
			param.Type = ParameterType::NumberLiteral;
		}
		else if (isalpha(arg[0])) {
			param.Type = ParameterType::Identifier;
		}
		else {
			param.Type = ParameterType::Invalid;
			ok = false;
		}

		if (param.Type == ParameterType::CharLiteral) {
			param.StringValue = String::ToString(arg[0]);
			param.NumberValue = arg[0];
		}
		else {
			param.StringValue = arg;
			param.NumberValue = String::ToInt(arg);
		}
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
		else if ((ch == ' ' || ch == '\t') && !quote) {
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
