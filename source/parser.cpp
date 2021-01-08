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

//static Ast_Node* PARSE_COMMAND(parse_statement);
static Ast_Node*  parse_statement(LexerState* lexer, Logger* logger, bool need_semi_colon = true);
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
	lexer->eat_token();
	while(true){
		//token = lexer->peek_token();
		
		if (lexer->peek_token().Type == ')'){
			//lexer->eat_token();
			break;
		}
		
		Ast_Node* node = CallParseCommand(parse_expression);
		array_add(&function_call->arguments, node);
		
		
		if (lexer->peek_token().Type != ','){
			//logger->print_with_location(&lexer->peek_token(), "[%s]\n"_s, lexer->peek_token().name);
			//assert(false);
			break;
		}
		
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


static Ast_Node*
PARSE_COMMAND(parse_porc_declaration)
{
	// (
	Token token = lexer->eat_token();
	if (token.Type != '(')
	{
		logger->print_with_location(&token, "Excpected '(' and got [%s]"_s, token.name);
		assert(false);
		//PANIC();
	}
	
	AllocateNode(Ast_ProcDecl, proc);
	// parse parmeters 
	// @TODO @IMPORTANT : check this .
	token = lexer->peek_token();
	while(true){
		
		// TODO : Rethink this part.
		Ast_Node* node = parse_statement(lexer, logger, false);
		if (node->type != AST_DECLARATION)
			assert(false);
		
		Ast_Declaration* decl = (Ast_Declaration*)node;
		if (decl->constant) assert(false);
		
		array_add(&proc->arguments, decl);
		
		//exit:
		if (lexer->peek_token().Type == ',') lexer->eat_token();
		else if (lexer->peek_token().Type == ')') { lexer->eat_token(); break;}
		else {
			logger->print_with_location(&token, "Excpected ':' and got [%s] [%s] [%s] \n"_s, lexer->peek_token().name,
										lexer->peek_token(1).name,
										lexer->peek_token(2).name);
			assert(false);// @TODO(Husam): Report Error
		}
	}
	
	if (lexer->peek_token().Type == TOKEN_ARROW) {
		lexer->eat_token();
		assert((lexer->peek_token(0).Type == TOKEN_IDENT) || (lexer->peek_token(0).Type == TOKEN_HDTYPE));
		
		AllocateNode(Ast_Ident, ident);
		ident->token = lexer->eat_token();
		proc->return_type = ident;
	}
	//parse statments
	// This should be parse block ?? 
	if (lexer->peek_token().Type != '{')  assert(false);
	AllocateNode(Ast_Block, block);
	block->token = lexer->eat_token();
	while(true){
		Ast_Node* node = CallParseCommand(parse_statement);
		if (node) array_add(&block->statements, node);
		
		if (lexer->peek_token().Type == '}')
			break;
		
	}
	proc->body = block;
	return proc;
}

static Ast_Node*
PARSE_COMMAND(parse_struct)
{
	
	Token token = lexer->eat_token();
	
	if (token.Type != '{') assert(false); // @TODO @Cleanup :  error message 
	
	
	
	AllocateNode(Ast_Struct, _struct);
	while(true) {
		if (lexer->peek_token().Type == '}') break;
		Ast_Node* node = CallParseCommand(parse_statement);
		if (node->type != AST_DECLARATION) assert(false);
		array_add(&_struct->fields, (Ast_Declaration*)node);
	}
	
	return _struct;
}

static Ast_Node*
//PARSE_COMMAND(parse_statement)
parse_statement(LexerState* lexer, Logger* logger, bool need_semi_colon)
{
	
	Ast_Node* return_node = nullptr;
	
	// We know it's identifer
	Token ident_token = lexer->peek_token();
	
	if (ident_token.Type == '}') //TODO: rethink this part .. ?? 
		return nullptr;
	
	
	Token token = lexer->peek_token(1);
	switch(token.Type)
	{
		case TOKEN_ASSIGN:
		case TOKEN_DOUBLECOLON:
		{
			//lexer->eat_token();
			Token temp = lexer->peek_token(1);
			Token maybe_struct = lexer->peek_token(2);
			if (maybe_struct.name.isEqual("struct"_s)){
				lexer->eat_token(); // ident 
				lexer->eat_token(); // :: 
				lexer->eat_token(); // struct  
				Ast_Struct* struct_decl  = (Ast_Struct*) CallParseCommand(parse_struct);
				if (!struct_decl) {
					// TODO: @CleanUp
					assert(false);
					return nullptr;
				}
				AllocateNode(Ast_Ident, ident);
				ident->token = ident_token;
				struct_decl->ident = ident;
				struct_decl->token = token;
				return struct_decl;
			} else if (maybe_struct.name.isEqual("proc"_s)){
				lexer->eat_token(); // ident 
				lexer->eat_token(); // :: 
				lexer->eat_token(); // proc  
				Ast_ProcDecl* decl = (Ast_ProcDecl*) CallParseCommand(parse_porc_declaration);
				if (!decl) {
					// TODO: @CleanUp
					assert(false);
					return nullptr;
				}
				AllocateNode(Ast_Ident, ident);
				ident->token = ident_token;
				decl->ident = ident;
				decl->token = token;
				return decl;
			}
		}
		case TOKEN_COLONEQUAL:
		{
			
			//ident_token = lexer->eat_token();
			Token temp = lexer->peek_token(1);
			Token maybe_proc = lexer->peek_token(2);
			// TODO : @CleanUp  
			
			// Eat := or ::
			lexer->eat_token();
			AllocateNode(Ast_Declaration, decl);
			decl->token = lexer->eat_token();
			decl->body = CallParseCommand(parse_expression);
			
			AllocateNode(Ast_Ident, ident);
			ident->token = ident_token;
			decl->ident = ident;
			
			if (token.Type == TOKEN_DOUBLECOLON) decl->constant = true;
			return_node = decl;
			break;
		}
		case TOKEN_COLON: 
		{
			ident_token = lexer->eat_token();
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
		//handle procedure calles 
		case '(': {
			return_node = CallParseCommand(parse_expression);
			break;
		}
		default : {
			logger->print_with_location(&token,"Got %s %d\n"_s, token.name, (u64)token.Type);
			assert(false);
		}
	}
	
	// TODO: Parse notes here
	if(need_semi_colon)
	{
		token = lexer->peek_token();
		if (token.Type != ';')
		{
			logger->print_with_location(&token, "Expected ';' here !!!\n"_s);
			PANIC();
		}
		lexer->eat_token();
	}
	return return_node;
}

static Ast_Node* 
PARSE_COMMAND(parse_scope)
{
	
	AllocateNode(Ast_Block, block);
	block->token = lexer->eat_token();
	Ast_Node* node = nullptr;
	while(true){
		if (lexer->peek_token().Type == '{') { node = CallParseCommand(parse_scope);}
		else node = CallParseCommand(parse_statement);
		
		if (node) array_add(&block->statements, node);
		
		if (lexer->peek_token().Type == '}') break;
		
	}
	
	return block;
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
				//logger->print("Token Name = [%s]\n"_s, token.name);
				Ast_Node* node = CallParseCommand(parse_statement);
				PRINT_GRAPH(node, logger);
				
				break;
			}
			case '{': 
			{
				//logger->print("Scope"_s);
				Ast_Node* scope = CallParseCommand(parse_scope);
				PRINT_GRAPH(scope, logger);
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


