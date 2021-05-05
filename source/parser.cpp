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



Ast_Node** flaten_ast = NULL;

// How does this perform ?? 
#define AllocateNode(type, name) type* name;\
name = (type*) arena_alloc(&ast_arena, sizeof(type));\
name = ::new(name) type();\
arrput(flaten_ast,(Ast_Node*)name);

#ifdef _DEBUG
#define PANIC() exit(0);//abort();
#else
#define PANIC() exit(-1);
#endif


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


// TODO: make this a marco 
/* 
inline void 
Parser::expect_and_eat(u64 type )
{
	if(lexer.peek_token().type != type)
	{
		//logger.parse
		// TODO loghere : 
		printf("expect_and_eat:\n");
		printf("Expected [%c, %d] got \"%.*s\"\n", (int)type, (int)type, SV_PRINT(lexer.peek_token().name));
		//_CrtDbgBreak();
		assert(false);
	}
	lexer.eat_token();
}
*/

//_CrtDbgBreak();
#define expect_and_eat(x){\
if(lexer.peek_token().type != x)\
{\
printf("expect_and_eat: %s %d\n", __FILE__ ,__LINE__);\
printf("Expected [%c, %d] got \"%.*s\"\n", (int)x, (int)x, SV_PRINT(lexer.peek_token().name));\
exit(-1);\
}\
lexer.eat_token();\
}


inline Ast_Scope*
Parser::enter_scope()
{
	Ast_Scope* prev = current_scope;
	Ast_Scope _scope = {0}; 
	arrput(scopes, _scope);
	Ast_Scope* scope = &arrlast(scopes);
	
	
	if (prev != nullptr){
		arrput(prev->children, *scope);
	}
	
	scope->parent = prev;
	current_scope = &arrlast(scopes);
	return scope;
}


inline void
Parser::exit_scope()
{
	assert(current_scope);
	current_scope->parent;
}

Predefined_Type* Parser::predefined_types = nullptr;


#define Hash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
// TODO: Remove this
Token create_token(StringView name)
{
	Token token = {0};
	token.name = name;
	token.hash = Hash(token);
	
	return token;
}

void 
Parser::register_predefined_types()
{
	Ast_Type _u8;
	_u8.size = 1;
	_u8.alignment = 1;
	_u8.is_signed = false;
	_u8.token = create_token("u8"_s);
	hmput(predefined_types, "u8", _u8);
	
	Ast_Type _u16;
	_u16.size = 2;
	_u16.alignment = 2;
	_u16.is_signed = false;
	_u16.token = create_token("u16"_s);
	hmput(predefined_types, "u16", _u16);
	
	Ast_Type _u32;
	_u32.size = 4;
	_u32.alignment = 4;
	_u32.is_signed = false;
	_u32.token = create_token("u32"_s);
	hmput(predefined_types, "u32", _u32);
	
	Ast_Type _u64;
	_u64.size = 8;
	_u64.alignment = 8;
	_u64.is_signed = false;
	_u64.token = create_token("u64"_s);
	hmput(predefined_types, "u64", _u64);
	
	Ast_Type _s8;
	_s8.size = 1;
	_s8.alignment = 1;
	_s8.is_signed = true;
	_s8.token = create_token("s8"_s);
	hmput(predefined_types, "s8", _s8);
	
	Ast_Type _s16;
	_s16.size = 2;
	_s16.alignment = 2;
	_s16.is_signed = true;
	_s16.token = create_token("s16"_s);
	hmput(predefined_types, "s16", _s16);
	
	Ast_Type _s32;
	_s32.size = 4;
	_s32.alignment = 4;
	_s32.is_signed = true;
	_s32.token = create_token("s32"_s);
	hmput(predefined_types, "s32", _s32);
	
	Ast_Type _s64;
	_s64.size = 8;
	_s64.alignment = 8;
	_s64.is_signed = true;
	_s64.token = create_token("s64"_s);
	hmput(predefined_types, "s64", _s64);
	
	/* 
		for(u64 i = 0; i < hmlenu(predefined_types); i++)
		{
			Predefined_Type type = predefined_types[i];
			printf("Name [%s]\n", type.key);
			//Ast_Type* hmget(predefined_types, type_name);
		}
	*/
	
	//auto type = hmget(predefined_types, "u8");
}

Ast Parser::parse()
{
	
	Ast_Block* block = parse_block();
	//for(u64 i = 0; i < arrlenu(block->nodes); i++)
	PRINT_GRAPH(block, &logger);
	generate_block(block);
	
	/* 	
		for (int i=0; i < hmlen(symbolTable); ++i)
			logger.print("%s %s\n"_s, symbolTable[i].value.node->token.name, symbolTable[i].value.scope.name );
		*/
	
	/* 	
		for (int i=0; i < arrlen(scopes); ++i){
			auto scope = scopes[i];
			logger.print("Scope [%s], Id: [%d]\n"_s, scope.name, scope.id);
		}
		*/
	
	/* 	
		for(u64 i = 0; i < arrlenu(flaten_ast); i++){
			if (flaten_ast[i]->type == AST_DECLARATION) {
				Ast_Declaration* decl = (Ast_Declaration*)flaten_ast[i];
				if (decl->kind == AST_KIND_DECL_PROCEDURE){
					generate_proc((Ast_Proc_Declaration*)decl);
				}
				
			}
		}
		 */
	
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
		}
		*/
	
	
	
	return {0};
}

Ast_Type*
Parser::parse_type()
{
	Token token = lexer.peek_token();
	
	expect_and_eat(TOKEN_IDENT);
	
	char type_name[256] = {0};
	memcpy(type_name, token.name.data, token.name.count);
	
	s64 type_index = hmgeti(predefined_types, "u8");
	
	if (type_index == -1) {
		AllocateNode(Ast_Type, type);
		type->token = token;
		return type;
	}
	
	
	return &predefined_types[type_index].value;
}

Ast_Var_Declaration*
Parser::parse_argument_def()
{
	AllocateNode(Ast_Var_Declaration, decl);
	Token token = lexer.eat_token();
	if (token.type != TOKEN_IDENT)
	{
		
		exit(-1);
	}
	
	decl->token = token;
	
	expect_and_eat(TOKEN_COLON);
	decl->data_type = parse_type();
	
	token = lexer.peek_token();
	if (token.type == '='){
		lexer.eat_token();
		decl->body= parse_expression();;
	}
	
	return  decl;
}

Ast_Node*
Parser::parse_def()
{
	Token decl_name = lexer.eat_token(); // Name
	lexer.eat_token(); // eat the first ':'
	
	//printf("decl name [%.*s]\n", SV_PRINT(decl_name.name));
	
	Token token = lexer.peek_token();
	
	// Inferred
	{
		// Constant 
		if (token.type == TOKEN_COLON) {
			//lexer.eat_token();// eat ':'
			
			token = lexer.peek_token(1); 
			
			// Procedure 
			if (cmp2sv(token.name, "proc"_s) == 0)
			{
				lexer.eat_token(); // ':'
				lexer.eat_token(); // 'proc'
				AllocateNode(Ast_Proc_Declaration, proc);
				proc->token = decl_name;
				proc->scope = enter_scope();
				
				token = lexer.eat_token(); // (
				// Parse Arguments
				while(token.type != ')' && token.type != TOKEN_EOFA)
				{
					Ast_Var_Declaration* var = parse_argument_def();
					arrput(proc->scope->variables, var);
					token = lexer.peek_token();
					if (token.type != ')') expect_and_eat(',');
				}
				
				expect_and_eat(')');
				
				// Parse Return Types
				if (lexer.peek_token().type == TOKEN_ARROW){
					lexer.eat_token(); 
					token = lexer.peek_token();
					while(token.type != '{') {
						Ast_Type* type = parse_type();
						arrput(proc->return_type, type);
						token = lexer.peek_token();
						if (token.type != '{') expect_and_eat(',');
					}
				}
				
				
				// Parse Body
				proc->body = parse_block_of_statements();
				
				return proc;
			}
			else if (cmp2sv(token.name, "struct"_s) == 0){ assert(false && "Struct Not Supported Yet");}
			else if (cmp2sv(token.name, "enum"_s) == 0){ assert(false && "Enum Not Supported Yet");}
			else 
			{
				lexer.eat_token(); // ':'
				AllocateNode(Ast_Var_Declaration, var);
				var->token = decl_name;
				var->data_type = nullptr;
				var->body = parse_expression();
				var->constant = true;
				arrput(current_scope->variables, var);
				expect_and_eat(';');
				return var;
			}
		}
		
		
		//  imutable;
		if (token.type == '='){
			lexer.eat_token(); // '='
			AllocateNode(Ast_Var_Declaration, var);
			var->token = decl_name;
			var->data_type = nullptr; // Todo : infered??
			var->body = parse_expression();
			var->constant = false;
			arrput(current_scope->variables, var);
			expect_and_eat(';');
			return var;
		}
	}
	
	AllocateNode(Ast_Var_Declaration, var);
	var->token = decl_name;
	var->data_type = parse_type(); // Todo : infered??
	if      (lexer.peek_token().type == TOKEN_COLON) var->constant = true;
	else if (lexer.peek_token().type == '=') var->constant = false;
	else    assert(false && "Expected [:] or [=]");
	lexer.eat_token();
	var->body = parse_expression();
	var->constant = false;
	arrput(current_scope->variables, var);
	expect_and_eat(';');
	return var;
}

Ast_Node* 
Parser::parse_statement_expression()
{
	Token t1 = lexer.peek_token();// ident 
	Token t2 = lexer.peek_token(1);
	
	// Declaration
	if (t1.type == TOKEN_IDENT && t2.type == TOKEN_COLON)
	{
		return parse_def();
	}
	
	
	// Statements
	Ast_Node* exp = parse_expression();
	expect_and_eat(';');
	return exp;
}

Ast_Node*
Parser::parse_statement()
{
	Ast_Node* return_node = nullptr;
	
	
	Token t1 = lexer.peek_token();// ident 
	//Token t2 = lexer.peek_token(1);
	
	// TODO: Handle comments 
	if (t1.type == TOKEN_COMMENT || t1.type == TOKEN_MULTILINE_COMMENT){
		lexer.eat_token();
		return nullptr;
	}
	
	// TODO: Handle Compile-time directives
	
	// Scopes
	if (t1.type == '{'){
		return parse_block_of_statements();
	}
	
	// TODO: Handle Return statements
	if (cmp2sv(t1.name, "return"_s) == 0){
		AllocateNode(Ast_Return, _return);
		_return->token = lexer.eat_token();
		
		t1 = lexer.peek_token();
		while (t1.type != ';'){
			arrput(_return->expressions, (Ast_Expression*)parse_expression()); 
			t1 = lexer.peek_token();
			if (t1.type != ';') expect_and_eat(',');
		}
		expect_and_eat(';');
		return _return;
		//assert(false && "Return Statements are not supported yet");
	}
	
	return parse_statement_expression();
	
	
	
}

// TODO: Merge token_to_operation with get_binary_precedence
static s64 token_to_operation(const Token& token){
	
	switch(token.type)
	{
		case '+': return AST_BINARY_PLUS;
		case '-': return AST_BINARY_MINUS;
		case '*': return AST_BINARY_MUL;
		case '/': return AST_BINARY_DIV;
		case '=': return AST_BINARY_ASSIGN;
		default:
		{
			printf("Unknown Operator [%.*s]\n", SV_PRINT(token.name));
			assert(false && "Unknown Operator");
			break;
		}
	}
	
	return -1;
}

static s8 get_binary_precedence(const Token& token)
{
	switch(token.type)
	{
		case '*':
		case '/':
		{
			return 10;
		}
		case '+':
		case '-':
		{
			return 9;
		}
		case '=':
		//case ':':
		{
			return 1;
		}
	}
	
	
	return 0;
}

Ast_Expression* 
Parser::parse_expression(s8 priority /* = -1*/ )
{
	Ast_Expression* left = parse_unary_expression();
	
	
	while (true)
	{
		s8 new_priority = get_binary_precedence(lexer.peek_token()); 
		
		if (new_priority == 0 || new_priority <= priority)
		{
			return left;
		}
		
		AllocateNode(Ast_Binary, binary);
		binary->token = lexer.eat_token();
		binary->op = token_to_operation(binary->token);
		binary->left = left;
		binary->right = parse_expression(new_priority);
		
		
		left = binary;
		
	}
}

Ast_Expression* 
Parser::parse_unary_expression()
{
	auto token = lexer.peek_token();
	
	Ast_Expression* result = nullptr;
	if (token.type == '(') // factors
	{
		lexer.eat_token(); // '('
		result = parse_expression();
		expect_and_eat(')');
		
		return parse_suffix_expression(result);
	}
	else if (token.type == '-' || token.type == '+' ) // minus 
	{
		lexer.eat_token(); //  eat the sign
		AllocateNode(Ast_Unary, unary);
		unary->token = token;
		unary->child = parse_suffix_expression(parse_primary_expression());
		return unary;
	}
	else if (token.type== '*') // addressof 
	{
		assert(false && "Pointers are not supported yet");
		
	}
	
	
	return parse_suffix_expression(parse_primary_expression());
}

Ast_Expression* 
Parser::parse_suffix_expression(Ast_Expression* prev)
{
	Token token = lexer.peek_token();
	
	// early exit 
	if (token.type == ';'){
		return prev; 
	}
	
	if (token.type == '('){
		// funccall // 
		assert(false && "Function Calls are not supported yet");
	}
	
	if (token.type == '['){
		assert(false && "Array Subscripts are not supported yet");
	}
	
	if (token.type == '.'){
		assert(false && "Access Operator is not supported yet");
	}
	
	
	return prev; 
}

Ast_Primary* 
Parser::parse_primary_expression()
{
	auto token = lexer.eat_token();
	AllocateNode(Ast_Primary, primary);
	primary->token = token;
	
	switch(token.type)
	{
		case TOKEN_IDENT: 
		{
			primary->priamry_kind = AST_KIND_PRIMARY_IDENTIFIER;
			break;
		}
		case TOKEN_LITERAL: 
		{
			if (token.kind == TOKEN_KIND_STRING_LITERAL) 
				primary->priamry_kind = AST_KIND_PRIMARY_STRING;
			else 
				primary->priamry_kind = AST_KIND_PRIMARY_NUMBER;
			break;
		}
		default:
		{
			printf("line: %d, Col: %d, [%.*s]\n", token.start_position.x,token.start_position.y ,SV_PRINT(token.name));
			printf("not a primary expression\n");
			exit(-1);
			break;
		}
	}
	
	return primary;
}

Ast_Block*
Parser::parse_block_of_statements()
{
	expect_and_eat('{');
	Ast_Block* statements = parse_block();
	expect_and_eat('}');
	return statements;
}

Ast_Block*
Parser::parse_block()
{
	Ast_Scope* scope = enter_scope();
	
	AllocateNode(Ast_Block, block);
	block->scope = scope;
	
	Token token = lexer.peek_token();
	// TODO:  will this break ?? 
	block->token = create_token(lexer.peek_token().name);
	
	while(token.type != TOKEN_EOFA &&
		  token.type != '}')
	{
		Ast_Node* statement = parse_statement();
		
		if (statement) arrput(block->statements, statement);
		
		token = lexer.peek_token();
	}
	
	exit_scope();
	return block;
}


