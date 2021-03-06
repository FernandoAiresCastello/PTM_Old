#pragma once

typedef const char* cstr;

struct ErrorMessages {
	cstr UnexpectedError = "Unexpected error";
	cstr UnknownFileExtension = "Unknown file extension: \"%s\"";
	cstr ProgramFileNotSpecified = "Program file not specified and \"autorun.ptml\" not found";
	cstr ProgramFileNotFound = "Program file not found: \"%s\"";
	cstr ProgramFileEmpty = "Empty program file: \"%s\"";
	cstr FileNotFound = "File not found: \"%s\"";
	cstr UnknownCommand = "Unknown command";
	cstr ProgEndWithoutExit = "Program ended without EXIT";
	cstr InvalidArgCount = "Expected %i arguments, got %i";
	cstr InvalidArgCountMinMax = "Expected from %i to %i arguments, got %i";
	cstr ArgIndexOutOfRange = "Argument index out of range";
	cstr StringLiteralExpected = "String literal expected";
	cstr NumberLiteralExpected = "Number literal expected";
	cstr IdentifierExpected = "Identifier expected";
	cstr SyntaxError = "Syntax error";
	cstr MemoryAddrOutOfBounds = "Memory address out of bounds";
	cstr VariableNotDeclared = "Variable \"%s\" is not declared";
	cstr DuplicateVariableName = "Duplicate variable name: \"%s\"";
	cstr LabelExpected = "Label name expected";
	cstr LabelNotFound = "Label \"%s\" not found";
	cstr CallStackEmpty = "Call stack empty";
	cstr DuplicateLabel = "Duplicate label: \"%s\"";
	cstr WindowAlreadyOpen = "Window already open";
	cstr TypeMismatch = "Type mismatch";
	cstr ArrayIndexOutOfBounds = "Array index (%i) out of bounds (0 to %i)";
	cstr IllegalArgumentValue = "Illegal argument value: %s";
	cstr InvalidTileIndex = "Invalid tile index: %i";
	cstr InvalidPaletteIndex = "Invalid palette index: %i";
	cstr SystemFunctionNotFound = "Unrecognized system function call";
	cstr IllegalVariableName = "Illegal variable name";
	cstr InvalidGraphicsMode = "Invalid graphics mode: %i";
	cstr StringIndexOutOfRange = "String index out of range";
	cstr IllegalConstType = "Illegal type of constant";
	cstr ConstNotModifiable = "Const variable \"%s\" is not modifiable";
};

extern ErrorMessages Error;
