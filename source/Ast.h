/* ========================================================================
   $File: Ast.h
   $Date: 2020-10-03
   $Creator: Husam Dababneh
   $Description: Ast.h
   ========================================================================*/
#pragma once

#include "array.h"
#include "lex.h"



enum {
	AST_IDENT,
	AST_ASSIGN,
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
	AST_UKNOWN,
};


struct Symbol {
	String name;
	u32 type;
	s32 Scope; // This is Hash 
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
	Ast_Node* rootNode;
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
	// name = "value" ---> in this case the type refers to the type's value not the name 
	
	Ast_Ident* ident;  //Name of the symbol
	Ast_Node*  body; // this could be a function body, expression, 
	Ast_Node*  data_type; // for now this is ident
	bool 	  constant;
};





// @TODO: Maybe we want this for names arguments ??
struct Ast_Argument : public Ast_Node {
	Ast_Argument() {
		type = AST_ARGUMENT;
	}
	
	
	Ast_Ident* type_name;
	Ast_Ident* name;
	//  TODO
	
};

struct Ast_Parmeter : public Ast_Node {
	Ast_Parmeter() {
		type= AST_PARMETER;
	}
	
};

struct Ast_FunctionCall : public Ast_Node {
	Ast_FunctionCall() {
		type = AST_FUNCALL;
		arguments = init_array<Ast_Node*>(5);
	}
	
	Array<Ast_Node*> arguments;
};

struct Ast_Block : public Ast_Node {
	Ast_Block(){
		type = AST_BLOCK;
		statements = init_array<Ast_Node*>(10);
	}
	
	Array<Ast_Node*> statements;
};


struct Ast_Struct : public Ast_Declaration  {
	Ast_Struct() {
		type = AST_STRUCT;
		constant = true;
		fields = init_array<Ast_Declaration*>(5);
	}
	Array<Ast_Declaration*> fields;
};


struct Ast_ProcDecl : public Ast_Declaration  {
	Ast_ProcDecl() {
		type = AST_PORCDECLARATION;
		constant = true;
		arguments = init_array<Ast_Declaration*>(5);
	}
	//Array<Ast_Declaration*> return_types;
	Ast_Ident * return_type; // TODO: 
	Array<Ast_Declaration*> arguments;
};
