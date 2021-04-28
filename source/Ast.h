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
	AST_IDENT,
	AST_ASSIGN,
	AST_MEBMER_ACCESS,
	AST_DECLARATION,
	AST_PORCDECLARATION,
	AST_TYPE,
	AST_BLOCK,
	AST_DEFINETION,
	AST_IF,
	AST_WHILE,
	AST_BINARY_EXP,
	AST_UNARY_EXP,
	AST_LITERAL,
	AST_FUNCALL,
	AST_ARGUMENT,
	AST_PARMETER,
	AST_FACTOR,
	AST_PROC,
	AST_STRUCT,
	AST_SUBSCRIPT,
	AST_LIST,
	AST_UKNOWN,
};


enum {
	OP_UNKOWN = -1,
	OP_ADD = 0,
	OP_SUB = 0,
	OP_MUL = 1,
	OP_DIV = 1
};


struct Ast_Node;

struct Ast {
	Ast_Node** nodes;
};


struct Ast_Node {
	u32 type = AST_UKNOWN; // ?? 
	Token token;
	
};

struct Ast_Literal : Ast_Node
{
	Ast_Literal(){
		type = AST_LITERAL;
	}
};


struct Ast_MemberAccess: Ast_Node
{
	Ast_MemberAccess()
	{
		type = AST_MEBMER_ACCESS;
	}
	
	Ast_Node* left;
	Ast_Node* right;
};

struct Ast_Assign: Ast_Node
{
	Ast_Assign(){
		type = AST_ASSIGN;
	}
	
	Ast_Node* left;
	Ast_Node* right;
};


struct Ast_Unary : public Ast_Node {
	//  <a> ? <b>
	Ast_Unary() {
		type = AST_UNARY_EXP;
		op = OP_UNKOWN;
		child= nullptr;
	}
	
	u32       op;
	Ast_Node* child;
	
};


struct Ast_Binary : public Ast_Node {
	//  <a> ? <b>
	Ast_Binary() {
		type = AST_BINARY_EXP;
		op = OP_UNKOWN;
		left = nullptr;
		right= nullptr;
	}
	
	bool isFactor = false;
	u32       op;
	Ast_Node* left;
	Ast_Node* right;
	
};

struct Ast_Factor : public Ast_Node {
	Ast_Factor(){
		type = AST_FACTOR;
	}
	
	Ast_Node* node;
};

struct Ast_Type : public Ast_Node {
	Ast_Type() {
		type = AST_TYPE;
	}
	
};

struct Ast_Ident : public Ast_Node {
	Ast_Ident() {
		type = AST_IDENT;
	}
};


struct Ast_Subscript : public Ast_Node {
	Ast_Subscript() {
		type = AST_SUBSCRIPT;
	}
	Ast_Node* exp;
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
	
	u64 kind;
	
	union {
		Ast_Node*  body; // this could be a function body, expression, struct Body, enum body ... 
		Ast_Node*  expression;
	};
	
	Ast_Node*  data_type; // for now this is ident
	Ast_Node*  params; // if this is a procedure call.. ?? 
	bool 	  constant;
	bool 	  inforced_type;
};




// TODO: Cleanup
struct Ast_List : public Ast_Node {
	Ast_List() { 
		type = AST_LIST; 
	}
	
	Ast_Declaration** list;
};




// @TODO: Maybe we want this for names arguments ??
struct Ast_Argument : public Ast_Node {
	Ast_Argument() {
		type = AST_ARGUMENT;
	}
	
	
	Ast_Ident* type_name;
	Ast_Ident* name;
	
};

struct Ast_Parmeter : public Ast_Node {
	Ast_Parmeter() {
		type= AST_PARMETER;
	}
	
};

struct Ast_FunctionCall : public Ast_Node {
	Ast_FunctionCall() {
		type = AST_FUNCALL;
	}
	
	Ast_Node** arguments;
};

struct Ast_Block : public Ast_Node {
	Ast_Block(){
		type = AST_BLOCK;
	}
	
	Ast_Node* enclosing_scope;
	Ast_Node** statements;
};

