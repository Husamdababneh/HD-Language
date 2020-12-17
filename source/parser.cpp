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

#define PARSE_COMMAND(name) name(LexerState* lexer, Logger* logger)
#define CallParseCommand(command) command(lexer, logger);

#define AllocateNode(type, name) type* name =  allocate<type>(&ast_arena);


static Arena ast_arena = make_arena(1024 * 1024); // 1MB ?? 



inline Ast_Binary*
PARSE_COMMAND(parse_operator)
{
	Token token = lexer->eat_token();
	
	
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
		{
			return nullptr;
		}
		default: 
		{
			logger->print_with_location(&token, "Unexpected Token [%s] :"_s, token.name);
			abort();
			break;
		}
	}
	
	Ast_Binary* exp = allocate<Ast_Binary>(&ast_arena);
	exp->token = token;
	exp->op = op;
	return exp;
	
}

static inline  Ast_Node*
PARSE_COMMAND(parse_one_function_call_argument)
{
	Token token = lexer->peek_next_token();
	if( token.Type == ')')
		return nullptr;
	
	AllocateNode(Ast_Ident, argument);
	
	argument->token = lexer->eat_token();
	
	return argument;
}

static Ast_Node*
PARSE_COMMAND(parse_function_call)
{
	AllocateNode(Ast_FunctionCall, function_call);
	
	Token token = lexer->eat_token(); // function name 
	function_call->token = token;
	
	while(true){
		Ast_Node* node = CallParseCommand(parse_one_function_call_argument);
		if (node == nullptr)
			break;
		
		if (node->type != AST_IDENT)
			abort();
		
		Ast_Ident* arg = (Ast_Ident*)node; 
		array_add(&function_call->arguments, arg);
	}
	
	
	return function_call; 
}

static Ast_Node*
PARSE_COMMAND(parse_subexpression)
{
	
	Token token = lexer->eat_token();
	switch(token.Type){
		case TOKEN_IDENT:
		{
			if(lexer->peek_next_token().Type == '(')
				AllocateNode(Ast_Ident, ident);
			
			break;
		}
		case TOKEN_LITERAL:
		{
			AllocateNode(Ast_Literal, literal);
			literal->token = token;
			return literal;
			break;
		}
		case '!':
		case '-':
		{
			AllocateNode(Ast_Unary, unary);
			//unary->op = token.Type;
			unary->token = token;
			unary->child = parse_subexpression(lexer,logger);
			return unary;
		}
		default: 
		{
			logger->print_with_location(&token, "Unexpected Token [%s] :"_s, token.name);
			abort();
			break;
		}
	}
	assert(true);
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
PARSE_COMMAND(parse_directive)
{
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_statement)
{
	//Ast_Declaration* decl = allocate<Ast_Declaration>(&ast_arena);
	Token ident_token = lexer->eat_token();
	//decl->name = token.name;
	
	Token token = lexer->peek_next_token();
	switch(token.Type)
	{
		case TOKEN_DOUBLECOLON: 
		{
			// Constatnts, Function and Lambda eventually
			//decl->decl_type = "FUNCTION OR CONSTANT"_s;
			break;
		}
		case TOKEN_ASSIGN:
		{
			AllocateNode(Ast_Assign, assign);
			assign->token = lexer->eat_token();
			
			AllocateNode(Ast_Ident, ident);
			ident->token = ident_token;
			
			Ast_Node* node = parse_expression(lexer, logger); 
			
			assign->ident = ident;
			assign->exp = node;
			
			return assign;
			//decl->decl_type = "NOT FUNCTION NOR  CONSTANT"_s;
			break;
		}
		
	}
	
	//logger->print("%s \n"_s, decl->decl_type);
	
	//token = lexer->eat_token();
	
	//return decl;
	return nullptr;
}



static Ast 
PARSE_COMMAND(parse)
{
	int a = 0;
	auto token = lexer->peek_next_token();
	while(token.Type != TOKEN_EOFA){
		switch(token.Type)
		{
			case TOKEN_IDENT:
#if 0
			case TOKEN_LITERAL:
			// @NOCHECKIN
			case '-':
			case '!':
#endif
			{
				logger->print("Token Name = [%s]\n"_s, token.name);
				
				
				Ast_Node* node = CallParseCommand(parse_function_call);
				//Ast_Node* node = parse_statement(lexer, logger);
				
				// @CleanUp(Husam): reset the array ... Do we need a "method for this";
				
				PRINT_GRAPH(node, logger);
				
				//Ast_Node* node = parse_declaration(lexer, logger);
				/*Ast_Node* node  = parse_expression(lexer, logger
);
*/
				//lexer->eat_token();
				break;
			}
			default:
			{
				// This should be an error eventually.. since we are not really handleing it
				lexer->eat_token();
				break;
			}
		}
		token = lexer->peek_next_token();
	}	
	return {0};
}

void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);
	
	parse(&lexer, &logger);
}




