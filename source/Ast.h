/* ========================================================================
$File: Ast.h
$Date: 2020-10-03
$Creator: Husam Dababneh
$Description: Ast.h
========================================================================*/
#pragma once

#include "lex.h"
//Note(GenerateStrings, ast_type)

#define AllocateNode(type, name)\
type* name = PushStruct(arena, type);\
AllocateNodeEx(name);\
arrput(flaten_ast,(Ast_Node*)name);

enum AstType : U16 {
	AST_UKNOWN = 0,
	AST_DECLARATION,
	AST_EXPRESSION,
	AST_IF,
	AST_WHILE,
	AST_BLOCK,
	AST_TYPE,
	AST_DIRECTIVE,
	AST_NOTE,
	
	
	AST_TYPE_MAX,
};

// Note(GenerateStrings, ast_kind) 
enum AstKind : U16 {
	AST_KIND_UNKNOWN = 0,
	
	// Expressions Kinds
	AST_KIND_EXP_BINARY,
	AST_KIND_EXP_UNARY,
	AST_KIND_EXP_LITERAL,
	AST_KIND_EXP_RETURN,
	AST_KIND_EXP_PROC_CALL,
	AST_KIND_EXP_MEM_ACC,
	AST_KIND_EXP_SUBSCRIPT,
	
	// Primary Literal Kinds
	AST_KIND_LITERAL_NUMBER,
	AST_KIND_LITERAL_STRING,
	
	// Declaration Kinds
	AST_KIND_DECL_PROCEDURE,
	AST_KIND_DECL_VARIABLE,
	AST_KIND_DECL_STRUCT,
	
	// Directive Kinds
	AST_KIND_DIR_IMPORT,
	
	// If/else/elseif Kinds
	AST_KIND_IF,
	
	// Types Kinds
	AST_KIND_TYPE,
	
	// Primary Expression Kinds
	AST_KIND_PRIMARY_IDENTIFIER,
	
	// Blocks kinds -> maybe useful for lambdas 
	AST_KIND_BLOCK,
	
	AST_KIND_MAX,
};

// NOTE(Husam Dababneh): Do we need this ?? 

enum AST_BINARY_TYPE : U16 {
	AST_BINARY_NONE = 0, 
	
	AST_BINARY_PLUS,
	AST_BINARY_MINUS,
	AST_BINARY_MUL,
	AST_BINARY_DIV,
	AST_BINARY_ASSIGN,
	
	AST_BINARY_IS_EQL,
	
	AST_BINARY_MAX,
};

struct Ast_Node;
struct Ast_Scope;
struct Ast_Type;
struct Ast_Declaration;


struct Ast {
	Ast_Node** nodes;
};


struct Ast_Node {
	AstType type;
	AstKind kind;
	Token   token;
};

struct Ast_Block : Ast_Node {
	Ast_Node** statements;
	Ast_Scope* scope;
};



struct Ast_Expression : Ast_Node {
	Ast_Type* resulting_type;
};

struct Ast_Literal : Ast_Expression {
};


struct Ast_Primary : Ast_Expression {
};


struct Ast_Unary : Ast_Expression {
	Ast_Node* child;
};

struct Ast_Proc_Call : Ast_Expression {
	Ast_Expression** arguments;
	Ast_Expression*  procedure;
};

struct Ast_Member_Access: Ast_Expression {
	Token            member; // ?? 
	Ast_Primary*     _struct;
};

struct Ast_Subscript : Ast_Expression {
	Ast_Expression* exp;
	Ast_Expression* value;
};


struct Ast_Binary : Ast_Expression {
	AST_BINARY_TYPE op;
	Ast_Expression* left;
	Ast_Expression* right;
};


// @Incomplete: Is this really an AST node ?? 
struct Ast_Type : public Ast_Node {
	U32 size;
	U32 alignment;
	bool is_signed;
	Ast_Declaration* variables;
};

struct Predefined_Type {
	char* key;
	Ast_Type value;
};


struct Ast_Return : Ast_Expression {
	Ast_Expression** expressions;
};


struct Ast_Declaration : public Ast_Node {
	// In What scope this decl is 
	Ast_Scope*     scope;
	bool 	      constant;
	bool 	      inforced_type;
};

struct Ast_Proc_Declaration : public Ast_Declaration {
	Ast_Type** return_type;
	Ast_Block* body;
};


struct Ast_Var_Declaration : public Ast_Declaration {
	Ast_Type* data_type;
	Ast_Expression* body;
};

struct Ast_Struct_Declaration : public Ast_Declaration {
	Ast_Declaration** decls;
};

struct Ast_If : public Ast_Node {
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
};



struct Ast_Directive_Import : Ast_Node {
	// Token have the location in file 
	Token filename; 
	// NOTE(Husam Dababneh): I think we only need as, and check if it has a value
	// otherwise we didn't find the 'as' keyword
	bool isAs; 
	Token as;
};


bool is_variable(Ast_Expression* node);










