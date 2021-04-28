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


#include "typer.h"

// @TEMP @CLEANUP This may break the premake generated files
#define ENABLE_GRAPH_PRINTING 1
#define GRAPH_SHOW_BLOCK_PARENT 0
#include <extra/graph.cpp>

#include "../submodules/tracy/Tracy.hpp"


// :nocheckin
#include "generator.cpp"

#define cmpsv(x, y) memcmp(x, y, strlen(x))
#define SV_PRINT(x) (int)x.count, x.data
// How does this perform ?? 

Ast_Node** flaten_ast = NULL;

#define AllocateNode(type, name) type* name;\
name = (type*) arena_alloc(&ast_arena, sizeof(type));\
name = ::new(name) type();\
arrput(flaten_ast,(Ast_Node*)name);

#ifdef _DEBUG
#define PANIC() exit(0);//abort();
#else
#define PANIC() exit(-1);
#endif

inline Ast_Node*
Parser::parse_operator()
{
	Token token = lexer.peek_token();
	
	
	if (token.type == '.'){
		lexer.eat_token();
		AllocateNode(Ast_MemberAccess, exp);
		exp->token = token;
		return exp;
	}
	
	u32 op = 0;
	switch(token.type)
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
print_simple_c_struct(Logger* logger, Ast_Struct* node) 
{
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
	if(lexer.peek_token().type != type)
	{
		//logger.parse
		// TODO loghere : 
		logger.print("expect_and_eat:\n"_s);
		logger.print_with_location(&lexer.peek_token(), "Expected [%c] got \"%s\"\n"_s, (u8) type,
								   lexer.peek_token().name);
		//_CrtDbgBreak();
		assert(false);
	}
	lexer.eat_token();
}

inline void 
Parser::register_scope(const StringView& name)
{
	u64 id = arrlenu(scopes);
	Scope scope = {id,  name};
	arrput(scopes, scope);
}

inline Scope 
Parser::get_current_scope()
{
	// TODO: Add checks here
	return scopes[arrlen(scopes) - 1];
}

inline void 
Parser::exit_scope()
{
	arrdel(scopes, arrlen(scopes) - 1);
}


Ast Parser::parse()
{
	int a = 0;
	auto token = lexer.peek_token();
	Ast ast = {0};
	
	register_scope("GLOBAL"_s);
	// Parse All Needed files
	while(token.type != TOKEN_EOFA){
		switch(token.type)
		{
			case '{': 
			case TOKEN_IDENT:
			{
				Ast_Node* node = parse_statement();
				arrput(ast.nodes, node);
				break;
			}
			case TOKEN_DIRECTIVE:
			{
				
				assert(false && "directives are not handled yet");
				break;
			}
			default:
			{
				assert(false && "Unexpected Token");
				break;
			}
			
		}
		
		token = lexer.peek_token();
	}	
	
	
	/* 	
		for (int i=0; i < hmlen(symbolTable); ++i)
			logger.print("%s %s\n"_s, symbolTable[i].value.node->token.name, symbolTable[i].value.scope.name );
		 */
	
	
	exit_scope();
	
	/* 	
		for (int i=0; i < arrlen(scopes); ++i){
			auto scope = scopes[i];
			logger.print("Scope [%s], Id: [%d]\n"_s, scope.name, scope.id);
		}
		 */
	
	for(u64 i = 0; i < arrlenu(flaten_ast); i++){
#if 0
		if (flaten_ast[i]->type == AST_DECLARATION) {
			Ast_Declaration* decl = (Ast_Declaration*)flaten_ast[i];
			if (decl->kind == AST_DEF_STRUCT){
				if (decl->data_type != nullptr) 
				{
					StringView type_name = decl->data_type->token.name;
					u8 name[256] = {0};
					memcpy(name, type_name.str, type_name.count + 1);
					name[type_name.count] = 0;
					type type = shget(types, name);
					if (type.size == 0){
						printf("Size:%2d, Name[%.*s], Aliases[], children[]\n", 
							   (int)type.size,
							   SV_PRINT(type_name));
					}
				}
			}
			
			/* 
						if (decl->kind == AST_DEF_PROC) {
							generate_proc(decl);
						}
			 */
		}
#endif
		
		if (flaten_ast[i]->type == AST_BINARY_EXP) {
			Ast_Binary* binary = (Ast_Binary*)flaten_ast[i];
			printf("Binary [%.*s]\n", SV_PRINT(binary->token.name));
			//generate_binary_expression(binary);
		}
	}
	
	/* 
		for(u64 a = 0; a < arrlenu(ast.nodes); a++){
			Ast_Node* node = ast.nodes[a];
			
			if (node->type == AST_DECLARATION){
				Ast_Declaration* decl = (Ast_Declaration*)ast.nodes[a];
				if (decl->data_type != nullptr){
					
					StringView type_name = decl->data_type->token.name;
					u8 name[256] = {0};
					memcpy(name, type_name.str, type_name.count + 1);
					name[type_name.count] = 0;
					type type = shget(types, name);
					printf("Size:%2d, Name[%s], Aliases[], children[]\n", 
						   (int)type.size,
						   type.name);
					
					printf("type [%.*s]\n", SV_PRINT(type_name));
				}
			}
			//PRINT_GRAPH(ast.nodes[a], &logger);
		}
		 */
	
	
	return {0};
}



Ast_Node* 
Parser::parse_proc_def()
{
	
	
	return  nullptr;
}

Ast_Node*
Parser::parse_def(bool semicolon /* = true */)
{
	AllocateNode(Ast_Declaration, decl);
	Token ident = lexer.peek_token();
	if (ident.type != TOKEN_IDENT) return nullptr;
	lexer.eat_token();
	
	decl->token = ident;
	
	// eat colon 
	if (lexer.peek_token().type != TOKEN_COLON) return nullptr;
	lexer.eat_token();
	
	Token t = lexer.peek_token();
	
	decl->inforced_type = false;
	
	// Inforced type 
	if (t.type == TOKEN_IDENT || t.type == TOKEN_HDTYPE) {
		AllocateNode(Ast_Ident, type);
		type->token = t;
		decl->inforced_type = true;
		decl->data_type = type;
		lexer.eat_token();
		t = lexer.peek_token();
	}
	
	
	// var : int ; 
	if (t.type == ';') {
		if(!decl->inforced_type) assert(false); // TODO: Log error
		decl->kind = AST_DEF_VAR;
		lexer.eat_token();
		goto returnDecl;
	}
	
	
	
	if (t.type == TOKEN_COLON){ // Inferred type - Constant Value 
		// Struct - Procedure - Constant Variable - Enum(Not yet) - Flags(Not yet)
		lexer.eat_token();
		decl->constant = true;
	} else if (t.type == TOKEN_ASSIGN){ // Inferred type - Mutable Value 
		lexer.eat_token();
		// var : =  asd 
		decl->constant = false;
		//  Normal variable with type, 
		decl->kind = AST_DEF_VAR;
		
		decl->body = parse_expression();
		// TODO: Rethink this part
		auto tok = lexer.peek_token();
		if (semicolon)
		{
			if (tok.type != ';') {
				logger.print_with_location(&tok, "Expected [;] got [%s]\n"_s, tok.name);
				//exit(-1);
				assert(false);
			}
			lexer.eat_token();
		}
		
		goto returnDecl;
	}else {
		//logger.print_with_location(&t, "Expected [;], got [%s]\n"_s, t.name); 
		//assert(false);
		goto returnDecl;
	}
	
	t = lexer.peek_token();
	
	if(decl->constant) {
		if(cmpsv("struct", t.name.data) == 0){
			decl->kind = AST_DEF_STRUCT;
			AllocateNode(Ast_List, decls);
			lexer.eat_token();
			decls->token = t;
			expect_and_eat('{');
			
			register_scope(decl->token.name);
			// TODO: Test this
			while(true){
				Ast_Declaration* cur = (Ast_Declaration*) parse_def(true);
				if(cur == nullptr) 
				{
					// What happend here???
					break;
				}
				
				// @CleanUp: This will eventually be removed ... i think ?? ... structs inside strcuts ?? 
				if(cur->kind != AST_DEF_VAR) assert(false);
				arrput(decls->list, cur);
			}
			
			expect_and_eat('}');
			exit_scope();
			decl->body = decls;
			goto returnDecl;
		}
		else if (cmpsv("proc", t.name.data) == 0){
			//@TODO: Error messages.
			lexer.eat_token();
			decl->kind = AST_DEF_PROC;
			AllocateNode(Ast_Block, block);
			
			register_scope(decl->token.name);
			register_scope("Header"_s);
			
			// Parse header
			block->token = lexer.peek_token();
			expect_and_eat('(');
			
			// @TODO: Test this
			while(true){
				Ast_Node* cur = parse_def(false);
				if(cur == nullptr) break;// what happened here ? 
				arrput(block->statements, cur);
				if (lexer.peek_token().type == ',') lexer.eat_token();
				else break;
				
			}
			
			decl->params = block;
			expect_and_eat(')');
			
			t = lexer.peek_token();
			
			if (t.type == TOKEN_ARROW){
				lexer.eat_token();
				t = lexer.peek_token();
				AllocateNode(Ast_Block, return_types);
				return_types->token = t;
				while(true){
					if (t.type == TOKEN_HDTYPE || 
						t.type == TOKEN_IDENT)
					{
						AllocateNode(Ast_Ident, ident);
						ident->token = t;
						arrput(return_types->statements, (Ast_Node*)ident);
						lexer.eat_token();
					}
					else break;
					
					t = lexer.peek_token();
					if (t.type == '{') break;
					else if (t.type == ',') continue;
					// LOGHERE: 
					else assert(false && "asd");
				}
				
			} 
			exit_scope();// header
			// Parse directives
			
			//expect_and_eat('{');
			if(lexer.peek_token().type != '{'){
				assert(false);
			}
			// Parse Body
			decl->body = parse_block_of_statments();
			((Ast_Block*)decl->body)->enclosing_scope = decl;
			exit_scope(); // func
			
			// If annotations are present, expect ";"
			goto returnDecl;
			
		}
		else if (cmpsv("enum",t.name.data) == 0){
			lexer.eat_token();
			decl->kind = AST_DEF_ENUM;
			assert((false && "We DO NOT support enum"));
		}
		else if (cmpsv("flag",t.name.data) == 0){
			lexer.eat_token();
			decl->kind = AST_DEF_FLAG;
			assert((false && "We DO NOT support flag"));
		}
		else {
			if (t.type == TOKEN_DIRECTIVE && 
				(cmpsv("#type",t.name.data) == 0))
			{
				
				logger.print_with_location(&lexer.peek_token(), "type Directive is not supported yet %s\n"_s, t.name);
				assert(false);
			}
			
			decl->body =  parse_expression();
			expect_and_eat(';');
			goto returnDecl;
		}
		
		
		
	}
	
	return nullptr;
	returnDecl:
	MySymbol symbol = {decl, get_current_scope()};
	hmput(symbolTable, ident.name, symbol);
	
	return decl;
}

Ast_Node*
Parser::parse_statement()
{
	Ast_Node* return_node = nullptr;
	
	
	Token t1 = lexer.peek_token();
	Token t2 = lexer.peek_token(1);
	// ?? What is this 
	if(t1.type == 279 ) return nullptr;
	
	// TODO : Handle compile-time directives 
	
	// TODO : add names scopes 
	// parse scope
	if (t1.type == '{')
	{
		return_node = parse_block_of_statments();
		auto t = lexer.eat_token();
		if (t.type != '}') 
		{
			logger.print_with_location(&t, "Expected [}] got [%s]\n"_s, t.name);
			assert(false);
		}
		
		return return_node ;
		
	}
	
	// Parse def
	if (t1.type == TOKEN_IDENT && t2.type == TOKEN_COLON)
	{
		return parse_def();
	}
	
	
	// parse_expression
	return_node = parse_expression();
	if(return_node->type == AST_FUNCALL) return return_node;
	
	// Assignment of global variables ??
	auto token = t1;
	expect_and_eat(TOKEN_ASSIGN);
	
	AllocateNode(Ast_Assign, assign);
	assign->token = token;
	assign->left = return_node;
	assign->right = parse_expression();
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
	
	switch(t1.type)
	{
		case TOKEN_IDENT:
		{
			if(t2.type == '(') // Procedure call
			{
				// eat the ident
				lexer.eat_token();
				// eat the '('
				lexer.eat_token();
				
				AllocateNode(Ast_FunctionCall, funcall);
				
				auto t = lexer.peek_token();
				funcall->token = t1;
				if (t.type == ')') {
					lexer.eat_token();
					return funcall;
				}
				
				while(true)
				{
					Ast_Node* node = parse_expression();
					assert(node != nullptr);
					
					arrput(funcall->arguments, node);
					
					t = lexer.peek_token();
					
					if      (t.type == ',') lexer.eat_token();
					else if (t.type == ')') { lexer.eat_token(); return funcall;}
					else    assert(false);
				}
				
			}
			else if (t2.type == '[') // Subscript 
			{
				// eat the ident
				lexer.eat_token();
				// eat the '['
				lexer.eat_token();
				
				AllocateNode(Ast_Subscript, subscript);
				subscript->token = t1;
				Ast_Node* node = parse_expression();
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
			//unary->op = token.type;
			unary->token = lexer.eat_token();
			unary->child = parse_subexpression();
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
			Ast_Node* factor = parse_factor();
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
	
	Ast_Node* exp = parse_expression();
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
	Token t = lexer.peek_token();
	expect_and_eat('{');
	AllocateNode(Ast_Block, block);
	
	// TODO : 
	auto scope = get_current_scope();
	register_scope(scope.name);
	block->token = t;
	
	// TODO : Rethink this part ?? 
	while(true){
		if(t.type == '}' || t.type == 279) {
			exit_scope();
			lexer.eat_token();
			break;
		}
		Ast_Node* node =  parse_statement();
		if(!node) {
			logger.print_with_location(&t, "Token [%s] type[%d]"_s, t.name, t.type);
			assert(false); // assert here
		}
		
		if (node->type == AST_BLOCK) ((Ast_Block *)node)->enclosing_scope = block;
		
		arrput(block->statements, node);
		t = lexer.peek_token();
	}
	
	return block;
}


