#include "lex.h"
const StringView token_type_to_string(u64 c){
	switch(c){
		case TOKEN_SEMI_COLON: return "TOKEN_SEMI_COLON"_s;  
		case TOKEN_COMMA: return "TOKEN_COMMA"_s;  
		case TOKEN_EQUAL: return "TOKEN_EQUAL"_s;  
		case TOKEN_IDENT: return "TOKEN_IDENT"_s;  
		case TOKEN_HDTYPE: return "TOKEN_HDTYPE"_s;  
		case TOKEN_KEYWORD: return "TOKEN_KEYWORD"_s;  
		case TOKEN_LITERAL: return "TOKEN_LITERAL"_s;  
		case TOKEN_COMMENT: return "TOKEN_COMMENT"_s;  
		case TOKEN_MULTILINE_COMMENT: return "TOKEN_MULTILINE_COMMENT"_s;  
		case TOKEN_COLON: return "TOKEN_COLON"_s;  
		case TOKEN_DOUBLE_COLON: return "TOKEN_DOUBLE_COLON"_s;  
		case TOKEN_COLON_EQUAL: return "TOKEN_COLON_EQUAL"_s;  
		case TOKEN_OPERATOR: return "TOKEN_OPERATOR"_s;  
		case TOKEN_DIRECTIVE: return "TOKEN_DIRECTIVE"_s;  
		case TOKEN_DOUBLEDOT: return "TOKEN_DOUBLEDOT"_s;  
		case TOKEN_ARROW: return "TOKEN_ARROW"_s;  
		case TOKEN_ASSIGN: return "TOKEN_ASSIGN"_s;  
		case TOKEN_EQL: return "TOKEN_EQL"_s;  
		case TOKEN_GT: return "TOKEN_GT"_s;  
		case TOKEN_LT: return "TOKEN_LT"_s;  
		case TOKEN_GT_OR_EQL: return "TOKEN_GT_OR_EQL"_s;  
		case TOKEN_LT_OR_EQL: return "TOKEN_LT_OR_EQL"_s;  
		case TOKEN_SHIFT_LEFT: return "TOKEN_SHIFT_LEFT"_s;  
		case TOKEN_SHIFT_RIGHT: return "TOKEN_SHIFT_RIGHT"_s;  
		case TOKEN_NONE: return "TOKEN_NONE"_s;  
		case TOKEN_EOFA: return "TOKEN_EOFA"_s;  
		case TOKEN_ERROR: return "TOKEN_ERROR"_s;  
		default: return ""_s;
	}
}
#include "ast.h"
const StringView ast_type_to_string(u64 c){
	switch(c){
		case AST_DECLARATION: return "AST_DECLARATION"_s;  
		case AST_EXPRESSION: return "AST_EXPRESSION"_s;  
		case AST_IF: return "AST_IF"_s;  
		case AST_WHILE: return "AST_WHILE"_s;  
		case AST_BLOCK: return "AST_BLOCK"_s;  
		case AST_TYPE: return "AST_TYPE"_s;  
		case AST_DIRECTIVE: return "AST_DIRECTIVE"_s;  
		case AST_UKNOWN: return "AST_UKNOWN"_s;  
		default: return ""_s;
	}
}
#include "ast.h"
const StringView ast_kind_to_string(u64 c){
	switch(c){
		case AST_KIND_UNKNOWN: return "AST_KIND_UNKNOWN"_s;  
		case AST_KIND_EXP_BINARY: return "AST_KIND_EXP_BINARY"_s;  
		case AST_KIND_EXP_UNARY: return "AST_KIND_EXP_UNARY"_s;  
		case AST_KIND_EXP_PRIMARY: return "AST_KIND_EXP_PRIMARY"_s;  
		case AST_KIND_EXP_LITERAL: return "AST_KIND_EXP_LITERAL"_s;  
		case AST_KIND_EXP_RETURN: return "AST_KIND_EXP_RETURN"_s;  
		case AST_KIND_EXP_PROC_CALL: return "AST_KIND_EXP_PROC_CALL"_s;  
		case AST_KIND_EXP_MEM_ACC: return "AST_KIND_EXP_MEM_ACC"_s;  
		case AST_KIND_EXP_SUBSCRIPT: return "AST_KIND_EXP_SUBSCRIPT"_s;  
		case AST_KIND_PRIMARY_IDENTIFIER: return "AST_KIND_PRIMARY_IDENTIFIER"_s;  
		case AST_KIND_LITERAL_NUMBER: return "AST_KIND_LITERAL_NUMBER"_s;  
		case AST_KIND_LITERAL_STRING: return "AST_KIND_LITERAL_STRING"_s;  
		case AST_KIND_DECL_PROCEDURE: return "AST_KIND_DECL_PROCEDURE"_s;  
		case AST_KIND_DECL_VARIABLE: return "AST_KIND_DECL_VARIABLE"_s;  
		case AST_KIND_DECL_STRUCT: return "AST_KIND_DECL_STRUCT"_s;  
		case AST_KIND_DIR_IMPORT: return "AST_KIND_DIR_IMPORT"_s;  
		default: return ""_s;
	}
}
