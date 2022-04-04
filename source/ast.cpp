#include "Ast.h"


bool 
is_variable(Ast_Expression* node) {
	if (node->kind == AST_KIND_PRIMARY_IDENTIFIER) return true;
	
	return false;
	
}

static inline void
AllocateNodeEx(Ast_Node* node)
{
	node->token = {};
}

static inline void
AllocateNodeEx(Ast_Scope* node)
{
	AllocateNodeEx((Ast_Node*) node);
	//printf("Not Implemented Yet (Ast_Scope)\n");
	//exit(-1);
}

static inline void
AllocateNodeEx(Ast_Block* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_BLOCK;
	node->kind = AST_KIND_BLOCK;
}

static inline void
AllocateNodeEx(Ast_Expression* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_EXPRESSION;
	node->resulting_type = nullptr;
}

static inline void
AllocateNodeEx(Ast_Literal* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_LITERAL;
}

static inline void
AllocateNodeEx(Ast_Primary* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	//node->kind = AST_KIND_EXP_PRIMARY;
}

static inline void
AllocateNodeEx(Ast_Unary* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_UNARY;
	node->child = nullptr; // List
}

static inline void
AllocateNodeEx(Ast_Proc_Call* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_PROC_CALL;
	node->arguments = nullptr;
}

static inline void
AllocateNodeEx(Ast_Member_Access* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_MEM_ACC;
}

static inline void
AllocateNodeEx(Ast_Subscript* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_SUBSCRIPT;
}

static inline void
AllocateNodeEx(Ast_Binary* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_BINARY;
	node->left = nullptr;
	node->right= nullptr;
}

static inline void
AllocateNodeEx(Ast_Type* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_TYPE;
	node->kind = AST_KIND_TYPE;
	node->variables = nullptr;
}

static inline void
AllocateNodeEx(Predefined_Type* node)
{
	printf("Not Implemented Yet (Predefined_Type)\n");
	exit(-1);
}

static inline void
AllocateNodeEx(Ast_Return* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Expression*)node);
	node->kind = AST_KIND_EXP_RETURN;
}

static inline void
AllocateNodeEx(Ast_Declaration* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_DECLARATION;
}

static inline void
AllocateNodeEx(Ast_Proc_Declaration* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Declaration*)node); 
	node->kind = AST_KIND_DECL_PROCEDURE;
	node->constant = true;
}

static inline void
AllocateNodeEx(Ast_Var_Declaration* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Declaration*)node); 
	node->kind = AST_KIND_DECL_VARIABLE;
	node->data_type = nullptr;
}

static inline void
AllocateNodeEx(Ast_Struct_Declaration* node)
{
	AllocateNodeEx((Ast_Node*) node);
	AllocateNodeEx((Ast_Declaration*)node); 
	node->kind = AST_KIND_DECL_STRUCT;
}

static inline void
AllocateNodeEx(Ast_If* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_IF;
	node->kind = AST_KIND_IF;
}

static inline void
AllocateNodeEx(Ast_Note* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_NOTE;
}

static inline void
AllocateNodeEx(Ast_Directive* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->type = AST_DIRECTIVE;
	
}

static inline void
AllocateNodeEx(Ast_Directive_Import* node)
{
	AllocateNodeEx((Ast_Node*) node);
	node->kind = AST_KIND_DIR_IMPORT;
	node->isAs = false;
}
