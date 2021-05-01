/* ========================================================================
$File: Ast.h
$Date: 2020-10-03
$Creator: Husam Dababneh
$Description: Ast.h
========================================================================*/
#pragma once

#include "lex.h"

enum {
	AST_DEF_VAR,
	AST_DEF_PROC,
	AST_DEF_STRUCT,
	AST_DEF_ENUM,
	AST_DEF_FLAG, // do I need this ?? 
};

enum {
	AST_DECLARATION,
	AST_TYPE,
	AST_BLOCK,
	AST_IF,
	AST_WHILE,
	AST_EXPRESSION,
	AST_LITERAL,
	AST_PRIMARY,
	AST_UKNOWN,
};


enum {
	OP_UNKOWN = -1,
	OP_ADD = 0,
	OP_SUB = 0,
	OP_MUL = 1,
	OP_DIV = 1
};


enum {
	AST_KIND_UNKNOWN,
	// AST_KIND_
	
	// Expressions Kinds
	AST_KIND_EXP_BINARY,
	AST_KIND_EXP_UNARY,
	//AST_KIND_EXP_BINARY,
	
	// Primary Expression Kinds
	AST_KIND_PRIMARY_NUMBER,
	AST_KIND_PRIMARY_IDENTIFIER,
	AST_KIND_PRIMARY_STRING,
	
	// Declaration Kinds
	AST_KIND_DECL_PROCEDURE,
	AST_KIND_DECL_VARIABLE,
	AST_KIND_DECL_STRUCT,
	
};

struct Ast_Node;
struct Ast_Scope;
struct Ast {
	Ast_Node** nodes;
};


struct Ast_Node {
	u16 type = AST_UKNOWN; // ?? 
	u16 kind = AST_KIND_UNKNOWN;
	Token token;
	
};

struct Ast_Block : Ast_Node {
	Ast_Block() {
		type = AST_BLOCK;
	}
	Ast_Node** nodes;
	Ast_Scope* scope;
};


struct Ast_Literal : Ast_Node
{
	Ast_Literal(){
		type = AST_LITERAL;
	}
};


struct Ast_Primary : Ast_Node
{
	Ast_Primary (){
		type = AST_PRIMARY;
	}
	
};


struct Ast_Expression : Ast_Node {
	Ast_Expression() {
		type = AST_EXPRESSION; 
	}
	u32       op;
};

struct Ast_Unary : Ast_Expression {
	//  <a> ? <b>
	Ast_Unary() {
		kind = AST_KIND_EXP_UNARY;
		op = OP_UNKOWN;
		child = nullptr;
	}
	Ast_Node* child;
	
};


struct Ast_Binary : Ast_Expression {
	//  <a> ? <b>
	Ast_Binary() {
		kind = AST_KIND_EXP_BINARY;
		op = OP_UNKOWN;
		left = nullptr;
		right= nullptr;
	}
	
	Ast_Node* left;
	Ast_Node* right;
	
};



struct Ast_Type : public Ast_Node {
	Ast_Type() {
		type = AST_TYPE;
	}
	
	u32 size;
	u32 alignment;
	bool is_signed;
	//u8* name;
};

struct Predefined_Type {
	char* key;
	Ast_Type value;
};





struct Ast_Declaration : public Ast_Node {
	// [X] name := value;
	// [X] name :  type  = value;
	// [X] name :: value;   // constant
	// [X] name :  type  : value; // constant
	// [X] name ::  ( <arguments>  ) -> ( <return types> )   { ... } 
	// [ ] name :: enum { ... }
	// [ ] name :: struct { ... }
	// [ ] name :: struct (   ) { ... }
	
	Ast_Declaration(){
		type = AST_DECLARATION;
	}
	
	Ast_Node*  data_type; // for now this is ident
	Ast_Node*  params; // if this is a procedure call.. ?? 
	bool 	  constant;
	bool 	  inforced_type;
	Ast_Scope*     scope;
};

struct Ast_Proc_Declaration : public Ast_Declaration {
	
	Ast_Proc_Declaration() {
		Ast_Declaration();
		kind = AST_KIND_DECL_PROCEDURE;
	}
	
	Ast_Type* return_type;
	Ast_Block* body;
};


struct Ast_Var_Declaration : public Ast_Declaration {
	
	Ast_Var_Declaration() {
		Ast_Declaration();
		kind = AST_KIND_DECL_VARIABLE;
	}
	
	Ast_Type* data_type;
	Ast_Node* body;
};

struct Ast_Scope {
	Ast_Var_Declaration** variables = {0};
	Ast_Proc_Declaration** procedures = {0};
	Ast_Node** types = {0};;
	
	
	//Ast_Node** nodes;
	Ast_Scope* parent = nullptr; // One item
	
	Ast_Scope* children = {0}; // Array
};

