#include <stack>
#include <CppUtils.h>
#include <TileGameLib.h>
#include "Interpreter.h"
#include "System.h"
#include "ErrorMessages.h"
#include "Variable.h"
using namespace CppUtils;
using namespace TileGameLib;

Program* Prog = nullptr;
ProgramLine* CurLine = nullptr;
int IxCurLine = 0;
bool Exit = false;
std::map<std::string, void(*)()> Op;
std::vector<Parameter>* Args = nullptr;
int IxArg = 0;
bool Branch = false;
std::stack<int> CallStack;
std::string NewProgram = "";

void InitInterpreter(Program* prog)
{
	if (prog != Prog)
		delete Prog;
	
	Prog = prog;
	ResetInterpreter();
}

void ResetInterpreter()
{
	Util::Randomize();

	Exit = false;
	Branch = false;
	IxCurLine = 0;
	CurLine = &Prog->Lines[IxCurLine];
	IxArg = 0;
	Args = &CurLine->Cmd.Params;

	while (!CallStack.empty())
		CallStack.pop();

	ResetSystem();
}

void DestroyInterpreter()
{
	DestroySystem();
	delete Prog;
	Prog = nullptr;
}

void RunInterpreter()
{
	InitSystem();

	SDL_Thread* thread = nullptr;

	while (!Exit) {
		ProcessGlobalEvents();
		UpdateWindow();

		if (!thread)
			thread = SDL_CreateThread(RunInterpreterThread, "RunMachineThread", nullptr);
	}
}

int RunInterpreterThread(void* dummy)
{
	while (!Exit) {
		CurLine = &Prog->Lines[IxCurLine];
		Args = &CurLine->Cmd.Params;
		IxArg = 0;

		if (Op.find(CurLine->Cmd.Operation) != Op.end())
			Op[CurLine->Cmd.Operation]();
		else
			Abort(Error.UnknownCommand);

		if (!Exit) {
			if (Branch)
				Branch = false;
			else
				IxCurLine++;

			if (IxCurLine >= Prog->Lines.size())
				Abort(Error.ProgEndWithoutExit);
		}
	}

	return 0;
}

bool IsValidOpcode(std::string& opcode)
{
	return Op.find(opcode) != Op.end();
}

void Abort(std::string msg, bool printInfo)
{
	if (printInfo) {
		std::string fmt = String::Format("%s at line %i:\n\n%s",
			msg.c_str(), CurLine->SrcLineNr, CurLine->SrcCode.c_str());

		MsgBox::Error(Wnd.Title, fmt);
	}
	else {
		MsgBox::Error(Wnd.Title, msg);
	}

	Exit = true;
	exit(1);
}

void Jump(int ixProgLine)
{
	IxCurLine = ixProgLine;
	Branch = true;
}

void Call(int ixProgLine)
{
	CallStack.push(IxCurLine + 1);
	IxCurLine = ixProgLine;
	Branch = true;
}

void Return()
{
	if (CallStack.empty()) {
		Abort(Error.CallStackEmpty);
		return;
	}
	IxCurLine = CallStack.top();
	CallStack.pop();
	Branch = true;
}

void Argc(int argc)
{
	if (Args->size() != argc) {
		Abort(String::Format(Error.InvalidArgCount, argc, Args->size()));
		return;
	}
}

int Argc()
{
	return Args->size();
}

void Argc(int minArgc, int maxArgc)
{
	if (Args->size() < minArgc || Args->size() > maxArgc) {
		Abort(String::Format(Error.InvalidArgCountMinMax, minArgc, maxArgc, Args->size()));
		return;
	}
}

Parameter* Arg()
{
	if (IxArg >= 0 && IxArg < Args->size()) {
		Parameter* arg = &Args->at(IxArg);
		IxArg++;
		return arg;
	}

	Abort(Error.ArgIndexOutOfRange);
	return nullptr;
}

Parameter* Arg(int index)
{
	if (index >= 0 && index < Args->size())
		return &Args->at(index);

	Abort(Error.ArgIndexOutOfRange);
	return nullptr;
}

std::string ArgStringLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::StringLiteral) {
		Abort(Error.StringLiteralExpected);
		return "";
	}
	return arg->StringValue;
}

int ArgNumberLiteral()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::NumberLiteral) {
		Abort(Error.NumberLiteralExpected);
		return 0;
	}
	return arg->NumberValue;
}

std::string ArgVariableName(bool assertExists)
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::Identifier) {
		Abort(Error.IdentifierExpected);
		return "";
	}
	if (String::StartsWith(arg->StringValue, SYS_VAR_PREFIX)) {
		Abort(Error.IllegalVariableName);
		return "";
	}

	AssertVariable(arg->StringValue, assertExists);

	return arg->StringValue;
}

int ArgNumber()
{
	Parameter* arg = Arg();
	
	if (arg->Type == ParameterType::NumberLiteral)
		return arg->NumberValue;
	if (arg->Type == ParameterType::StringLiteral)
		return arg->NumberValue;
	if (arg->Type == ParameterType::CharLiteral)
		return arg->NumberValue;
	else if (arg->Type == ParameterType::Identifier)
		return GetNumberFromVariable(arg->StringValue);
	else if (arg->Type == ParameterType::ArrayIndexLiteral)
		return GetNumberFromArrayAtIndex(arg->StringValue, arg->ArrayIndex);
	else if (arg->Type == ParameterType::ArrayIndexVariable)
		return GetNumberFromArrayAtVarIndex(arg->StringValue, arg->VariableArrayIndex);
	else
		Abort(Error.TypeMismatch);
	
	return 0;
}

std::string ArgString()
{
	Parameter* arg = Arg();

	if (arg->Type == ParameterType::StringLiteral)
		return arg->StringValue;
	else if (arg->Type == ParameterType::NumberLiteral)
		return arg->StringValue;
	else if (arg->Type == ParameterType::CharLiteral)
		return arg->StringValue;
	else if (arg->Type == ParameterType::Identifier)
		return GetStringFromVariable(arg->StringValue);
	else if (arg->Type == ParameterType::ArrayIndexLiteral)
		return GetStringFromArrayAtIndex(arg->StringValue, arg->ArrayIndex);
	else if (arg->Type == ParameterType::ArrayIndexVariable)
		return GetStringFromArrayAtVarIndex(arg->StringValue, arg->VariableArrayIndex);
	else
		Abort(Error.TypeMismatch);
	
	return "";
}

int ArgLabel()
{
	Parameter* arg = Arg();
	if (arg->Type != ParameterType::Identifier) {
		Abort(Error.LabelExpected);
		return -1;
	}
	std::string& label = arg->StringValue;
	if (Prog->Labels.find(label) == Prog->Labels.end()) {
		Abort(String::Format(Error.LabelNotFound, label.c_str()));
		return -1;
	}
	return Prog->Labels[label];
}

int GetNumberFromVariable(std::string& identifier)
{
	if (Vars.find(identifier) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, identifier.c_str()));
		return 0;
	}

	if (Vars[identifier].Type == VariableType::Number) {
		return Vars[identifier].Number;
	}
	else if (Vars[identifier].Type == VariableType::String) {
		return String::ToInt(Vars[identifier].String);
	}

	Abort(Error.TypeMismatch);
	return 0;
}

int GetNumberFromArrayAtIndex(std::string& identifier, int index)
{
	if (Vars.find(identifier) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, identifier.c_str()));
		return 0;
	}

	if (Vars[identifier].Type == VariableType::StringArray) {
		size_t length = Vars[identifier].StringArray.size();
		if (index < Vars[identifier].StringArray.size())
			return String::ToInt(Vars[identifier].StringArray[index]);

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return 0;
	}
	if (Vars[identifier].Type == VariableType::NumberArray) {
		size_t length = Vars[identifier].NumberArray.size();
		if (index < Vars[identifier].NumberArray.size())
			return Vars[identifier].NumberArray[index];

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return 0;
	}

	Abort(Error.TypeMismatch);
	return 0;
}

int GetNumberFromArrayAtVarIndex(std::string& idVariable, std::string& idIndex)
{
	if (Vars.find(idVariable) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, idVariable.c_str()));
		return 0;
	}
	if (Vars.find(idIndex) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, idIndex.c_str()));
		return 0;
	}
	if (Vars[idIndex].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return 0;
	}

	int index = Vars[idIndex].Number;

	if (Vars[idVariable].Type == VariableType::StringArray) {
		size_t length = Vars[idVariable].StringArray.size();
		if (index < length)
			return String::ToInt(Vars[idVariable].StringArray[index]);

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return 0;
	}
	if (Vars[idVariable].Type == VariableType::NumberArray) {
		size_t length = Vars[idVariable].NumberArray.size();
		if (index < length)
			return Vars[idVariable].NumberArray[index];

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return 0;
	}

	Abort(Error.TypeMismatch);
	return 0;
}

std::string GetStringFromVariable(std::string& identifier)
{
	if (Vars.find(identifier) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, identifier.c_str()));
		return "";
	}

	if (Vars[identifier].Type == VariableType::String) {
		return Vars[identifier].String;
	}
	else if (Vars[identifier].Type == VariableType::Number) {
		return String::ToString(Vars[identifier].Number);
	}
	else if (Vars[identifier].Type == VariableType::NumberArray) {
		std::string str = "[";
		int i = 0;
		auto arr = Vars[identifier].NumberArray;
		for (auto& num : arr) {
			str.append(String::ToString(num));
			i++;
			if (i < arr.size())
				str.append(", ");
		}
		str.append("]");
		return str;
	}
	else if (Vars[identifier].Type == VariableType::StringArray) {
		std::string str = "[";
		int i = 0;
		auto arr = Vars[identifier].StringArray;
		for (auto& item : arr) {
			str.append("\"");
			str.append(item);
			str.append("\"");
			i++;
			if (i < arr.size())
				str.append(", ");
		}
		str.append("]");
		return str;
	}

	Abort(Error.TypeMismatch);
	return "";
}

std::string GetStringFromArrayAtIndex(std::string& identifier, int index)
{
	if (Vars.find(identifier) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, identifier.c_str()));
		return "";
	}

	if (Vars[identifier].Type == VariableType::StringArray) {
		size_t length = Vars[identifier].StringArray.size();
		if (index < length)
			return Vars[identifier].StringArray[index];
		
		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return "";
	}
	if (Vars[identifier].Type == VariableType::NumberArray) {
		size_t length = Vars[identifier].NumberArray.size();
		if (index < length)
			return String::ToString(Vars[identifier].NumberArray[index]);

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return "";
	}

	Abort(Error.TypeMismatch);
	return "";
}

std::string GetStringFromArrayAtVarIndex(std::string& idVariable, std::string& idIndex)
{
	if (Vars.find(idVariable) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, idVariable.c_str()));
		return "";
	}
	if (Vars.find(idIndex) == Vars.end()) {
		Abort(String::Format(Error.VariableNotDeclared, idIndex.c_str()));
		return "";
	}
	if (Vars[idIndex].Type != VariableType::Number) {
		Abort(Error.TypeMismatch);
		return "";
	}

	int index = Vars[idIndex].Number;

	if (Vars[idVariable].Type == VariableType::StringArray) {
		size_t length = Vars[idVariable].StringArray.size();
		if (index < length)
			return Vars[idVariable].StringArray[index];

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return "";
	}
	if (Vars[idVariable].Type == VariableType::NumberArray) {
		size_t length = Vars[idVariable].NumberArray.size();
		if (index < length)
			return String::ToString(Vars[idVariable].NumberArray[index]);

		Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length));
		return "";
	}

	Abort(Error.TypeMismatch);
	return "";
}

void AssertVariable(std::string& identifier, bool exists)
{
	if (exists) {
		if (Vars.find(identifier) == Vars.end())
			Abort(String::Format(Error.VariableNotDeclared, identifier.c_str()));
	}
	else {
		if (Vars.find(identifier) != Vars.end())
			Abort(String::Format(Error.DuplicateVariableName, identifier.c_str()));
	}
}

void AssertVariableIsTypeNumber(std::string& identifier)
{
	if (Vars[identifier].Type != VariableType::Number)
		Abort(Error.TypeMismatch);
}

void AssertVariableIsTypeString(std::string& identifier)
{
	if (Vars[identifier].Type != VariableType::String)
		Abort(Error.TypeMismatch);
}

void AssertVariableIsTypeNumberArray(std::string& identifier)
{
	if (Vars[identifier].Type != VariableType::NumberArray)
		Abort(Error.TypeMismatch);
}

void AssertVariableIsTypeStringArray(std::string& identifier)
{
	if (Vars[identifier].Type != VariableType::StringArray)
		Abort(Error.TypeMismatch);
}

void AssertArrayIndex(std::string& identifier, int index)
{
	if (Vars[identifier].Type == VariableType::StringArray) {
		size_t length = Vars[identifier].StringArray.size();
		if (index >= Vars[identifier].StringArray.size())
			Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length - 1));
	}
	else if (Vars[identifier].Type == VariableType::NumberArray) {
		size_t length = Vars[identifier].NumberArray.size();
		if (index >= Vars[identifier].NumberArray.size())
			Abort(String::Format(Error.ArrayIndexOutOfBounds, index, length - 1));
	}
	else {
		Abort(Error.TypeMismatch);
	}
}

void AssertFileExists(std::string& path)
{
	if (!File::Exists(path))
		Abort(String::Format(Error.FileNotFound, path.c_str()));
}

void AssertTileIndex(int index)
{
	if (index < 0 || index >= Wnd.Ptr->GetCharset()->GetSize())
		Abort(String::Format(Error.InvalidTileIndex, index));
}

void AssertPaletteIndex(int index)
{
	if (index < 0 || index >= Wnd.Ptr->GetPalette()->GetSize())
		Abort(String::Format(Error.InvalidPaletteIndex, index));
}
