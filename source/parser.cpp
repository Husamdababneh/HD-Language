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


// Forward declarations

Ast parse(Parser& );
Ast_Node* parse_statement(Parser& );
Ast_Node* parse_const_def(Parser& );
Ast_Type* parse_type(Parser& );
Ast_Block* parse_block(Parser& );

Ast_Node* parse_directive(Parser& );
Ast_Node* parse_if_statement(Parser& );
Ast_Var_Declaration* parse_var_def(Parser& );
Ast_Proc_Declaration* parse_proc_def(Parser& );
Ast_Node* parse_statement_expression(Parser& );
Ast_Block* parse_block_of_statements(Parser& );
Ast_Proc_Declaration* parse_proc_header(Parser& );
Ast_Var_Declaration* parse_argument_def(Parser& );
Ast_Struct_Declaration* parse_struct_def(Parser& );

Ast_Expression* parse_expression(Parser&, S8 priority = -1);
Ast_Expression* parse_suffix_expression(Parser&, Ast_Expression* prev);
Ast_Expression* parse_unary_expression(Parser& );
Ast_Expression* parse_primary_expression(Parser& );

// TODO: Re-read the Tracy documentation to use this in the right way
#include "../submodules/tracy/Tracy.hpp"

Ast_Node** flaten_ast = NULL;

// How does this perform ?? 
#define AllocateNode(type, name) type* name;\
name = (type*) arena_alloc(&parser.ast_arena, sizeof(type));\
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
	printf(RESET"%.*s\n", SV_PRINT(message));
	exit(-1);
}

inline 
void _expect_and_eat(Parser* _this, U64 type, U64 line, const char* file)
{
	Token& token = peek_token(_this->lexer);
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", file ,line);
		
		auto token_type = token_type_to_string(type); 
		if(token_type.length <= 1){
			printf("Expected: %.*s(\'%c\', %d)  got \"%.*s\" \n", SV_PRINT(token_type_to_string(type)), (char)type, (int)type, SV_PRINT(token.name));
		}else{
			printf("Expected: %.*s(%d)  got \"%.*s\" \n", SV_PRINT(token_type_to_string(type)), (int)type, SV_PRINT(token.name));
		}
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.line, token.start_position.index);
		
		SyntaxError(token, "Expected Another Token Here"_s);
		exit(-1);
	}
	eat_token(_this->lexer);
	
}
#define expect_and_eat(x) _expect_and_eat(&parser, x, __LINE__, __FILE__)


inline Ast_Scope*
enter_scope(Parser& parser)
{
	Ast_Scope* prev = parser.current_scope;
	Ast_Scope ss = {0};
	arrput(parser.scopes, ss);
	Ast_Scope* scope = &arrlast(parser.scopes);
	
	
	if (prev != nullptr){
		arrput(prev->children, scope);
	}
	
	scope->parent = prev;
	parser.current_scope = scope;
	return scope;
}


inline void
exit_scope(Parser& parser)
{
	assert(parser.current_scope);
	parser.current_scope = parser.current_scope->parent;
}

Predefined_Type* Parser::predefined_types = nullptr;


#define MHash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
// TODO: Remove this
Token create_token(StringView name)
{
	static U32 id = 0;
	id = (id + 3772) % 1000;
	Token token = {0};
	token.name = name;
	token.start_position = {id+22, id+9};
	token.hash = MHash(token);
	
	return token;
}

inline 
Ast_Type 
create_type(const U32 size, 
			const U32 alignment, 
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
Ast_Type create_type(const U32 size, 
					 const U32 alignment, 
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
register_predefined_types(Parser& parser)
{
	Ast_Type _U8 = create_type(1, 1, false, "U8"_s);
	hmput(parser.predefined_types, _U8.token.name.str_char, _U8);
	
	Ast_Type _u16 = create_type(2, 2, false, "u16"_s);
	hmput(parser.predefined_types, _u16.token.name.str_char, _u16);
	
	Ast_Type _U32 = create_type(4, 4, false, "U32"_s);
	hmput(parser.predefined_types, _U32.token.name.str_char, _U32);
	
	Ast_Type _U64 = create_type(8, 8, false, "U64"_s);
	hmput(parser.predefined_types, _U64.token.name.str_char, _U64);
	
	Ast_Type _S8 = create_type(1, 1, true, "S8"_s);
	hmput(parser.predefined_types, _S8.token.name.str_char, _S8);
	
	Ast_Type _s16 = create_type(2, 2, true, "s16"_s);
	hmput(parser.predefined_types, _s16.token.name.str_char, _s16);
	
	Ast_Type _s32 = create_type(4, 4, true, "s32"_s);
	hmput(parser.predefined_types, _s32.token.name.str_char, _s32);
	
	Ast_Type _S64 = create_type(8, 8, true, "S64"_s);
	hmput(parser.predefined_types, _S64.token.name.str_char, _S64);
	
	
	/* 	
		for(U64 i = 0; i < hmlenu(predefined_types); i++)
		{
			Predefined_Type type = predefined_types[i];
			//printf("Name [%s]\n", type.key);
			//printf("Name [%s]\n", *type.value.token.value);
			//Ast_Type* hmget(predefined_types, type_name);
		}
		*/
	
	//auto type = hmget(predefined_types, "U8");
}


void print_scopes(Ast_Scope* scope);
void print_struct_decl(Ast_Struct_Declaration* decl) 
{
	if (decl == nullptr) return;
	
	for (U64 it = 0; it < arrlenu(decl->decls); it++) 
	{
		auto& it_data = decl->decls[it]; 
		printf("%.*s: [%.*s]\n", SV_PRINT(decl->token.name),SV_PRINT(it_data->token.name));
	} 
}

void print_decls(Ast_Scope* scope)
{
	for (U64 it = 0; it < arrlenu(scope->procedures); it++) 
	{
		auto& it_data = scope->procedures[it];
		printf("\tFunc: [%.*s]\n", SV_PRINT(it_data->token.name));
	}
	
	for (U64 it = 0; it < arrlenu(scope->variables); it++) 
	{
		auto& it_data = scope->variables[it];
		printf("\tVar: [%.*s]\n", SV_PRINT(it_data->token.name));
	}
	
	for (U64 it = 0; it < arrlenu(scope->structs); it++) 
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
	for(U64 it = 0; it < arrlenu(scope->children); it++)
	{
		print_scopes(scope->children[it]);
	}
	
	printf("End Scope #%p----------------------------------------------\n", scope);
	//printf("---------\n");
	
}



//static Queue<StringView> files = make_queue<StringView>(10);

Ast parse(Parser& parser)
{
	
	bool still_parsing = true;
	
	
	Ast_Block* block = parse_block(parser);
	PRINT_GRAPH(block, &parser.logger);
	//print_scopes(block->scope);
	//generate(block);
	
	
	
	/* 
		for(U64 i = 0; i < hmlenu(predefined_types); i++)
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
parse_type(Parser& parser)
{
	auto& lexer = parser.lexer;
	Token token = peek_token(lexer);
	
	expect_and_eat(TOKEN_IDENT);
	
	S64 type_index = hmgeti(parser.predefined_types, token.name.str);
	
	if (type_index == -1) {
		AllocateNode(Ast_Type, type);
		type->token = token;
		return type;
	}
	
	
	return &parser.predefined_types[type_index].value;
}

Ast_Var_Declaration*
parse_argument_def(Parser& parser)
{
	auto& lexer = parser.lexer;
	AllocateNode(Ast_Var_Declaration, decl);
	Token token = eat_token(lexer);
	if (token.type != TOKEN_IDENT)
	{
		
		exit(-1);
	}
	
	decl->token = token;
	
	expect_and_eat(TOKEN_COLON);
	decl->data_type = parse_type(parser);
	
	token = peek_token(lexer);
	if (token.type == '='){
		eat_token(lexer);
		decl->body= parse_expression(parser);
	}
	
	
	arrput(parser.current_scope->variables, decl);
	return  decl;
}


// TODO(Husam Dababneh): Rethink this part
Ast_Proc_Declaration* 
parse_proc_header(Parser& parser) 
{
	auto& lexer = parser.lexer;
	Token decl_name = eat_token(lexer); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	eat_token(lexer); // eat 'proc'
	
	AllocateNode(Ast_Proc_Declaration, proc);
	proc->token = decl_name;
	proc->scope = parser.current_scope;
	expect_and_eat('(');
	
	Token token = peek_token(lexer);
	// Parse Arguments
	while(token.type != ')' && token.type != TOKEN_EOFA)
	{
		Ast_Var_Declaration* var = parse_argument_def(parser);
		token = peek_token(lexer);
		if (token.type != ')') expect_and_eat(',');
	}
	
	expect_and_eat(')');
	
	// Parse Return Types
	if (peek_token(lexer).type == TOKEN_ARROW){
		eat_token(lexer); 
		token = peek_token(lexer);
		while(token.type != '{') {
			Ast_Type* type = parse_type(parser);
			arrput(proc->return_type, type);
			token = peek_token(lexer);
			if (token.type != '{') expect_and_eat(',');
		}
	}
	return proc;
}

Ast_Proc_Declaration* 
parse_proc_def(Parser& parser)
{
	enter_scope(parser);
	
	Ast_Proc_Declaration* proc = nullptr;
	proc = parse_proc_header(parser);
	proc->body = parse_block_of_statements(parser);
	
	exit_scope(parser);
	
	// Add method to current scope
	arrput(parser.current_scope->procedures, proc);
	return proc;
}

Ast_Var_Declaration*
parse_var_def(Parser& parser)
{
	auto& lexer = parser.lexer;
	Token decl_name = eat_token(lexer); // Name
	Token token = peek_token(lexer);
	
	Ast_Type* type = nullptr;
	AllocateNode(Ast_Var_Declaration, var);
	var->token = decl_name;
	
	if (token.type == TOKEN_COLON) // Forced Type
	{
		eat_token(lexer);
		type = parse_type(parser);
		token = peek_token(lexer);
		var->data_type = nullptr;
		
		// Immutable Variable
		if (token.type == TOKEN_COLON){
			var->constant = true;
			eat_token(lexer);
		}
		
		// Mutable Variable
		if (token.type == '='){
			var->constant = false;
			eat_token(lexer);
		}
		
	} 
	// Inferred
	else if (token.type == TOKEN_COLON_EQUAL) 
	{
		var->constant = true;
		eat_token(lexer);
	}
	else if (token.type == TOKEN_DOUBLE_COLON) {
		var->constant = false;
		eat_token(lexer);
	}
	else {
		assert(false);
	}
	
	
	
	token = peek_token(lexer);
	// If it's a constant it have to have a value; 
	
	if (!var->constant && token.type == TOKEN_SEMI_COLON)  
		goto exit;
	
	var->body = parse_expression(parser);
	
	exit:
	arrput(parser.current_scope->variables, var);
	expect_and_eat(TOKEN_SEMI_COLON);
	return var;
}


Ast_Struct_Declaration* 
parse_struct_def(Parser& parser)
{
	auto& lexer = parser.lexer;
	Token decl_name = eat_token(lexer); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	Token token = eat_token(lexer); // eat 'struct'
	
	enter_scope(parser);
	AllocateNode(Ast_Struct_Declaration, _struct);
	_struct->token = decl_name;
	_struct->scope = parser.current_scope;
	
	expect_and_eat('{');
	
	
	token = peek_token(lexer); 
	while(token.type != '}' && token.type != TOKEN_EOFA)
	{
		Ast_Var_Declaration* var = parse_var_def(parser);
		arrput(_struct->decls, var);
		token = peek_token(lexer);
		if (token.type != '}') continue;
	}
	
	expect_and_eat('}');
	exit_scope(parser);
	
	// Add the new type to current scope
	arrput(parser.current_scope->structs, _struct);
	
	// Add Struct to types
	
	hmput(parser.predefined_types, decl_name.name.str_char, create_type(0,0,0,decl_name));
	
	//arrput(current_scope->types, type);
	
	return _struct;
}



Ast_Node* 
parse_const_def(Parser& parser)
{
	auto lexer = parser.lexer;
	Token token = peek_token(lexer, 2); 
	if      (EqualStrings(token.name, "proc"_s)) return parse_proc_def(parser);
	else if (EqualStrings(token.name, "struct"_s)) return parse_struct_def(parser);
	// NOTE (Husam): I need to justify the existance of this aproch
	else if (EqualStrings(token.name, "template"_s)) assert(false && "Templates are Not Supported Yet");
	else if (EqualStrings(token.name, "enum"_s)) assert(false && "Enums are Not Supported Yet");
	else if (EqualStrings(token.name, "flag"_s)) assert(false && "Flags are Not Supported Yet");
	// Check for compile time statements ?? 
	else    return parse_var_def(parser);
	return nullptr;
}

Ast_Node* 
parse_statement_expression(Parser& parser)
{
	auto& lexer = parser.lexer;
	Token t1 = peek_token(lexer);// ident 
	Token t2 = peek_token(lexer, 1);
	
	if (t1.type == TOKEN_IDENT && t2.type == TOKEN_DOUBLE_COLON)
	{
		// Constant Variables, Structs, Procs, Flags, Enums
		return parse_const_def(parser);
	}
	
	if (t1.type == TOKEN_IDENT && (t2.type == TOKEN_COLON ||  t2.type == TOKEN_COLON_EQUAL))
	{
		// Variable Declarations 
		return parse_var_def(parser);
	}
	
	
	// Statements
	Ast_Node* exp = parse_expression(parser);
	expect_and_eat(TOKEN_SEMI_COLON);
	return exp;
}

Ast_Node*
parse_if_statement(Parser& parser)
{
	auto& lexer = parser.lexer;
	// Determine If it's a normal 'if' statement or switch 'statement' 
	
	AllocateNode(Ast_If, statement);
	statement->token = eat_token(lexer);
	
	expect_and_eat('(');
	
	statement->exp = parse_expression(parser);
	
	expect_and_eat(')');
	
	statement->statement = parse_statement(parser);
	
	Token token = peek_token(lexer);
	if (token.type == TOKEN_KEYWORD){ // this is faster than string compare, this way we maybe saving some time ??... I doubt that, but I'm gonna keep it :)
		if (EqualStrings(token.name, "else"_s)) {
			eat_token(lexer);
			arrput(statement->next, parse_statement(parser));
			//statement->next = parse_statement();
		}
	}
	
	return statement;
}

Ast_Node*
parse_statement(Parser& parser)
{
	auto& lexer = parser.lexer;
	Ast_Node* return_node = nullptr;
	
	
	Token t1 = peek_token(lexer);// ident 
	//Token t2 = lexer.peek_token(1);
	
	// TODO: Handle comments 
	if (t1.type == TOKEN_COMMENT || t1.type == TOKEN_MULTILINE_COMMENT){
		eat_token(lexer);
		return nullptr;
	}
	
	// TODO: Handle Compile-time directives
	if (t1.type == TOKEN_DIRECTIVE) {
		return parse_directive(parser);
	}
	
	// Scopes
	if (t1.type == '{'){
		return parse_block_of_statements(parser);
	}
	
	
	if (t1.type == TOKEN_KEYWORD) {
		if (EqualStrings(t1.name, "if"_s)) {
			return parse_if_statement(parser);
		}
		
		if (EqualStrings(t1.name, "return"_s)){
			AllocateNode(Ast_Return, _return);
			_return->token = eat_token(lexer);
			
			t1 = peek_token(lexer);
			while (t1.type != TOKEN_SEMI_COLON)
			{
				arrput(_return->expressions, (Ast_Expression*)parse_expression(parser)); 
				t1 = peek_token(lexer);
				if (t1.type != TOKEN_SEMI_COLON) expect_and_eat(',');
			}
			
			expect_and_eat(TOKEN_SEMI_COLON);
			return _return;
		}
	}
	return parse_statement_expression(parser);
	
	
	
}

// TODO: Merge token_to_operation with get_binary_precedence
static S64 token_to_operation(const Token& token){
	
	switch(token.type)
	{
		case '+': return AST_BINARY_PLUS;
		case '-': return AST_BINARY_MINUS;
		case '*': return AST_BINARY_MUL;
		case '/': return AST_BINARY_DIV;
		case '=': return AST_BINARY_ASSIGN;
		case TOKEN_EQL: return AST_BINARY_IS_EQL;
		default:
		{
			printf("Unknown Operator [%.*s]\n", SV_PRINT(token.name));
			assert(false && "Unknown Operator");
			break;
		}
	}
	
	return -1;
}

static S8 get_binary_precedence(const Token& token)
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
		case TOKEN_EQL:
		case TOKEN_GT:
		case TOKEN_LT:
		case TOKEN_GT_OR_EQL:
		case TOKEN_LT_OR_EQL:
		{
			return 8;
		}
		case '=':
		{
			return 1;
		}
	}
	
	
	return 0;
}

Ast_Expression* 
parse_expression(Parser& parser, S8 priority /* = -1*/ )
{
	auto& lexer = parser.lexer;
	Ast_Expression* left = parse_unary_expression(parser);
	
	
	while (true)
	{
		S8 new_priority = get_binary_precedence(peek_token(lexer)); 
		
		if (new_priority == 0 || new_priority <= priority)
		{
			return left;
		}
		
		AllocateNode(Ast_Binary, binary);
		binary->token = eat_token(lexer);
		binary->op = token_to_operation(binary->token);
		binary->left = left;
		binary->right = parse_expression(parser, new_priority);
		
		
		left = binary;
		
	}
}

Ast_Expression* 
parse_unary_expression(Parser& parser)
{
	auto& lexer = parser.lexer;
	auto token = peek_token(lexer);
	
	Ast_Expression* result = nullptr;
	if (token.type == '(') // factors
	{
		eat_token(lexer); // '('
		result = parse_expression(parser);
		expect_and_eat(')');
		
		return parse_suffix_expression(parser, result);
	}
	else if (token.type == '-' || token.type == '+' ) // minus 
	{
		eat_token(lexer); //  eat the sign
		AllocateNode(Ast_Unary, unary);
		unary->token = token;
		unary->child = parse_suffix_expression(parser, parse_primary_expression(parser));
		return unary;
	}
	else if (token.type== '*') // addressof 
	{
		assert(false && "Pointers are not supported yet");
		
	}
	
	
	return parse_suffix_expression(parser, parse_primary_expression(parser));
}

Ast_Expression* 
parse_suffix_expression(Parser& parser , Ast_Expression* prev)
{
	auto& lexer = parser.lexer;
	Token token = peek_token(lexer);
	
	// early exit 
	if (token.type == TOKEN_SEMI_COLON){
		return prev; 
	}
	
	if (token.type == '('){
		eat_token(lexer);
		
		
		AllocateNode(Ast_Proc_Call, proc_call);
		proc_call->token = token;
		
		Token next = peek_token(lexer);
		while(next.type != ')')
		{
			Ast_Expression* exp = parse_expression(parser);
			arrput(proc_call->arguments, exp);
			next = peek_token(lexer);
			if (next.type != ')') expect_and_eat(',');
		}
		
		expect_and_eat(')');
		proc_call->procedure = prev;
		
		return parse_suffix_expression(parser, proc_call);
	}
	
	if (token.type == '['){
		eat_token(lexer);
		AllocateNode(Ast_Subscript, sub);
		sub->token = token;
		sub->value= parse_expression(parser);
		sub->exp= prev;
		
		expect_and_eat(']');
		
		return parse_suffix_expression(parser, sub);
	}
	
	if (token.type == '.'){
		eat_token(lexer);
		AllocateNode(Ast_Member_Access, mem_acc);
		mem_acc->token = token;
		
		if (peek_token(lexer).type != TOKEN_IDENT) {
			printf("Identifer Must be.. \n");
			exit(-1);
		}
		
		mem_acc->_struct = (Ast_Primary*)prev;
		mem_acc->member = eat_token(lexer);
		return parse_suffix_expression(parser, mem_acc);
	}
	
	
	return prev; 
}

Ast_Expression* 
parse_primary_expression(Parser& parser)
{
	auto& lexer = parser.lexer;
	auto token = eat_token(lexer);
	
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
			printf("Token [%.*s] At [Line: %d, Col: %d], Is not a primary expression [%.*s] \n",SV_PRINT(token.name), token.start_position.line,token.start_position.index
				   , SV_PRINT(token_type_to_string(token.type)));
			exit(-1);
			break;
		}
	}
	
	return nullptr;
}

Ast_Block*
parse_block_of_statements(Parser& parser)
{
	expect_and_eat('{');
	Ast_Block* statements = parse_block(parser);
	expect_and_eat('}');
	return statements;
}

Ast_Block*
parse_block(Parser& parser)
{
	auto& lexer = parser.lexer;
	enter_scope(parser);
	AllocateNode(Ast_Block, block);
	block->scope = parser.current_scope; 
	
	Token token = peek_token(lexer);
	// TODO:  will this break ?? 
	block->token = create_token(peek_token(lexer).name);
	
	while(token.type != TOKEN_EOFA && token.type != '}')
	{
		Ast_Node* statement = parse_statement(parser);
		
		if (statement) arrput(block->statements, statement);
		
		token = peek_token(lexer);
	}
	
	exit_scope(parser);
	return block;
}


Ast_Node* 
parse_directive(Parser& parser)
{
	auto& lexer = parser.lexer;
	Token directive = eat_token(lexer);
	if (EqualStrings(directive.name, "#import"_s))
	{
		Token filename = eat_token(lexer);
		
		if (filename.type != TOKEN_LITERAL &&
			filename.kind != TOKEN_KIND_STRING_LITERAL)
		{
			SyntaxError(filename, "\"Import\" Directive Expect a string literal"_s);
		}
		
		Token next = peek_token(lexer);
		
		AllocateNode(Ast_Directive_Import, import);
		import->token = directive;
		import->filename = filename;
		
		if (EqualStrings(next.name, "as"_s))
		{
			eat_token(lexer);
			import->isAs = true;
			import->as = next;
		}
		
		// Add filename to parsing queue :) 
		return import;
	}
	
	
	
	
	printf("%.*s Directive is not Supported Yet\n", SV_PRINT(directive.name) );
	assert(false);
	
	return nullptr;
}

