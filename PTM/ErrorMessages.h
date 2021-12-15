#pragma once

typedef const char* cstr;

struct ErrorMessages {
	cstr UnknownCommand = "Unknown command";
	cstr ProgEndWithoutExit = "Program ended without EXIT";
	cstr InvalidArgCount = "Expected %i arguments, got %i";
	cstr ArgIndexOutOfRange = "Argument index out of range";
	cstr StringLiteralExpected = "String literal expected";
	cstr NumberLiteralExpected = "Number literal expected";
	cstr IdentifierExpected = "Identifier expected";
	cstr SyntaxError = "Syntax error";
	cstr MemoryAddrOutOfBounds = "Memory address out of bounds";
	cstr IdentifierNotFound = "Identifier %s not found";
	cstr LabelExpected = "Label name expected";
	cstr LabelNotFound = "Label %s not found";
	cstr CallStackEmpty = "Call stack empty";
	cstr DuplicateLabel = "Duplicate label: %s";
	cstr WindowAlreadyOpen = "Window already open";
};
