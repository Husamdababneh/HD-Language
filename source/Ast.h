/* ========================================================================
   $File: Ast.h
   $Date: 2020-10-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Ast.h
   ========================================================================*/
#pragma once

#include "String.h"
#include "Array.h"

enum NODE_TYPE {
	AST_BLOCK,
	AST_DEFINETION,
	AST_IF,
	AST_WHILE,
	
};

enum OpType {
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV
};


struct Ast_Declaration;


struct Ast_Node {
	NODE_TYPE type; // ?? 
};



struct BinaryOp : public Ast_Node {
	//  <a> ? <b>
	OpType op;
	Ast_Node* left;
	Ast_Node* right;
	
};

struct Ast_Expresion : public Ast_Node {
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
	// name := value;
	// name :: value;   // constant
	// name :  type  = value;
	// name :  type  : value; // constant
	// name :: struct { ... }
	// name :: struct (   ) { ... }

	
	// name = "value" ---> in this case the type refers to the type's value not the name 
	
	String name;
	String type;
	union {
		String value;
		struct {
			Ast_ParmeterList parmeterList;
			Ast_Body body;
		};
	};

};

struct Ast_While : public Ast_Node {
	Ast_Expresion* exp;
	Ast_Body body;
};
	






