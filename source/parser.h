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

struct Parser {
	Parser() = delete;

	Parser(StringView loggerName, StringView code, bool isFile)
		:logger(loggerName), lexer(code, isFile)
	{
	}

	Logger logger;
	LexerState lexer;
	Arena ast_arena = { 0 };

	void expect_and_eat(u64 type);
	Ast parse();
	Ast_Node* parse_expression();
	Ast_Node* parse_factor();
	//Ast_Node* parse_scope();
	Ast_Node* parse_def();
	Ast_Node* parse_operator();
	Ast_Node* parse_statement();
	Ast_Node* parse_subexpression();
	Ast_Node* parse_block_of_statments();

	void free() {
		arena_free(&ast_arena);
	}
};
