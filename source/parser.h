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
	u64 size;
	Ast_Block* tree;
};

struct SourceCode {
	StringView code;
	u64 size;
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
	Parser() = delete;
	
	Parser(StringView loggerName, StringView code, bool isFile)
		:logger(loggerName), lexer(code, isFile)
	{
	}
	
	/* 
		Parser(SourceUnit unit)
		{
			logger(unit.file);
			lexer(unit.code, unit.isFile)
		}
		 */
	
	Logger logger;
	LexerState lexer;
	Arena ast_arena = { 0 };
	
	static Predefined_Type*  predefined_types;
	static void register_predefined_types();
	
	
	Ast_Scope* scopes = {0};
	Ast_Scope* current_scope = nullptr;
	
	inline Ast_Scope* enter_scope();
	inline void exit_scope();
	//inline Scope* get_current_scope();
	
	//void expect_and_eat(u64 type);
	void free() { arena_free(&ast_arena);}
	
	Ast parse();
	Ast_Node* parse_statement();
	Ast_Node* parse_const_def();
	Ast_Type* parse_type();
	Ast_Block* parse_block();
	
	
	Ast_Node* parse_directive();
	Ast_Struct_Declaration* parse_struct_def();
	Ast_Proc_Declaration* parse_proc_def();
	Ast_Node* parse_statement_expression();
	Ast_Block* parse_block_of_statements();
	Ast_Var_Declaration* parse_var_def();
	Ast_Var_Declaration* parse_argument_def();
	Ast_Expression* parse_expression(s8 priority = -1);
	Ast_Expression* parse_suffix_expression(Ast_Expression* prev);
	Ast_Expression* parse_unary_expression();
	Ast_Expression* parse_primary_expression();
};
