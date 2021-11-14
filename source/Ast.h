/* ========================================================================
$File: Ast.h
$Date: 2020-10-03
$Creator: Husam Dababneh
$Description: Ast.h
========================================================================*/
#pragma once

#include "lex.h"

enum Note(GenerateStrings, ast_type) {
	AST_UKNOWN = 0,
	AST_DECLARATION,
	AST_EXPRESSION,
	AST_IF,
	AST_WHILE,
	AST_BLOCK,
	AST_TYPE,
	AST_DIRECTIVE,
};

enum Note(GenerateStrings, ast_kind) {
	AST_KIND_UNKNOWN = 0,
	// AST_KIND_
	
	// Expressions Kinds
	AST_KIND_EXP_BINARY,
	AST_KIND_EXP_UNARY,
	AST_KIND_EXP_PRIMARY,
	AST_KIND_EXP_LITERAL,
	AST_KIND_EXP_RETURN,
	AST_KIND_EXP_PROC_CALL,
	AST_KIND_EXP_MEM_ACC,
	AST_KIND_EXP_SUBSCRIPT,
	
	
	// Primary Expression Kinds
	AST_KIND_PRIMARY_IDENTIFIER,
	
	// Primary Literal Kinds
	AST_KIND_LITERAL_NUMBER,
	AST_KIND_LITERAL_STRING,
	
	// Declaration Kinds
	AST_KIND_DECL_PROCEDURE,
	AST_KIND_DECL_VARIABLE,
	AST_KIND_DECL_STRUCT,
	
	// Directive Kinds
	AST_KIND_DIR_IMPORT,
	
};

enum AST_BINARY_TYPE {
	AST_BINARY_PLUS,
	AST_BINARY_MINUS,
	AST_BINARY_MUL,
	AST_BINARY_DIV,
	AST_BINARY_ASSIGN,
	
	AST_BINARY_IS_EQL
};

struct Ast_Node;
struct Ast_Scope;

struct Ast {
	Ast_Node** nodes;
};


struct Ast_Node {
	U16 type;
	U16 kind;
	Token token;
};

struct Ast_Block : Ast_Node {
	Ast_Block() {
		type = AST_BLOCK;
	}
	Ast_Node** statements;
	Ast_Scope* scope;
};



struct Ast_Expression : Ast_Node {
	Ast_Expression() {
		type = AST_EXPRESSION; 
	}
};

struct Ast_Literal : Ast_Expression {
	Ast_Literal() {
		kind = AST_KIND_EXP_LITERAL;
	}
	
	S64 literal_kind;
};


struct Ast_Primary : Ast_Expression {
	
	Ast_Primary (){
		kind = AST_KIND_EXP_PRIMARY;
	}
	S64 priamry_kind ;
};


struct Ast_Unary : Ast_Expression {
	Ast_Unary() {
		kind = AST_KIND_EXP_UNARY;
		child = nullptr;
	}
	Ast_Node* child;
	
};

struct Ast_Proc_Call : Ast_Expression {
	Ast_Proc_Call() {
		kind = AST_KIND_EXP_PROC_CALL;
		arguments = nullptr;
	}
	Ast_Expression** arguments;
	Ast_Expression*  procedure;
};

struct Ast_Member_Access: Ast_Expression {
	Ast_Member_Access() {
		kind = AST_KIND_EXP_MEM_ACC;
	}
	Token            member;
	Ast_Primary*     _struct;
};

struct Ast_Subscript : Ast_Expression {
	Ast_Subscript () {
		kind = AST_KIND_EXP_SUBSCRIPT;
	}
	
	Ast_Expression* exp;
	Ast_Expression* value;
};


struct Ast_Binary : Ast_Expression {
	//  <a> ? <b>
	Ast_Binary() {
		kind = AST_KIND_EXP_BINARY;
		left = nullptr;
		right= nullptr;
	}
	
	S64 op;
	Ast_Node* left;
	Ast_Node* right;
	
};


// @Incomplete: Is this really an AST node ?? 
struct Ast_Type : public Ast_Node {
	Ast_Type() {
		type = AST_TYPE;
	}
	
	U32 size;
	U32 alignment;
	bool is_signed;
};

struct Predefined_Type {
	char* key;
	Ast_Type value;
};


struct Ast_Return : Ast_Expression{
	Ast_Return() {
		kind = AST_KIND_EXP_RETURN;
	}
	
	Ast_Expression** expressions;
};


struct Ast_Declaration : public Ast_Node {
	Ast_Declaration(){
		type = AST_DECLARATION;
	}
	// In What scope this decl is 
	Ast_Scope*     scope;
	bool 	      constant;
	bool 	      inforced_type;
};

struct Ast_Proc_Declaration : public Ast_Declaration {
	
	Ast_Proc_Declaration() {
		Ast_Declaration();
		kind = AST_KIND_DECL_PROCEDURE;
		constant = true;
	}
	Ast_Type** return_type;
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

struct Ast_Struct_Declaration : public Ast_Declaration {
	Ast_Struct_Declaration() {
		Ast_Declaration();
		kind = AST_KIND_DECL_STRUCT;
	}
	
	Ast_Declaration** decls;
};

struct Ast_If : public Ast_Node {
	Ast_If() {
		type = AST_IF;
		//kind = AST_KIND_IF;
	}
	
	Ast_Expression* exp;
	Ast_Node* statement;
	Ast_Node** next;
	
};

struct Ast_Scope {
	Ast_Var_Declaration** variables = nullptr;
	Ast_Proc_Declaration** procedures = nullptr;
	Ast_Struct_Declaration** structs = nullptr;
	//Ast_Type** types = nullptr;
	
	
	//Ast_Node** nodes;
	Ast_Scope* parent = nullptr; // One item
	
	Ast_Scope** children = nullptr; // Array
};

// TODO: Basic data for any Note
struct Ast_Note : Ast_Node {
};

// TODO: Basic data for any directive 
struct Ast_Directive : Ast_Node {
	Ast_Directive() {
		type = AST_DIRECTIVE;
	}
	
};



struct Ast_Directive_Import : Ast_Node {
	Ast_Directive_Import() {
		kind = AST_KIND_DIR_IMPORT;
		isAs = false;
	}
	
	// Token have the location in file 
	
	Token filename; 
	// NOTE(Husam Dababneh): I think we only need as, and check if it has a value
	// otherwise we didn't find the 'as' keyword
	bool isAs; 
	Token as;
};


bool is_variable(Ast_Expression* node);










