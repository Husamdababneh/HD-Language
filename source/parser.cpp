/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "pch.h"
#include "parser.h"
#include "lex.h"

// @TEMP @CLEANUP This may break the premake generated files
#define ENABLE_GRAPH_PRINTING 1
#include <extra/graph.cpp>
//

#include "../submodules/tracy/Tracy.hpp"

#define PARSE_COMMAND(name) name(LexerState* lexer, Logger* logger)
#define CallParseCommand(command) command(lexer, logger);

#define AllocateNode(type, name) type* name =  allocate<type>(&ast_arena);

#ifdef _DEBUG
#define PANIC() exit(0);//abort();
#else
#define PANIC() exit(-1);
#endif

static Arena ast_arena = make_arena(1024 * 1024); // 1MB ?? 
static u8 counter = 0;

static Ast_Node* PARSE_COMMAND(parse_expression);
static Ast_Node* PARSE_COMMAND(parse_factor);

inline Ast_Binary*
PARSE_COMMAND(parse_operator)
{
	Token token = lexer->peek_token();
	
	
	u32 op = 0; 
	switch(token.Type)
	{
		case '+':
		case '-':
		{
			op = 1;
			break;
		}
		case '*':
		case '/':
		{
			op = 2;
			break;
		}
		case ';':
		case ',':
		case ')':
		{
			// @NOTE(Husam) : These token are somewhat expected here .. right ?? 
			return nullptr;
		}
		default: 
		{
			//@CleanUp(Husam): Should we report error here ?? since we are reporting 
			logger->print_with_location(&token, "[parse_operator] Unexpected Token [%s]\n"_s, token.name);
			
			// @ROBUSTNESS @NOTE(Husam) : No need for panic here  
			return nullptr;
		}
	}
	lexer->eat_token();
	AllocateNode(Ast_Binary, exp);
	exp->token = token;
	exp->op = op;
	return exp;
	
}

static Ast_Node*
PARSE_COMMAND(parse_function_call)
{
	AllocateNode(Ast_FunctionCall, function_call);
	// eat '('
	Token token = lexer->eat_token();
	while(true){
		token = lexer->peek_token();
		
		if (token.Type == ')'){
			//lexer->eat_token();
			break;
		}
		
		Ast_Node* node = CallParseCommand(parse_expression);
		array_add(&function_call->arguments, node);
		
		token = lexer->peek_token();
		
		if (token.Type != ',')
			break;
		
		// eat the ','
		lexer->eat_token();
	}
	return function_call; 
}

static bool from_the_start = false;

static Ast_Node*
PARSE_COMMAND(parse_subexpression)
{
	ZoneScoped;
	Token token = lexer->peek_token();
	switch(token.Type){
		case TOKEN_IDENT:
		{
			token = lexer->eat_token();
			// Function call 
			if(lexer->peek_token().Type == '(')
			{
				Ast_FunctionCall* node  = (Ast_FunctionCall*)CallParseCommand(parse_function_call);
				node->token = token;
				token = lexer->peek_token();
				if (token.Type != ')')
				{
					logger->print_with_location(&token, "Expected [(] got [%s]"_s, token.name  );
					PANIC();
				}
				
				lexer->eat_token();
				return node;
			}
			else
			{
				AllocateNode(Ast_Ident, ident);
				ident->token = token;
				return ident;
			}
		}
		case TOKEN_LITERAL:
		{
			AllocateNode(Ast_Literal, literal);
			literal->token = lexer->eat_token();
			return literal;
		}
		case '!':
		case '-':
		{
			AllocateNode(Ast_Unary, unary);
			//unary->op = token.Type;
			unary->token = lexer->eat_token();
			unary->child = CallParseCommand(parse_subexpression);
			return unary;
		}
		case '(':
		{
			return CallParseCommand(parse_factor);
		}
		case TOKEN_COMMENT: 
		case TOKEN_MULTILINE_COMMENT: 
		{
			lexer->eat_token();
			return CallParseCommand(parse_subexpression);
			break;
		}
		default: 
		{
			logger->print_with_location(&token, "[parse_subexpression] Unexpected Token [%s] :"_s, token.name);
			PANIC();
			break;
		}
	}
	assert(false);
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_expression)
{
	Ast_Node* left = parse_subexpression(lexer, logger);
	if (!left) return nullptr;
	
	Ast_Binary* exp = parse_operator(lexer, logger);
	if (!exp) return left;
	
	Ast_Node* right = parse_expression(lexer, logger);
	if (!right) return nullptr;;
	
	exp->left = left;
	if (right->type == AST_BINARY_EXP)
	{
		Ast_Binary* rightSide = (Ast_Binary*)right;
		if(rightSide->op < exp->op)
		{
			exp->right = rightSide->left;
			rightSide->left = exp;
			return rightSide;
		}
		
	}
	
	exp->right = right;
	return exp;
}



static Ast_Node*
PARSE_COMMAND(parse_factor)
{
	Token token = lexer->peek_token();
	
	if (token.Type == ';') return nullptr;
	
	if (token.Type == '(')
	{
		lexer->eat_token();
		
		AllocateNode(Ast_Factor, factor);
		factor->node = CallParseCommand(parse_expression);
		
		token = lexer->peek_token();
		
		if (token.Type != ')') {
			logger->print_with_location(&token, "Expected [)] here !!\n"_s);
			PANIC();
		}
		
		lexer->eat_token();
		
		return factor;
	}
	else 
	{
		return CallParseCommand(parse_expression);
	}
}

static bool
PARSE_COMMAND(check_if_proc)
{
	Token token = lexer->peek_token(1);
	
	return token.name.isEqual("proc"_s); 
}

/*
AllocateNode(Ast_ProcDecl, proc);
// @TODO @UX : This should be range right ?? -_-.
proc->token = token;
proc->
*/

static Ast_Node*
PARSE_COMMAND(parse_porc_declaration)
{
	// ::
	Token token = lexer->eat_token();
	logger->print("%s\n"_s, token.name);
	// proc 
	token = lexer->eat_token();
	logger->print("%s\n"_s, token.name);
	// :: 
	token = lexer->eat_token();
	logger->print("%s\n"_s, token.name);
	// ( 
	//token = lexer->eat_token();
	if (token.Type != '(')
	{
		logger->print_with_location(&token, "Excpected '(' and got [%s]"_s, token.name);
		PANIC();
	}
	
	//AllocateNode(Ast_ProcDecl, proc);
	while(token.Type != ')'){
		
		// name/type 
		//  :
		// type
		// optional = 
		// value iff = 
		// if , continue ; if ) break; else error 
		
		token = lexer->eat_token();
	}
	
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_statement)
{
	
	Ast_Node* return_node = nullptr;
	
	// We know it's identifer
	Token ident_token = lexer->eat_token();
	
	Token token = lexer->peek_token();
	switch(token.Type)
	{
		case TOKEN_ASSIGN:
		case TOKEN_DOUBLECOLON:
		case TOKEN_COLONEQUAL:
		{
			
			if (!check_if_proc(lexer, logger)){
				lexer->eat_token();
				
				AllocateNode(Ast_Declaration, decl);
				decl->body = CallParseCommand(parse_expression);
				decl->token = token;
				
				AllocateNode(Ast_Ident, ident);
				ident->token = ident_token;
				decl->ident = ident;
				
				if (token.Type == TOKEN_DOUBLECOLON) decl->constant = true;
				return_node = decl;
			} else {
				Ast_ProcDecl* decl = (Ast_ProcDecl*) CallParseCommand(parse_porc_declaration);
				if (!decl) {
					// TODO: @CleanUp
					return nullptr;
				}
				AllocateNode(Ast_Ident, ident);
				ident->token = ident_token;
				decl->ident = ident;
				
				exit(0);
			}
			break;
		}
		case TOKEN_COLON: 
		{
			AllocateNode(Ast_Declaration, decl);
			// name 
			AllocateNode(Ast_Ident, ident);
			ident->token = ident_token;
			decl->ident = ident;
			// : 
			decl->token = lexer->eat_token();
			
			// TYPE / IDENT | HDTYPE
			AllocateNode(Ast_Ident, type);
			type->token = lexer->eat_token();
			decl->data_type = type;
			
			if (lexer->peek_token().Type == TOKEN_COLON)
				decl->constant = true;
			
			if (lexer->peek_token().Type == TOKEN_ASSIGN ||
				lexer->peek_token().Type == TOKEN_COLON){
				lexer->eat_token();
				decl->body = CallParseCommand(parse_expression);
			}
			return_node = decl;
			// optinal = | : 
			// EXP iff = | :
			break;
		}
		default : {
			abort();
		}
		
	}
	
	
	token = lexer->peek_token();
	if (token.Type != ';')
	{
		logger->print_with_location(&token, "Expected ';' here !!!\n"_s);
		PANIC();
	}
	
	lexer->eat_token();
	return return_node;
}

static Ast_Node* 
PARSE_COMMAND(parse_scope)
{
	
}


static Ast 
PARSE_COMMAND(parse)
{
	int a = 0;
	auto token = lexer->peek_token();
	while(token.Type != TOKEN_EOFA){
		switch(token.Type)
		{
			case TOKEN_IDENT:
			{
				logger->print("Token Name = [%s]\n"_s, token.name);
				Ast_Node* node = CallParseCommand(parse_statement);
				PRINT_GRAPH(node, logger);
				
				break;
			}
			//case '{': 
			{
				//logger->print("Scope"_s);
				//Ast_Node* scope = CallParseCommand(parse_scope);
				//panic();
			}
			default:
			{
				// This should be an error eventually.. since we are not really handleing it
				lexer->eat_token();
				break;
			}
		}
		token = lexer->peek_token();
	}	
	return {0};
}

void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);
	
	parse(&lexer, &logger);
	
}


