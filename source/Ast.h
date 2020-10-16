/* ========================================================================
   $File: Ast.h
   $Date: 2020-10-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Ast.h
   ========================================================================*/
#pragma once

#include "String.h"

/*
enum NODE_TYPE {
	AST_BLOCK,
	AST_DEFINETION,
	AST_IF,
	AST_WHILE,
	
};

struct Ast_Node {
	Type type; // ?? 
};

enum OpType {
	ADD,
	SUB,
	MUL,
	DIV
};

struct BinaryOp : public Ast_Node {
	//  <a> ? <b>
	OpType op;
	Ast_Node left;
	Ast_Node right
	
};

struct Ast_Expresion : public Ast_Node {
	// { ... }
	// 
};

struct Ast_Body : public Ast_Node {
	// { ... }
	Ast_Node* expresions;
};

struct Ast_Declaration : public Ast_Node {
	// name :: (arguments) -> (returns)
	
	String name;
	
	Ast_Node* arguments; // the problem with c/c++ that there is really no arrays :) ?. 
	Ast_Node* returns;   // These are arrays 
	
	Ast_Body* body;
};

struct Ast_While : public Ast_Node {
	Ast_Expresion* exp;
	Ast_Body* body;
};


struct Ast {
	Ast_Node* data;
};

	





*/
