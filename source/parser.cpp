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


#define CallParseCommand(command) command();

// How does this perform ?? 
#define AllocateNode(type, name) type* name;\
name = (type*) arena_alloc(&ast_arena, sizeof(type));\
name = ::new(name) type();

#define AllocateSymbol(type, name) type* name;\
name = (type*) arena_alloc(&ast_arena, sizeof(type));\
name = ::new(name) type();

#ifdef _DEBUG
#define PANIC() exit(0);//abort();
#else
#define PANIC() exit(-1);
#endif

//static Arena ast_arena = {0};
static Arena Symbol_Table = {0};
static u8 counter = 0;


inline Ast_Node*
Parser::parse_operator()
{
	Token token = lexer.peek_token();
	
	
	if (token.Type == '.'){
		lexer.eat_token();
		AllocateNode(Ast_MemberAccess, exp);
		exp->token = token;
		return exp;
	}
	
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
	lexer.eat_token();
	AllocateNode(Ast_Binary, exp);
	exp->token = token;
	exp->op = op;
	return exp;
	
}


#if 0
void 
print_simple_c_struct(Logger* logger, Ast_Struct* node) {
	
	logger.print("struct %s {\n"_s, node->ident->token.name);
	for(u64 i = 0; i < node->fields.occupied; i++){
		logger.print("\t%s %s; \n"_s,
					  node->fields[i]->data_type->token.name,
					  node->fields[i]->ident->token.name);
	}
	logger.print("};\n"_s, node->ident->token.name);
	
	
}
#endif


inline void 
Parser::expect_and_eat(u64 type)
{
	if(lexer.peek_token().Type != type)
	{
		//logger.parse
		// TODO loghere : 
		logger.print("expect_and_eat Broke \n"_s);
		logger.print_with_location(&lexer.peek_token(), "Expected [%c] got \"%s\"\n"_s, (u8) type,
									lexer.peek_token().name);
		//_CrtDbgBreak();
		assert(false);
	}
	lexer.eat_token();
}

Ast Parser::parse()
{
	int a = 0;
	auto token = lexer.peek_token();
	Ast ast = {0};
	
	// Parse All Needed files
	while(token.Type != TOKEN_EOFA){
		switch(token.Type)
		{
			case '{': 
			case TOKEN_IDENT:
			{
				Ast_Node* node = CallParseCommand(parse_statement);
				array_add(&ast.nodes, node);
				break;
			}
			case TOKEN_DIRECTIVE:
			{
				
				assert(false && "directives are not handled yet");
				break;
			}
			default:
			{
				// This should be an error eventually.. since we are not really handleing it
				//lexer.eat_token();
				break;
			}
			
		}
		token = lexer.peek_token();
	}	
	
	
	
	for(u64 a = 0; a < ast.nodes.occupied ; a++){
		PRINT_GRAPH(ast.nodes[a], &logger);
	}
	return {0};
}


Ast_Node*
Parser::parse_def()
{
	AllocateNode(Ast_Declaration, decl);
	Token ident = lexer.peek_token();
	if (ident.Type != TOKEN_IDENT) return nullptr;
	lexer.eat_token();
	
	decl->token = ident;
	
	// eat colon 
	if (lexer.peek_token().Type != TOKEN_COLON) return nullptr;
	lexer.eat_token();
	
	Token t = lexer.peek_token();
	
	decl->inforced_type = false;
	
	// Inforced Type 
	if (t.Type == TOKEN_IDENT || t.Type == TOKEN_HDTYPE) {
		AllocateNode(Ast_Ident, type);
		type->token = t;
		decl->inforced_type = true;
		decl->data_type = type;
		lexer.eat_token();
		t = lexer.peek_token();
	}
	
	
	// var : int ; 
	if (t.Type == ';') {
		if(!decl->inforced_type) assert(false); // TODO: Log error
		decl->def_type = AST_DEF_VAR;
		lexer.eat_token();
		goto returnDecl;
	}
	
	
	
	if (t.Type == TOKEN_COLON){ // Inferred type - Constant Value 
		// Struct - Procedure - Constant Variable - Enum - Flags(Not yet)
		lexer.eat_token();
		decl->constant = true;
	} else if (t.Type == TOKEN_ASSIGN){ // Inferred type - Mutable Value 
		lexer.eat_token();
		// var : =  asd 
		// Variable
		decl->constant = false;
		decl->def_type = AST_DEF_VAR;
		decl->body = CallParseCommand(parse_expression);
		// TODO: Rethink this part
		auto tok = lexer.peek_token();
		if(tok.Type == ';') lexer.eat_token();
		goto returnDecl;
	}else {
		//logger.print_with_location(&t, "Expected [;], got [%s]\n"_s, t.name); 
		//assert(false);
		goto returnDecl;
	}
	
	t = lexer.peek_token();
	
	if(decl->constant){
		if(isEqual(&t.name, &"struct"_s)){
			decl->def_type = AST_DEF_STRUCT;
			AllocateNode(Ast_List, decls);
			lexer.eat_token();
			decls->token = t;
			expect_and_eat('{');
			
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
			
			expect_and_eat('}');
			expect_and_eat(';');
			
			decl->body = decls;
			goto returnDecl;
		}
		//@TODO: Error messages.
		else if (isEqual(&t.name, &"proc"_s)){
			lexer.eat_token();
			decl->def_type = AST_DEF_PROC;
			AllocateNode(Ast_Block, block);
			
			//assert(false); // for now 
			// Parse header
			block->token = lexer.peek_token();
			expect_and_eat('(');
			
			// @TODO: Test this
			while(true){
				Ast_Node* cur = CallParseCommand(parse_def);
				if(cur == nullptr) break;// what happened here ? 
				array_add(&block->statements, cur);
				if (lexer.peek_token().Type == ',') lexer.eat_token();
				else break;
				
			}
			
			decl->params = block;
			expect_and_eat(')');
			
			t = lexer.peek_token();
			
			if (t.Type == TOKEN_ARROW){
				lexer.eat_token();
				t = lexer.peek_token();
				AllocateNode(Ast_Block, return_types);
				return_types->token = t;
				while(true){
					if (t.Type == TOKEN_HDTYPE || 
						t.Type == TOKEN_IDENT)
					{
						AllocateNode(Ast_Ident, ident);
						ident->token = t;
						array_add(&return_types->statements, (Ast_Node*)ident);
						lexer.eat_token();
					}
					else break;
					
					t = lexer.peek_token();
					if (t.Type == '{') break;
					else if (t.Type == ',') continue;
					// LOGHERE: 
					else assert(false && "asd");
				}
				
			} 
			
			// Parse directives
			
			//expect_and_eat('{');
			if(lexer.peek_token().Type != '{'){
				assert(false);
			}
			// Parse Body
			decl->body = CallParseCommand(parse_block_of_statments);
			goto returnDecl;
			
		}
		else if (isEqual(&t.name, &"enum"_s)){
			lexer.eat_token();
			decl->def_type = AST_DEF_ENUM;
			assert((false && "We DO NOT support enum"));
		}
		else if (isEqual(&t.name, &"flag"_s)){
			lexer.eat_token();
			decl->def_type = AST_DEF_FLAG;
			assert((false && "We DO NOT support flag"));
		}
		else {
			if (t.Type == TOKEN_DIRECTIVE && 
				(isEqual(&t.name, &"#type"_s)))
			{
				
				logger.print_with_location(&lexer.peek_token(), "Type Directive is not supported yet %s\n"_s, t.name);
				assert(false);
			}
			
			decl->body =  CallParseCommand(parse_expression);
			expect_and_eat(';');
			goto returnDecl;
		}
		
		
		
	}
	
	return nullptr;
	returnDecl:
	
	
	return decl;
}

Ast_Node*
Parser::parse_statement()
{
	Ast_Node* return_node = nullptr;
	
	
	Token t1 = lexer.peek_token();
	Token t2 = lexer.peek_token(1);
	
	if(t1.Type == 279 ) return nullptr;
	
	// parse blocks
	if (t1.Type == '{')
	{
		return_node = CallParseCommand(parse_block_of_statments);
		auto t = lexer.eat_token();
		if (t.Type != '}') 
		{
			logger.print_with_location(&t, "Expected [}] got [%s]\n"_s, t.name);
			assert(false);
		}
		
		return return_node ;
		
	}
	
	// Parse def
	if (t1.Type == TOKEN_IDENT && t2.Type == TOKEN_COLON)
	{
		return CallParseCommand(parse_def);
	}
	
	
	// parse_expression
	return_node = CallParseCommand(parse_expression);
	if(return_node->type == AST_FUNCALL) return return_node;
	
	// Assignment ?? 
	auto token = lexer.peek_token();
	expect_and_eat(TOKEN_ASSIGN);
	
	AllocateNode(Ast_Assign, assign);
	assign->token = token;
	assign->left = return_node;
	assign->right = CallParseCommand(parse_expression);
	return_node = assign;
	expect_and_eat(';');
	return return_node;
	
	assert(false);
	return nullptr;
	
}


Ast_Node*
Parser::parse_subexpression()
{
	Token t1 = lexer.peek_token();
	Token t2 = lexer.peek_token(1);
	
	switch(t1.Type)
	{
		case TOKEN_IDENT:
		{
			if(t2.Type == '(') // Procedure call
			{
				// eat the ident
				lexer.eat_token();
				// eat the '('
				lexer.eat_token();
				
				AllocateNode(Ast_FunctionCall, funcall);
				
				auto t = lexer.peek_token();
				funcall->token = t1;
				if (t.Type == ')') {
					lexer.eat_token();
					return funcall;
				}
				
				while(true)
				{
					Ast_Node* node = CallParseCommand(parse_expression);
					assert(node != nullptr);
					
					array_add(&funcall->arguments, node);
					
					t = lexer.peek_token();
					
					if      (t.Type == ',') lexer.eat_token();
					else if (t.Type == ')') { lexer.eat_token(); return funcall;}
					else    assert(false);
				}
				
			}
			else if (t2.Type == '[') // Subscript 
			{
				// eat the ident
				lexer.eat_token();
				// eat the '['
				lexer.eat_token();
				
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
				expect_and_eat(']');
				return subscript;
				
			}
			else {
				lexer.eat_token();
				AllocateNode(Ast_Ident, ident);
				ident->token = t1;
				return ident;
			}
			assert(false);
			break;
		}
		case '!':
		case '-':
		{
			AllocateNode(Ast_Unary, unary);
			//unary->op = token.Type;
			unary->token = lexer.eat_token();
			unary->child = CallParseCommand(parse_subexpression);
			return unary;
		}
		case TOKEN_LITERAL: 
		{
			lexer.eat_token();
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
			logger.print_with_location(&t1, "[parse_subexpression] Unexpected Token [%s]\n"_s, t1.name);
			assert(false);
			break;
		}
		
	}
	
	return nullptr;
}

Ast_Node*
Parser::parse_factor()
{
	// eat the '('
	lexer.eat_token();
	
	Ast_Node* exp = CallParseCommand(parse_expression);
	Token token = lexer.peek_token();
	
	if (!exp){
		logger.print_with_location(&token, "[parse_factor] Expected colsing bracket here ')' got [%s] instead\n"_s, token.name);
		assert(false);// TODO: Error Message
	}
	
	expect_and_eat(')');
	
	if(exp->type == AST_BINARY_EXP){
		auto node = (Ast_Binary*) exp;
		node->isFactor = true;
	}
	
	return exp;
}

Ast_Node*
Parser::parse_expression()
{
	Ast_Node* left = parse_subexpression();
	if (!left) return nullptr;
	
	Ast_Node* op = parse_operator();
	if (!op) return left;
	
	if(op->type == AST_MEBMER_ACCESS)
	{
		auto exp = (Ast_MemberAccess*) op;
		exp->left = left;
		exp->right= parse_expression();
		return exp;
	}
	
	Ast_Node* right = parse_expression();
	if (!right) return nullptr;
	
	
	if(op->type == AST_BINARY_EXP)
	{
		auto exp  = (Ast_Binary*)op;
		exp->left = left;
		if (right->type == AST_BINARY_EXP)
		{
			Ast_Binary* rightSide = (Ast_Binary*)right;
			if (rightSide->isFactor == true) goto label; 
			if(rightSide->op < exp->op)
			{
				exp->right = rightSide->left;
				rightSide->left = exp;
				return rightSide;
			}
			
		}
		label:
		exp->right = right;
		return exp;
	}
	
	
	
	assert(false);// we should never come here
	return nullptr;
}


Ast_Node*
Parser::parse_block_of_statments()
{
	Token t = lexer.peek_token(); // it must be "{"
	expect_and_eat('{');
	AllocateNode(Ast_Block, block);
	block->token = t;
	while(true){
		if(t.Type == '}') {
			lexer.eat_token();
			break;
		}
		Ast_Node* node =  CallParseCommand(parse_statement);
		if(!node) break;
		
		array_add(&block->statements, node);
		t = lexer.peek_token();
	}
	
	return block;
}


