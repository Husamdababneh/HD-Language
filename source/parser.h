/* ========================================================================
$File: parser.h
$Date: 2020-10-16
$Creator: Husam Dababneh
$Description: parser
========================================================================*/
#pragma once

#include "Ast.h"
#include "lex.h"
#include "typer.h"

struct SourceFile {
	StringView name;
	U64 size;
	Ast_Block* tree;
};

struct SourceCode {
	StringView code;
	U64 size;
	Ast_Block* tree;
};

struct SourceUnit {
	bool isFile;
	
	union {
		SourceFile file;
		SourceCode code;
	};
};


struct Parser {
	static Predefined_Type*  predefined_types;
	
	// TODO(Husam): Make a list of global Loggers and then call the one you need when you want ?? 
	// i think we only need 1 logger ? 
	Logger logger;
	LexerState lexer;
	Arena ast_arena;
	Ast_Scope* scopes;
	Ast_Scope* current_scope;
	
};
