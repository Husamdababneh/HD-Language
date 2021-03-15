/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "pch.h"
#include "lex.h"
#include "Ast.h"
#include "parser.h"


// @TEMP @CLEANUP This may break the premake generated files
#define ENABLE_GRAPH_PRINTING 1
#include <extra/graph.cpp>
//

#include "../submodules/tracy/Tracy.hpp"

#define PARSE_COMMAND(name) name(LexerState* lexer, Logger* logger)
#define CallParseCommand(command) command(lexer, logger);

#define AllocateNode(type, name) type* name;\
name = (type*) arena_alloc(&ast_arena, sizeof(type));\
name = ::new(name) type();

#ifdef _DEBUG
#define PANIC() exit(0);//abort();
#else
#define PANIC() exit(-1);
#endif

static Arena ast_arena = {0};
static u8 counter = 0;

static Ast_Node* PARSE_COMMAND(parse_expression);
static Ast_Node* PARSE_COMMAND(parse_factor);
static Ast_Node* PARSE_COMMAND(parse_scope);
static Ast_Node* PARSE_COMMAND(parse_statement);
static Ast_Node* PARSE_COMMAND(parse_def);
static Ast_Node* PARSE_COMMAND(parse_block_of_statments);


inline Ast_Binary*
PARSE_COMMAND(parse_operator)
{
	Token token = lexer->peek_token();
	
	
	u32 op = 0; 
	switch(token.Type)
	{
		case TOKEN_SHIFT_LEFT:
		case TOKEN_SHIFT_RIGHT:
		{
			op = 8;
			break;
		}
		case '+':
		case '-':
		{
			op = 9;
			break;
		}
		case '*':
		case '/':
		{
			op = 10;
			break;
		}
		default: 
		{
			return nullptr;
		}
	}
	lexer->eat_token();
	AllocateNode(Ast_Binary, exp);
	exp->token = token;
	exp->op = op;
	return exp;
	
}


#if 0
void 
print_simple_c_struct(Logger* logger, Ast_Struct* node) {
	
	logger->print("struct %s {\n"_s, node->ident->token.name);
	for(u64 i = 0; i < node->fields.occupied; i++){
		logger->print("\t%s %s; \n"_s,
					  node->fields[i]->data_type->token.name,
					  node->fields[i]->ident->token.name);
	}
	logger->print("};\n"_s, node->ident->token.name);
	
	
}
#endif


static inline void 
expect_token(LexerState* lexer, Logger* logger, u64 type)
{
	if(lexer->peek_token().Type != type)
	{
		//logger->parse
		// TODO loghere : 
		logger->print("expect_token Broke \n"_s);
		logger->print_with_location(&lexer->peek_token(), "Expected [%c] got \"%s\"\n"_s, (u8) type,
									lexer->peek_token().name);
		//_CrtDbgBreak();
		assert(false);
	}
	lexer->eat_token();
}

static Ast 
PARSE_COMMAND(parse)
{
#if 0
	
	Ast_Node* node = CallParseCommand(parse_expression);
	output_graph(node, logger);
	output_labels(logger);
#else
	
	int a = 0;
	auto token = lexer->peek_token();
	while(token.Type != TOKEN_EOFA){
		switch(token.Type)
		{
			case TOKEN_IDENT:
			{
				//logger->print("Token Name = [%s]\n"_s, token.name);
				Ast_Node* node = CallParseCommand(parse_statement);
				if(node){
					output_graph(node, logger);
					output_labels(logger);
					//if(node->def_type == AST_DEF_STRUCT) printf("Number of decls = %I64i\n", ((Ast_List*)node->body)->list.occupied);
				}
				break;
			}
			case '{': 
			{
				//logger->print("Scope"_s);
				//Ast_Node* scope = CallParseCommand(parse_scope);
				//PRINT_GRAPH(scope, logger);
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
#endif
	return {0};
}

void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);
	
	parse(&lexer, &logger);
	
	arena_free(&ast_arena);
}

static Ast_Node*
PARSE_COMMAND(parse_def)
{
	AllocateNode(Ast_Declaration, decl);
	Token ident = lexer->peek_token();
	if (ident.Type != TOKEN_IDENT) return nullptr;
	lexer->eat_token();
	
	decl->token = ident;
	
	// eat colon 
	if (lexer->peek_token().Type != TOKEN_COLON) return nullptr;
	lexer->eat_token();
	
	Token t = lexer->peek_token();
	
	decl->inforced_type = false;
	
	// Inforced Type 
	if (t.Type == TOKEN_IDENT || t.Type == TOKEN_HDTYPE) {
		AllocateNode(Ast_Ident, type);
		type->token = t;
		decl->inforced_type = true;
		decl->data_type = type;
		lexer->eat_token();
		t = lexer->peek_token();
	}
	
	
	// var : int ; 
	if (t.Type == ';') {
		if(!decl->inforced_type) assert(false); // TODO: Log error
		decl->def_type = AST_DEF_VAR;
		lexer->eat_token();
		return decl;
	}
	
	
	
	if (t.Type == TOKEN_COLON){ // Inferred type - Constant Value 
		// Struct - Procedure - Constant Variable - Enum - Flags(Not yet)
		lexer->eat_token();
		decl->constant = true;
	} else if (t.Type == TOKEN_ASSIGN){ // Inferred type - Mutable Value 
		lexer->eat_token();
		// var : =  asd 
		// Variable
		decl->constant = false;
		decl->def_type = AST_DEF_VAR;
		decl->body = CallParseCommand(parse_expression);
		auto tok = lexer->peek_token();
		if(tok.Type == ';') lexer->eat_token();
		return decl;
	}else {
		//logger->print_with_location(&t, "Expected [;], got [%s]\n"_s, t.name); 
		//assert(false);
		return decl;
	}
	
	t = lexer->eat_token();
	
	if(decl->constant){
		if(isEqual(&t.name, &"struct"_s)){
			decl->def_type = AST_DEF_STRUCT;
			AllocateNode(Ast_List, decls);
			
			expect_token(lexer,logger, '{');
			
			// TODO: Test this
			while(true){
				Ast_Declaration* cur = (Ast_Declaration*) CallParseCommand(parse_def);
				if(cur == nullptr) 
				{
					// What happend here???
					break;
				}
				
				// @CleanUp: This will eventually be removed ... i think ?? ... structs inside strcuts ?? 
				if(cur->def_type != AST_DEF_VAR) assert(false);
				array_add(&decls->list, cur);
			}
			
			expect_token(lexer,logger, '}');
			expect_token(lexer,logger, ';');
			
			decl->body = decls;
			return decl;
		}
		//@TODO: Error messages.
		else if (isEqual(&t.name, &"proc"_s)){
			decl->def_type = AST_DEF_PROC;
			AllocateNode(Ast_Block, block);
			
			//assert(false); // for now 
			// Parse header
			expect_token(lexer,logger, '(');
			
			// @TODO: Test this
			while(true){
				Ast_Node* cur = CallParseCommand(parse_def);
				if(cur == nullptr) break;// what happened here ? 
				array_add(&block->statements, cur);
				if (lexer->peek_token().Type == ',') lexer->eat_token();
				else break;
				
			}
			
			decl->params = block;
			expect_token(lexer,logger, ')');
			
			t = lexer->peek_token();
			
			if (t.Type == TOKEN_ARROW){
				lexer->eat_token();
				t = lexer->peek_token();
				AllocateNode(Ast_Block, return_types);
				while(true){
					if (t.Type == TOKEN_HDTYPE || 
						t.Type == TOKEN_IDENT)
					{
						AllocateNode(Ast_Ident, ident);
						ident->token = t;
						array_add(&return_types->statements, (Ast_Node*)ident);
						lexer->eat_token();
					}
					else break;
					
					t = lexer->peek_token();
					if (t.Type == '{') break;
					else if (t.Type == ',') continue;
					// LOGHERE: 
					else assert(false);
				}
				
			} 
			
			// Parse directives
			
			expect_token(lexer,logger, '{');
			// Parse Body
			decl->body = CallParseCommand(parse_block_of_statments);
			return decl;
			
		}
		else if (isEqual(&t.name, &"enum"_s)){
			decl->def_type = AST_DEF_ENUM;
			assert((false && "We DO NOT support enum"));
		}
		else if (isEqual(&t.name, &"flag"_s)){
			decl->def_type = AST_DEF_FLAG;
			assert((false && "We DO NOT support flag"));
		}
		else {
			if (t.Type == TOKEN_DIRECTIVE && 
				(isEqual(&t.name, &"#type"_s)))
			{
				
				logger->print_with_location(&lexer->peek_token(), "Type Directive is not supported yet %s\n"_s, t.name);
				assert(false);
			}
			/// What type is this ??
			logger->print_with_location(&lexer->peek_token(), "What ? %s\n"_s, t.name);
			assert(false);
			
		}
		
		// Parse Expression
		//decl->
		
	}
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_statement)
{
	Ast_Node* return_node = nullptr;
	
	
	Token t1 = lexer->peek_token();
	Token t2 = lexer->peek_token(1);
	
	// parse blocks
	if (t1.Type == '{')
	{
		return_node = CallParseCommand(parse_block_of_statments);
		auto t = lexer->eat_token();
		if (t.Type != '}') assert(false);
		
		return return_node ;
		
	}
	
	// Parse def
	if (t1.Type == TOKEN_IDENT && 
		t2.Type == TOKEN_COLON) {
		return_node = CallParseCommand(parse_def);
		return return_node ;
	}
	
	
	// other statment types ... 
	//logger->print_with_location(&t1, "Unexpected Tokens \n\t1: [%s]\n\t2: [%s]\n"_s, t1.name, t2.name);
	//assert(false);
	return nullptr;
}


static Ast_Node*
PARSE_COMMAND(parse_subexpression)
{
	Token t1 = lexer->peek_token();
	Token t2 = lexer->peek_token(1);
	
	switch(t1.Type)
	{
		case TOKEN_IDENT:
		{
			if(t2.Type == '(') // Procedure call
			{
				// eat the ident
				lexer->eat_token();
				// eat the '('
				lexer->eat_token();
				
				AllocateNode(Ast_FunctionCall, funcall);
				
				auto t = lexer->peek_token();
				funcall->token = t1;
				if (t.Type == ')') {
					lexer->eat_token();
					return funcall;
				}
				
				while(true)
				{
					Ast_Node* node = CallParseCommand(parse_expression);
					assert(node != nullptr);
					
					array_add(&funcall->arguments, node);
					
					t = lexer->peek_token();
					
					if      (t.Type == ',') lexer->eat_token();
					else if (t.Type == ')') { lexer->eat_token(); return funcall;}
					else    assert(false);
				}
				
			}
			else if (t2.Type == '[') // Subscript 
			{
				// eat the ident
				lexer->eat_token();
				// eat the '['
				lexer->eat_token();
				
				AllocateNode(Ast_Subscript, subscript);
				subscript->token = t1;
				Ast_Node* node = CallParseCommand(parse_expression);
				// TODO: Report Here
				assert(node != nullptr);
				if(node->type == AST_IDENT || 
				   node->type == AST_BINARY_EXP ||
				   node->type == AST_UNARY_EXP ||
				   node->type == AST_LITERAL)
				{
					subscript->exp = node;
				}
				else
				{
					assert(false);
				}
				expect_token(lexer, logger, ']');
				return subscript;
				
			}
			else {
				lexer->eat_token();
				AllocateNode(Ast_Ident, ident);
				ident->token = t1;
				return ident;
			}
			
			//break;
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
		case TOKEN_LITERAL: 
		{
			lexer->eat_token();
			AllocateNode(Ast_Literal, literal);
			literal->token = t1;
			return literal;
		}
		case ')':
		case ']':
		case ',':
		case ';':
		return nullptr;
		case '(':
		{
			Ast_Node* factor = CallParseCommand(parse_factor);
			return factor;
			
		}
		default:
		{
			logger->print_with_location(&t1, "[parse_subexpression] Unexpected Token [%s]\n"_s, t1.name);
			assert(false);
			break;
		}
		
	}
	
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_factor)
{
	// eat the '('
	lexer->eat_token();
	
	Ast_Node* exp = CallParseCommand(parse_expression);
	if (!exp) assert(false);
	Token token = lexer->peek_token();
	if (token.Type != ')') {
		
		// TODO: Make this  better
		logger->print_with_location(&token, "[parse_factor] Expected colsing bracket here ')' instead got [%s] instead \n"_s, token.name);
		assert(false);
	}
	
	// eat ')'
	lexer->eat_token();
	return exp;
}

static Ast_Node*
PARSE_COMMAND(parse_expression)
{
	Ast_Node* left = parse_subexpression(lexer, logger);
	if (!left) return nullptr;
	
	Ast_Binary* exp = parse_operator(lexer, logger);
	if (!exp) return left;
	
	Ast_Node* right = parse_expression(lexer, logger);
	if (!right) return nullptr;
	
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
parse_comma_seperated(LexerState* lexer, Logger* logger, Ast_Node* (func) (LexerState* , Logger*))
{
	
	while(true){
		break;
	}
	
	func(lexer, logger);
	return nullptr;
}

static Ast_Node*
PARSE_COMMAND(parse_block_of_statments)
{
	u8 scope_count = 0;
	Token t = lexer->eat_token(); // it must be "{"
	t = lexer->peek_token();
	AllocateNode(Ast_Block, block);
	
	while(true){
		if(t.Type == '}') {
			lexer->eat_token();
			break;
		}
		Ast_Node* node =  CallParseCommand(parse_statement);
		if(!node) break;
		
		array_add(&block->statements, node);
		t = lexer->peek_token();
	}
	
	return block;
}







