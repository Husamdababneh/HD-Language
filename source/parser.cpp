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

inline 
void _expect_and_eat(Parser* _this, u64 type, u64 line, const char* file)
{
	Token& token = _this->lexer.peek_token();
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", file ,line);
		printf("Expected [%c, %d] got \"%.*s\"\n", (int)type, (int)type, SV_PRINT(token.name));
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.x, token.start_position.y);
		exit(-1);
	}
	_this->lexer.eat_token();
	
}

//_CrtDbgBreak();
#define expect_and_eat(x) _expect_and_eat(this, x, __LINE__, __FILE__)


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
	current_scope = current_scope->parent;
}

Predefined_Type* Parser::predefined_types = nullptr;


#define Hash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
// TODO: Remove this
Token create_token(StringView name)
{
	static u32 id = 0;
	id = (id + 3772) % 1000;
	Token token = {0};
	token.name = name;
	token.start_position = {id+22, id+9};
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


void print_scopes(Ast_Scope* scope);

void print_struct_decl(Ast_Struct_Declaration* decl) 
{
	if (decl == nullptr) return;
	STBARRFOR(decl->decls, {
				  printf("\t%.*s: [%.*s]\n", SV_PRINT(decl->token.name),SV_PRINT(it_data->token.name));
			  });
}

void print_decls(Ast_Scope* scope)
{
	STBARRFOR(scope->procedures , {
				  printf("Func: [%.*s]\n", SV_PRINT(it_data->token.name)); //print_scopes(it_data->self_scope); 
				  print_scopes(it_data->body->scope);
			  } );
	
	STBARRFOR(scope->variables, {printf("\tVar: [%.*s]\n", SV_PRINT(it_data->token.name));});
	STBARRFOR(scope->structs, {
				  printf("Struct: [%.*s]\n", SV_PRINT(it_data->token.name));
				  print_struct_decl(it_data);});
}

void print_scopes(Ast_Scope* scope)
{
	if (scope == nullptr) return;
	static int i = 0;
	print_decls(scope);
	for(u64 it = 0; it < arrlenu(scope->children); it++)
	{
		print_scopes(&scope->children[it]);
	}
	//printf("---------\n");
	
}

Ast Parser::parse()
{
	
	Ast_Block* block = parse_block();
	print_scopes(block->scope);
	//PRINT_GRAPH(block, &logger);
	//generate(block);
	
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
		decl->body= parse_expression();
	}
	
	
	arrput(current_scope->variables, decl);
	return  decl;
}

Ast_Proc_Declaration* 
Parser::parse_proc_def()
{
	Token decl_name = lexer.eat_token(); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	lexer.eat_token(); // eat 'proc'
	
	AllocateNode(Ast_Proc_Declaration, proc);
	proc->token = decl_name;
	proc->scope = enter_scope();//current_scope;
	//proc->self_scope = 
	expect_and_eat('(');
	
	Token token = lexer.peek_token();
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
	
	exit_scope();
	// Parse Body
	proc->body = parse_block_of_statements();
	
	// Add method to current scope
	arrput(current_scope->procedures, proc);
	return proc;
}

Ast_Var_Declaration*
Parser::parse_var_def()
{
	Token decl_name = lexer.eat_token(); // Name
	Token token = lexer.peek_token();
	
	Ast_Type* type = nullptr;
	AllocateNode(Ast_Var_Declaration, var);
	var->token = decl_name;
	
	if (token.type == TOKEN_COLON) // Forced Type
	{
		lexer.eat_token();
		type = parse_type();
		token = lexer.peek_token();
		var->data_type = nullptr;
		
		// Immutable Variable
		if (token.type == TOKEN_COLON){
			var->constant = true;
			lexer.eat_token();
		}
		
		// Mutable Variable
		if (token.type == '='){
			var->constant = false;
			lexer.eat_token();
		}
		
	} 
	// Inferred
	else if (token.type == TOKEN_COLON_EQUAL) 
	{
		var->constant = true;
		lexer.eat_token();
	}
	else if (token.type == TOKEN_DOUBLE_COLON) {
		var->constant = false;
		lexer.eat_token();
	}
	else {
		assert(false);
	}
	
	
	
	token = lexer.peek_token();
	// If it's a constant it have to have a value; 
	
	if (!var->constant && token.type == ';')  
		goto exit;
	
	var->body = parse_expression();
	
	exit:
	arrput(current_scope->variables, var);
	expect_and_eat(';');
	return var;
}


Ast_Struct_Declaration* 
Parser::parse_struct_def()
{
	Token decl_name = lexer.eat_token(); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	Token token = lexer.eat_token(); // eat 'struct'
	
	
	AllocateNode(Ast_Struct_Declaration, _struct);
	_struct->token = decl_name;
	_struct->scope = enter_scope();//current_scope;
	//_struct->self_scope = 
	
	expect_and_eat('{');
	
	
	token = lexer.peek_token(); 
	while(token.type != '}' && token.type != TOKEN_EOFA)
	{
		Ast_Var_Declaration* var = parse_var_def();
		arrput(_struct->decls, var);
		token = lexer.peek_token();
		if (token.type != '}') continue;
	}
	
	expect_and_eat('}');
	exit_scope();
	
	// Add the new type to current scope
	arrput(current_scope->structs, _struct);
	
	return _struct;
}

Ast_Node* 
Parser::parse_const_def()
{
	Token token = lexer.peek_token(2); 
	if      (cmp2sv(token.name, "proc"_s) == 0) return parse_proc_def();
	else if (cmp2sv(token.name, "struct"_s) == 0) return parse_struct_def();
	else if (cmp2sv(token.name, "enum"_s) == 0) assert(false && "Enum Not Supported Yet");
	else if (cmp2sv(token.name, "flag"_s) == 0) assert(false && "Flag Not Supported Yet");
	else    return parse_var_def();
	return nullptr;
}

Ast_Node* 
Parser::parse_statement_expression()
{
	Token t1 = lexer.peek_token();// ident 
	Token t2 = lexer.peek_token(1);
	
	if (t1.type == TOKEN_IDENT && t2.type == TOKEN_DOUBLE_COLON)
	{
		// Constant Variables, Structs, Procs, Flags, Enums
		return parse_const_def();
	}
	
	if (t1.type == TOKEN_IDENT && (t2.type == TOKEN_COLON ||  t2.type == TOKEN_COLON_EQUAL))
	{
		// Variable Declarations 
		return parse_var_def();
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
	if (t1.type == TOKEN_DIRECTIVE){
		lexer.eat_token();
		return nullptr;
	}
	
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
			printf("Token [%.*s] At [Line: %d, Col: %d], Is not a primary expression \n",SV_PRINT(token.name), token.start_position.x,token.start_position.y);
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


