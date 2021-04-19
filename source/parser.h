/* ========================================================================
   $File: parser.h
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser
   ========================================================================*/
#pragma once

#include "Ast.h"
/*
struct Type {
	String name;
	u64 size;
};
*/

struct Scope {
	u64 id;
	StringView name;
};

struct MySymbol {
	Ast_Node* node;
	Scope scope;
};

struct SymbolTableEntry{
	StringView key;
	MySymbol value;
};

struct Parser {
	Parser() = delete;
	
	Parser(StringView loggerName, StringView code, bool isFile)
		:logger(loggerName), lexer(code, isFile)
	{
	}
	
	Logger logger;
	LexerState lexer;
	Arena ast_arena = { 0 };
	Scope* scopes = nullptr;
	SymbolTableEntry* symbolTable = NULL;
	
	
	inline void register_scope(const StringView& name);
	inline Scope get_current_scope();
	inline void exit_scope();
	void expect_and_eat(u64 type);
	void free() { arena_free(&ast_arena);}
	
	Ast parse();
	Ast_Node* parse_expression();
	Ast_Node* parse_factor();
	//Ast_Node* parse_scope();
	Ast_Node* parse_def();
	Ast_Node* parse_operator();
	Ast_Node* parse_statement();
	Ast_Node* parse_subexpression();
	Ast_Node* parse_block_of_statments();
	
	
};
