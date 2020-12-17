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




#if 0
struct Ast_Declaration;


struct Ast_Node {
	u32 type;
	Token token;
	
	virtual Ast_Node () : type(AST_UKNOWN) {
		
	}
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
		op = OP_UNKOWN;
		left = nullptr;
		right= nullptr;
	}
	
	u64       op;
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
	Array<Ast_Declaration*> declerations;
};

struct Ast_ArgumentList : public Ast_Node {
	// ( name : type, name : type ... )
	Array<Ast_Literal*> declerations;
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



struct Ast_While : public Ast_Node {
	Ast_Expresion* exp;
	Ast_Body body;
};

#endif

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
	//Token* token;
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

struct Ast_Type : public Ast_Node {
	Ast_Type() {
		type = AST_TYPE;
	}
	
	
	
};

struct Ast_Declaration ;
struct Ast_ParmeterList : public Ast_Node {
	// ( name : type, name : type ... )
	Array<Ast_Declaration*> declerations;
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
	
	Ast_Declaration(){
		type = AST_DECLARATION;
	}
	// name = "value" ---> in this case the type refers to the type's value not the name 
	
	String decl_type;
	String name;
	union {
		String value;
		struct {
			Ast_ParmeterList* parmeterList;
			//Ast_Body* body;
		};
	};
	
};

struct Ast_Ident : public Ast_Node {
	Ast_Ident() {
		type = AST_IDENT;
	}
	
};



// @TODO: Maybe we want this for names arguments ??
#if 0
struct Ast_Argument : public Ast_Node {
	Ast_Argument() {
		type = AST_ARGUMENT;
	}
	
	
	Ast_Ident* type;
	Ast_Ident* name;
	//  TODO
	
};
#endif

struct Ast_Parmeter : public Ast_Node {
	Ast_Parmeter() {
		type= AST_PARMETER;
	}
	
};


struct Ast_FunctionCall : public Ast_Node {
	Ast_FunctionCall() {
		type = AST_FUNCALL;
		arguments = init_array<Ast_Ident*>(5);
	}
	
	Array<Ast_Ident*> arguments;
};

struct Ast_Assign : public Ast_Node {
	// name : value
	// name := value;
	// name :: value;   // constant
	// name :  type  = value;
	// name :  type  : value; // constant
	// name ::  ( <arguments>  ) -> ( <return types> )   { ... } 
	// name :: struct { ... }
	// name :: strucct (   ) { ... }
	
	Ast_Assign(){
		type = AST_ASSIGN;
	}
	// name = "value" ---> in this case the type refers to the type's value not the name 
	Ast_Ident* ident; 
	Ast_Node* exp;
};

