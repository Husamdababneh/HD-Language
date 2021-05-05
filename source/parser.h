/* ========================================================================
   $File: parser.h
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser
   ========================================================================*/
#pragma once

#include "Ast.h"


struct Parser {
	Parser() = delete;
	
	Parser(StringView loggerName, StringView code, bool isFile)
		:logger(loggerName), lexer(code, isFile)
	{
	}
	
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
	Ast_Node* parse_factor();
	Ast_Node* parse_proc_def();
	Ast_Node* parse_operator();
	Ast_Node* parse_statement();
	Ast_Node* parse_def();
	Ast_Type* parse_type();
	Ast_Block* parse_block();
	
	Ast_Node* parse_statement_expression();
	Ast_Block* parse_block_of_statements();
	Ast_Var_Declaration* parse_argument_def();
	Ast_Expression* parse_expression(s8 priority = -1);
	Ast_Expression* parse_suffix_expression(Ast_Expression* prev);
	Ast_Expression* parse_unary_expression();
	Ast_Primary* parse_primary_expression();
};
