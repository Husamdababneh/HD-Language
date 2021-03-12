/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "pch.h"
#include "lex.h"
#include "Ast.h"
#include "hash_table.h"
#include "parser.h"


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
static Ast_Node* parse_statement(LexerState* lexer, Logger* logger, bool need_semi_colon = true);
static Ast_Node* PARSE_COMMAND(parse_expression);
static Ast_Node* PARSE_COMMAND(parse_factor);
static Ast_Node* PARSE_COMMAND(parse_scope);
static Ast_Node* PARSE_COMMAND(parse_statment_v2);
static Ast_Node* PARSE_COMMAND(parse_def);
static Ast_Node* PARSE_COMMAND(parse_block_of_statments);

static HashTable<Ast_Node*>  symbols; 

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
			op = 0;
			break;
		}
		case '*':
		case '/':
		{
			op = 1;
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
				Ast_Node* node = CallParseCommand(parse_statment_v2);
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
	
	delete ast_arena.data;
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
		//auto tok = lexer->eat_token();
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
			
			if (lexer->eat_token().Type != '{') assert(false);
			
			
			// TODO: Test this
			while(true){
				Ast_Declaration* cur = (Ast_Declaration*) CallParseCommand(parse_def);
				if(cur == nullptr) 
				{
					//logger->print_with_location(&t, "Expected [;], got [%s]\n"_s, t.name); 
					break;
				}
				
				auto tok = lexer->eat_token();
				if (tok.Type != ';') {
					logger->print_with_location(&tok, "Expected [;], got [%s]\n"_s, tok.name); 
					assert(false);
				}
				// @CleanUp: This will eventually be removed ... i think ?? ... structs inside strcuts ?? 
				if(cur->def_type != AST_DEF_VAR) assert(false);
				array_add(&decls->list, cur);
			}
			
			t = lexer->eat_token();
			if (t.Type != '}') {
				logger->print_with_location(&t, "Expected [}], got [%s]\n"_s, t.name); 
				assert(false);
			}
			
			decl->body = decls;
			return decl;
			// parse defs only block 
			// def->body = CallParseCommand(parse_def_block);
		}
		//@TODO: Error messages.
		else if (isEqual(&t.name, &"proc"_s)){
			decl->def_type = AST_DEF_PROC;
			AllocateNode(Ast_Block, block);
			
			//assert(false); // for now 
			// Parse header
			if(lexer->eat_token().Type != '(') assert(false);
			
			// @TODO: Test this
			while(true){
				Ast_Node* cur = CallParseCommand(parse_def);
				if(cur == nullptr) break;
				array_add(&block->statements, cur);
				// @Incomplete: do we need to check more things to give a presice error message??   
				
				auto tok = lexer->peek_token();
				if      (tok.Type == ')') break;
				else if (tok.Type == ',') lexer->eat_token();
				else {
					logger->print_with_location(&tok, "Expected [nothing], got [%s]\n"_s, tok.name); 
					assert(false);
				}
				
			}
			
			decl->params = block;
			if(lexer->eat_token().Type != ')') assert(false);
			
			t = lexer->peek_token();
			
			if (t.Type == TOKEN_ARROW){
				lexer->eat_token();
				// parse type 
				// parse return types
				t = lexer->peek_token();
			}
			
			// Parse Body
			if (t.Type == '{') {
				//  type is assigned assign void as default procedure return type 
				decl->body = CallParseCommand(parse_block_of_statments);
				return decl;
				
			}else{
				assert(false);
			}
			
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
PARSE_COMMAND(parse_statment_v2)
{
	Ast_Node* return_node = nullptr;
	
	
	Token t2 = lexer->peek_token(1);
	if (t2.Type == TOKEN_COLON) {
		Ast_Declaration* node = (Ast_Declaration*)CallParseCommand(parse_def);
		return_node = node;
	}
#if 0
	else {
		logger->print("Token Type = %s\n"_s, t2.name);
		assert(false);
	}
#endif
	
	
	return return_node;
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
				
				if (lexer->peek_token().Type != ']') assert(false);
				lexer->eat_token();
				
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
		Ast_Node* node =  CallParseCommand(parse_statment_v2);
		
		array_add(&block->statements, node);
		t = lexer->eat_token();
		if (t.Type != ';') assert(false);
		t = lexer->peek_token();
	}
	
	return block;
}







