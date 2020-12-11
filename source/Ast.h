/* ========================================================================
   $File: Ast.h
   $Date: 2020-10-03
   $Creator: Husam Dababneh
   $Description: Ast.h
   ========================================================================*/
#pragma once

#include "String.h"
#include "array.h"
#include "lex.h"

enum {
	AST_BLOCK,
	AST_DEFINETION,
	AST_IF,
	AST_WHILE,
	AST_BINARY_EXP,
	AST_LITERAL,
	AST_UKNOWN,
};


struct Symbol {
	String name;
	u32 type;
	s32 Scope; // This is Hash 
};

enum OpType {
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV
};


struct Ast_Declaration;


struct Ast_Node {
	u32 type = AST_UKNOWN; // ?? 
	Token token;
};

struct Ast {
	Ast_Node* rootNode;
	// I made this a struct becuase maybe sometime in the future we wanted to add some information about the ast
};

struct Ast_Literal : Ast_Node
{
	Ast_Literal(){
		type = AST_LITERAL;
	}
	//Token* token;
};

struct Ast_Binary : public Ast_Node {
	//  <a> ? <b>
	Ast_Binary() {
		type = AST_BINARY_EXP;
	}
	
	OpType op;
	Ast_Node* left;
	Ast_Node* right;
	
};

struct Ast_Expresion : public Ast_Node {
	Ast_Node* subExp;
	// { ... }
	// 
};

struct Ast_Body : public Ast_Node {
	// { ... }
	Array<Ast_Node*> expresions;
};




struct Ast_ParmeterList : public Ast_Node {
	// ( name : type, name : type ... )
	Array<Ast_Declaration> declerations;
};

struct Ast_ArgumentList : public Ast_Node {
	// ( name : type, name : type ... )
	//	Array<Ast_Literal> declerations;
};

enum {
	AST_VALUE_FLOAT,
	AST_VALUE_INT,
	AST_VALUE_STRING,
	
};

struct Ast_Value : public Ast_Node {
	u8 ValueType;
	union {
		u64 unsinged_value;
		u64 singed_value;
		float64 float_value;
	};
};


struct Ast_Declaration : public Ast_Node {
	// name : value
	// name := value;
	// name :: value;   // constant
	// name :  type  = value;
	// name :  type  : value; // constant
	// name ::  ( <arguments>  ) -> ( <return types> )   { ... } 
	// name :: struct { ... }
	// name :: strucct (   ) { ... }
	
	
	// name = "value" ---> in this case the type refers to the type's value not the name 
	
	String name;
	String type;
	union {
		String value;
		struct {
			Ast_ParmeterList* parmeterList;
			Ast_Body* body;
		};
	};
	
};

struct Ast_While : public Ast_Node {
	Ast_Expresion* exp;
	Ast_Body body;
};







