/* ========================================================================
$File: parser.cpp
$Date: 2020-10-16
$Creator: Husam Dababneh
$Description: parser.cpp
========================================================================*/

#include "parser.h"

#define ENABLE_GRAPH_PRINTING 1
#define GRAPH_SHOW_BLOCK_PARENT 0
#include <extra/graph.cpp>

// :nocheckin
#include "generator.cpp"


// TODO: Re-read the Tracy documentation to use this in the right way
#include "../submodules/tracy/Tracy.hpp"

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


#define RED   "\033[0;31m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

static void SyntaxError(const Token& token, const StringView& message) 
{
	printf(RED"Error");
	printf(CYAN"[%d, %d]: ", token.start_position.line, token.start_position.index);
	printf(RESET"%s\n", *message);
	exit(-1);
}

inline 
void _expect_and_eat(Parser* _this, u64 type, u64 line, const char* file)
{
	Token& token = _this->lexer.peek_token();
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", file ,line);
		printf("Expected [%c, %d] got %.*s \n", (char)type, (int)type, SV_PRINT(token.name));
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.line, token.start_position.index);
		
		SyntaxError(token, "Expected Another Token Here"_s);
		exit(-1);
	}
	_this->lexer.eat_token();
	
}
#define expect_and_eat(x) _expect_and_eat(this, x, __LINE__, __FILE__)


inline Ast_Scope*
Parser::enter_scope()
{
	Ast_Scope* prev = current_scope;
	Ast_Scope ss = {0};
	arrput(scopes, ss);
	Ast_Scope* scope = &arrlast(scopes);
	
	
	if (prev != nullptr){
		arrput(prev->children, scope);
	}
	
	scope->parent = prev;
	current_scope = scope;
	return scope;
}


inline void
Parser::exit_scope()
{
	assert(current_scope);
	current_scope = current_scope->parent;
}

Predefined_Type* Parser::predefined_types = nullptr;


#define MHash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
// TODO: Remove this
Token create_token(StringView name)
{
	static u32 id = 0;
	id = (id + 3772) % 1000;
	Token token = {0};
	token.name = name;
	token.start_position = {id+22, id+9};
	token.hash = MHash(token);
	
	return token;
}

inline 
Ast_Type create_type(const u32 size, 
					 const u32 alignment, 
					 const bool is_signed, 
					 const StringView& name)
{
	Ast_Type type;
	type.size = size;
	type.alignment = alignment;
	type.is_signed = is_signed;
	type.token = create_token(name);
	
	return type;
}


inline 
Ast_Type create_type(const u32 size, 
					 const u32 alignment, 
					 const bool is_signed, 
					 const Token& token)
{
	Ast_Type type;
	type.size = size;
	type.alignment = alignment;
	type.is_signed = is_signed;
	type.token = token;
	
	return type;
}


void 
Parser::register_predefined_types()
{
	Ast_Type _u8 = create_type(1, 1, false, "u8"_s);
	hmput(predefined_types, _u8.token.name.str, _u8);
	
	Ast_Type _u16 = create_type(2, 2, false, "u16"_s);
	hmput(predefined_types, _u16.token.name.str, _u16);
	
	Ast_Type _u32 = create_type(4, 4, false, "u32"_s);
	hmput(predefined_types, _u32.token.name.str, _u32);
	
	Ast_Type _u64 = create_type(8, 8, false, "u64"_s);
	hmput(predefined_types, _u64.token.name.str, _u64);
	
	Ast_Type _s8 = create_type(1, 1, true, "s8"_s);
	hmput(predefined_types, _s8.token.name.str, _s8);
	
	Ast_Type _s16 = create_type(2, 2, true, "s16"_s);
	hmput(predefined_types, _s16.token.name.str, _s16);
	
	Ast_Type _s32 = create_type(4, 4, true, "s32"_s);
	hmput(predefined_types, _s32.token.name.str, _s32);
	
	Ast_Type _s64 = create_type(8, 8, true, "s64"_s);
	hmput(predefined_types, _s64.token.name.str, _s64);
	
	
	/* 	
		for(u64 i = 0; i < hmlenu(predefined_types); i++)
		{
			Predefined_Type type = predefined_types[i];
			//printf("Name [%s]\n", type.key);
			//printf("Name [%s]\n", *type.value.token.value);
			//Ast_Type* hmget(predefined_types, type_name);
		}
		*/
	
	//auto type = hmget(predefined_types, "u8");
}


void print_scopes(Ast_Scope* scope);
void print_struct_decl(Ast_Struct_Declaration* decl) 
{
	if (decl == nullptr) return;
	
	for (u64 it = 0; it < arrlenu(decl->decls); it++) 
	{
		auto& it_data = decl->decls[it]; 
		printf("%.*s: [%.*s]\n", SV_PRINT(decl->token.name),SV_PRINT(it_data->token.name));
	} 
}

void print_decls(Ast_Scope* scope)
{
	for (u64 it = 0; it < arrlenu(scope->procedures); it++) 
	{
		auto& it_data = scope->procedures[it];
		printf("\tFunc: [%.*s]\n", SV_PRINT(it_data->token.name));
	}
	
	for (u64 it = 0; it < arrlenu(scope->variables); it++) 
	{
		auto& it_data = scope->variables[it];
		printf("\tVar: [%.*s]\n", SV_PRINT(it_data->token.name));
	}
	
	for (u64 it = 0; it < arrlenu(scope->structs); it++) 
	{
		auto& it_data = scope->structs[it];
		printf("\tStruct: [%.*s]\n", SV_PRINT(it_data->token.name));
		//print_struct_decl(it_data);
	}
}

void print_scopes(Ast_Scope* scope)
{
	if (scope == nullptr) return;
	
	printf("Start Scope #%p--------------------------------------------\n", scope);
	
	print_decls(scope);
	for(u64 it = 0; it < arrlenu(scope->children); it++)
	{
		print_scopes(scope->children[it]);
	}
	
	printf("End Scope #%p----------------------------------------------\n", scope);
	//printf("---------\n");
	
}



static Queue<StringView> files = make_queue<StringView>(10);

Ast Parser::parse()
{
	
	bool still_parsing = true;
	
	
	Ast_Block* block = parse_block();
	//PRINT_GRAPH(block, &logger);
	print_scopes(block->scope);
	//generate(block);
	
	
	
	/* 
		for(u64 i = 0; i < hmlenu(predefined_types); i++)
		{
			Predefined_Type type = predefined_types[i];
			printf("Name [%s]\n", type.key);
			//printf("Name [%s]\n", *type.value.token.value);
			//Ast_Type* hmget(predefined_types, type_name);
		}
		 */
	
	return {0};
}

Ast_Type*
Parser::parse_type()
{
	Token token = lexer.peek_token();
	
	expect_and_eat(TOKEN_IDENT);
	
	s64 type_index = hmgeti(predefined_types, token.name.str);
	
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
	enter_scope();
	
	Token decl_name = lexer.eat_token(); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	lexer.eat_token(); // eat 'proc'
	
	AllocateNode(Ast_Proc_Declaration, proc);
	proc->token = decl_name;
	proc->scope = current_scope;
	expect_and_eat('(');
	
	Token token = lexer.peek_token();
	// Parse Arguments
	while(token.type != ')' && token.type != TOKEN_EOFA)
	{
		Ast_Var_Declaration* var = parse_argument_def();
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
	
	exit_scope();
	
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
	
	if (!var->constant && token.type == TOKEN_SEMI_COLON)  
		goto exit;
	
	var->body = parse_expression();
	
	exit:
	arrput(current_scope->variables, var);
	expect_and_eat(TOKEN_SEMI_COLON);
	return var;
}


Ast_Struct_Declaration* 
Parser::parse_struct_def()
{
	Token decl_name = lexer.eat_token(); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	Token token = lexer.eat_token(); // eat 'struct'
	
	enter_scope();
	AllocateNode(Ast_Struct_Declaration, _struct);
	_struct->token = decl_name;
	_struct->scope = current_scope;
	
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
	
	// Add Struct to types
	
	hmput(predefined_types, decl_name.name.str, create_type(0,0,0,decl_name));
	
	//arrput(current_scope->types, type);
	
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
	expect_and_eat(TOKEN_SEMI_COLON);
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
	if (t1.type == TOKEN_DIRECTIVE) {
		return parse_directive();
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
		while (t1.type != TOKEN_SEMI_COLON){
			arrput(_return->expressions, (Ast_Expression*)parse_expression()); 
			t1 = lexer.peek_token();
			if (t1.type != TOKEN_SEMI_COLON) expect_and_eat(',');
		}
		expect_and_eat(TOKEN_SEMI_COLON);
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
	if (token.type == TOKEN_SEMI_COLON){
		return prev; 
	}
	
	if (token.type == '('){
		lexer.eat_token();
		
		
		AllocateNode(Ast_Proc_Call, proc_call);
		proc_call->token = token;
		
		Token next = lexer.peek_token();
		while(next.type != ')')
		{
			Ast_Expression* exp = parse_expression();
			arrput(proc_call->arguments, exp);
			next = lexer.peek_token();
			if (next.type != ')') expect_and_eat(',');
		}
		
		expect_and_eat(')');
		proc_call->procedure = prev;
		
		return parse_suffix_expression(proc_call);
		// funccall //
		//assert(false && "Function Calls are not supported yet");
	}
	
	if (token.type == '['){
		lexer.eat_token();
		AllocateNode(Ast_Subscript, sub);
		sub->token = token;
		sub->value= parse_expression();
		sub->exp= prev;
		
		expect_and_eat(']');
		
		return parse_suffix_expression(sub);
		//assert(false && "Array Subscripts are not supported yet");
	}
	
	if (token.type == '.'){
		lexer.eat_token();
		AllocateNode(Ast_Member_Access, mem_acc);
		mem_acc->token = token;
		
		if (lexer.peek_token().type != TOKEN_IDENT) {
			printf("Identifer Must be.. \n");
			exit(-1);
		}
		
		mem_acc->_struct = (Ast_Primary*)prev;
		mem_acc->member = lexer.eat_token();
		return parse_suffix_expression(mem_acc);
		//assert(false && "Access Operator is not supported yet");
	}
	
	
	return prev; 
}

Ast_Expression* 
Parser::parse_primary_expression()
{
	auto token = lexer.eat_token();
	
	switch(token.type)
	{
		case TOKEN_IDENT: 
		{
			AllocateNode(Ast_Primary, primary);
			primary->token = token;
			primary->priamry_kind = AST_KIND_PRIMARY_IDENTIFIER;
			return primary;
			break;
		}
		case TOKEN_LITERAL: 
		{
			AllocateNode(Ast_Literal, literal);
			
			if (token.kind == TOKEN_KIND_STRING_LITERAL) 
				literal->literal_kind = AST_KIND_LITERAL_STRING;
			else 
				literal->literal_kind = AST_KIND_LITERAL_NUMBER;
			literal->token = token;
			return literal;
			break;
		}
		default:
		{
			printf("Token [%.*s] At [Line: %d, Col: %d], Is not a primary expression \n",SV_PRINT(token.name), token.start_position.line,token.start_position.index);
			exit(-1);
			break;
		}
	}
	
	return nullptr;
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
	enter_scope();
	AllocateNode(Ast_Block, block);
	block->scope = current_scope; 
	
	Token token = lexer.peek_token();
	// TODO:  will this break ?? 
	block->token = create_token(lexer.peek_token().name);
	
	while(token.type != TOKEN_EOFA && token.type != '}')
	{
		Ast_Node* statement = parse_statement();
		
		if (statement) arrput(block->statements, statement);
		
		token = lexer.peek_token();
	}
	
	exit_scope();
	return block;
}


Ast_Node* 
Parser::parse_directive()
{
	Token directive = lexer.eat_token();
	if (cmp2sv(directive.name, "#import"_s) == 0)
	{
		Token filename = lexer.eat_token();
		
		if (filename.type != TOKEN_LITERAL ||
			filename.kind != TOKEN_KIND_STRING_LITERAL)
		{
			SyntaxError(filename, "\"Import\" Directive Expect a string literal"_s);
		}
		
		Token next = lexer.peek_token();
		
		AllocateNode(Ast_Directive_Import, import);
		import->token = directive;
		import->filename = filename;
		
		if (cmp2sv(next.name, "as"_s) == 0)
		{
			lexer.eat_token();
			import->isAs = true;
			import->as = next;
		}
		
		// Add filename to parsing queue :) 
		return import;
	}
	
	
	
	
	printf("%s Directive is not Supported Yet\n", *directive.name );
	assert(false);
	
	return nullptr;
}

