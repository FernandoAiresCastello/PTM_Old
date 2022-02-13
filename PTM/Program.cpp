#include <CppUtils.h>
#include "Program.h"
#include "Interpreter.h"
#include "GlobalDefs.h"
using namespace CppUtils;

bool Program::Load(std::string path)
{
	FilePath = path;

	if (!File::Exists(FilePath)) {
		MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, FilePath.c_str()));
		return false;
	}

	auto lines = File::ReadLines(FilePath);

	Lines.clear();
	Labels.clear();
	Errors.clear();

	for (int srcLineNr = 0; srcLineNr < lines.size(); srcLineNr++) {
		std::string srcLine = String::Trim(lines[srcLineNr]);
		bool quote = false;

		for (int i = 0; i < srcLine.length(); i++) {
			char ch = srcLine[i];
			if (ch == '"' || (ch == '\'' && !quote)) {
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
				if (Labels.find(label) == Labels.end())
					Labels[label] = Lines.size();
				else
					Errors.push_back(String::Format(Error.DuplicateLabel, label.c_str()));
			}
			else {
				Lines.push_back(Parse(srcLineNr + 1, srcLine));
			}
		}
	}

	return true;
}

std::string Program::GetFilePath()
{
	return FilePath;
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
	
	if (!IsValidOpcode(line.Cmd.Operation)) {
		err = Error.UnknownCommand;
	}
	if (err != "") {
		std::string fmt = String::Format("Error at line %i:\n\n%s\n\n%s",
			srcLineNr, err.c_str(), srcLine.c_str());

		Errors.push_back(fmt);
	}

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
		else if (String::StartsAndEndsWith(arg, '\'')) {
			param.Type = ParameterType::CharLiteral;
			arg = String::RemoveFirstAndLast(arg);
		}
		else if (String::IsEnclosedBy(arg, '[', ']')) {
			arg = String::RemoveFirstAndLast(arg);
			auto arrayAccess = String::Split(arg, ' ', true);

			if (String::StartsWithNumber(arrayAccess[1]) || arrayAccess[1][0] == '-' || arrayAccess[1][0] == '+') {
				param.Type = ParameterType::ArrayIndexLiteral;
				param.ArrayIndex = String::ToInt(arrayAccess[1]);
			}
			else if (isalpha(arrayAccess[1][0])) {
				param.Type = ParameterType::ArrayIndexVariable;
				param.VariableArrayIndex = arrayAccess[1];
			}

			arg = arrayAccess[0];
		}
		else if (String::StartsWithNumber(arg) || arg[0] == '-' || arg[0] == '+') {
			param.Type = ParameterType::NumberLiteral;
		}
		else if (isalpha(arg[0]) || String::StartsWith(arg, '$')) {
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
			arg.push_back(ch);
		}
		else if (ch == '[') {
			quote = true;
			arg.push_back(ch);
		}
		else if (ch == ']') {
			quote = false;
			arg.push_back(ch);
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

bool Program::Validate()
{
	if (!Errors.empty()) {
		MsgBox::Error(APP_NAME, Errors[0]);
		return false;
	}
	if (Lines.empty()) {
		MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileEmpty, FilePath.c_str()));
		return false;
	}
	return true;
}
