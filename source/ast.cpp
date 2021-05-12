#include "pch.h"

#include "Ast.h"


bool 
is_variable(Ast_Expression* node) {
	assert(node);
	
	if (node->kind == AST_KIND_EXP_PRIMARY && 
		((Ast_Primary *)node)->priamry_kind == AST_KIND_PRIMARY_IDENTIFIER) return true;
	
	return false;
	
}