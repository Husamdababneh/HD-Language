/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/


#include "parser.h"
#include "Ast.cpp"

#include <memory>

#define ENABLE_GRAPH_PRINTING 1
#define GRAPH_SHOW_BLOCK_PARENT 0
#include <extra/graph.cpp>


#include "generator.cpp"

Ast_Node** flaten_ast = NULL;


#include "typer.cpp"

// Forward declarations

Ast parse(MemoryArena*, Parser& );
Ast_Node* parse_statement(MemoryArena* arena, Parser& );
Ast_Node* parse_const_def(MemoryArena* arena, Parser& );
Ast_Type* parse_type(MemoryArena* arena, Parser& );
Ast_Node* parse_directive(MemoryArena* arena, Parser& );
Ast_Node* parse_if_statement(MemoryArena* arena, Parser& );
Ast_Var_Declaration* parse_var_def(MemoryArena* arena, Parser&, B8);
Ast_Proc_Declaration* parse_proc_def(MemoryArena* arena, Parser& );
Ast_Node* parse_statement_expression(MemoryArena* arena, Parser& );
Ast_Block* parse_block_of_statements(MemoryArena* arena, Parser& );
Ast_Proc_Declaration* parse_proc_header(MemoryArena* arena, Parser& );
Ast_Var_Declaration* parse_argument_def(MemoryArena* arena, Parser& );
Ast_Struct_Declaration* parse_struct_def(MemoryArena* arena, Parser& );

Ast_Expression* parse_expression(MemoryArena* arena, Parser&, S8 priority = -1);
Ast_Expression* parse_suffix_expression(MemoryArena* arena, Parser&, Ast_Expression* prev);
Ast_Expression* parse_unary_expression(MemoryArena* arena, Parser& );
Ast_Expression* parse_primary_expression(MemoryArena* arena, Parser& );

// TODO: Re-read the Tracy documentation to use this in the right way
#include "../submodules/tracy/Tracy.hpp"


// How does this perform ?? 

//name = (type*) PushStruct(arena, type);\
//name = ::new(name) type();\

// NOTE(Husam): I'm not sure about the non-allocating palcement 'new' operator above
//              I need to know how fast it is and is it worth it to create 'CreateNode' method
//              for each Node type ?? ... that would be cumbersome but at least we know how fast it is 
// NOTE(Husam): If we choose to do the cumbersome way, is it possible to creat meta program to help us 
//              with the task ? 


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
Token _expect_and_eat(Parser* _this, U64 type, U64 line, const char* file)
{
	const Token& token = peek_token(_this->lexer);
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", file ,line);
		
		String token_type = "TODO : "_s;
		// auto token_type = token_type_to_string(type); 
		if(token_type.length <= 1){
			printf("Expected: %.*s(\'%c\', %d)  got \"%.*s\" \n", SV_PRINT(token_type), (char)type, (int)type, SV_PRINT(token.name));
		}else{
			printf("Expected: %.*s(%d)  got \"%.*s\" \n", SV_PRINT(token_type), (int)type, SV_PRINT(token.name));
		}
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.line, token.start_position.index);
		
		SyntaxError(token, "Expected Another Token Here"_s);
		exit(-1);
	}
	return eat_token(_this->lexer);
	
}
#define expect_and_eat(x) _expect_and_eat(&parser, x, __LINE__, __FILE__)


inline Ast_Scope*
enter_scope(MemoryArena* arena, Parser& parser)
{
	Ast_Scope* prev = parser.current_scope;
	AllocateNode(Ast_Scope, current_scope);
	
	if (prev != nullptr){
		arrput(prev->children, current_scope);
	}
	
	current_scope->parent = prev;
	parser.current_scope = current_scope;
	
	return current_scope;
}


inline void
exit_scope(MemoryArena* arena, Parser& parser)
{
	assert(parser.current_scope);
	parser.current_scope = parser.current_scope->parent;
}

//Predefined_Type* Parser::predefined_types = nullptr;

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
}





#define my_assert(x) if(!x) {*(int*)0 = 0;}

Ast parse(MemoryArena* arena, Parser& parser)
{
	
	//bool still_parsing = true;
	
	
	enter_scope(arena, parser);
	AllocateNode(Ast_Block, block);
	block->scope = parser.current_scope; 
	
	Token token = {};
	token.type = TOKEN_IDENT;
	token.name = parser.lexer.filename;
	token.hash = MHash(token);
	// TODO:  will this break ?? 
	block->token = token; //create_token(peek_token(lexer).name);
	
	while(token.type != TOKEN_EOFA && token.type != '}')
	{
		Ast_Node* statement = parse_statement(arena, parser);
		
		if (statement) arrput(block->statements, statement);
		
		token = peek_token(parser.lexer);
	}
	
	exit_scope(arena, parser);
	
	
	type_check_node(block);
	//PRINT_GRAPH(block);
	
	return {0};
}

Ast_Type*
parse_type(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	Token token = peek_token(lexer);
	
	expect_and_eat(TOKEN_IDENT); // TODO: Add message if failed
	
	Ast_Type* type_from_map = get_type_from_map(token.name);

	if (type_from_map != nullptr) return type_from_map;
	
	AllocateNode(Ast_Type, type);
	type->token = token;
	return type;
	
}

Ast_Var_Declaration*
parse_argument_def(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	AllocateNode(Ast_Var_Declaration, decl);
	Token token = eat_token(lexer);
	if (token.type != TOKEN_IDENT)
	{
		UNREACHABLE;
		exit(-1);
	}
	
	decl->token = token;
	
	expect_and_eat(TOKEN_COLON);
	decl->data_type = parse_type(arena, parser);
	
	token = peek_token(lexer);
	if (token.type == '='){
		eat_token(lexer);
		decl->body = parse_expression(arena, parser);
	}
	
	
	arrput(parser.current_scope->variables, decl);
	return  decl;
}


// TODO(Husam Dababneh): Rethink this part
Ast_Proc_Declaration* 
parse_proc_header(MemoryArena* arena, Parser& parser) 
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
		//Ast_Var_Declaration* var = parse_argument_def(parser);
		token = peek_token(lexer);
		if (token.type != ')') expect_and_eat(',');
	}
	
	expect_and_eat(')');
	
	// Parse Return Types
	if (peek_token(lexer).type == TOKEN_ARROW){
		eat_token(lexer); 
		token = peek_token(lexer);
		while(token.type != '{') {
			Ast_Type* type = parse_type(arena, parser);
			arrput(proc->return_type, type);
			token = peek_token(lexer);
			if (token.type != '{') expect_and_eat(',');
		}
	}
	return proc;
}

Ast_Proc_Declaration* 
parse_proc_def(MemoryArena* arena, Parser& parser)
{
	Ast_Proc_Declaration* proc = nullptr;
	proc = parse_proc_header(arena, parser);
	proc->body = parse_block_of_statements(arena, parser);
	arrput(parser.current_scope->procedures, proc);
	return proc;
}

Ast_Var_Declaration*
parse_var_def(MemoryArena* arena, Parser& parser, B8 addToScope = true)
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
		type = parse_type(arena, parser);
		token = peek_token(lexer);
		var->data_type = type;
		
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
		
		printf("decl_name[%.*s], Token[%.*s], Line [%d], Index [%d]\n",
			   SV_PRINT(decl_name.name),
			   SV_PRINT(token.name),
			   token.start_position.line,
			   token.start_position.index);
		
		//exit(-1);
		my_assert(false);
	}
	
	
	
	token = peek_token(lexer);
	
	// If it's a constant it have to have a value; 
	if (!var->constant && token.type == TOKEN_SEMI_COLON)  
		goto exit;
	
	var->body = parse_expression(arena, parser);
	my_assert(var->body);
	
	
  exit:
	if (addToScope) arrput(parser.current_scope->variables, var);
	expect_and_eat(TOKEN_SEMI_COLON);
	return var;
}


Ast_Struct_Declaration* 
parse_struct_def(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	Token decl_name = eat_token(lexer); // Name
	expect_and_eat(TOKEN_DOUBLE_COLON);
	Token token = eat_token(lexer); // eat 'struct'
	
	enter_scope(arena, parser);
	AllocateNode(Ast_Struct_Declaration, _struct);
	_struct->token = decl_name;
	_struct->scope = parser.current_scope;
	
	expect_and_eat('{');
	
	
	token = peek_token(lexer); 
	while(token.type != '}' && token.type != TOKEN_EOFA)
	{
		Ast_Var_Declaration* var = parse_var_def(arena, parser, false);
		arrput(_struct->decls, var);
		token = peek_token(lexer);
		if (token.type != '}') continue;
	}
	
	expect_and_eat('}');
	exit_scope(arena, parser);
	
	// Add the new type to current scope
	arrput(parser.current_scope->structs, _struct);
	
	// Add Struct to types
	
	//hmput(parser.predefined_types, decl_name.name.str_char, create_type(0,0,0,decl_name));
	
	//arrput(current_scope->types, type);
	
	return _struct;
}



Ast_Node* 
parse_const_def(MemoryArena* arena, Parser& parser)
{
	auto lexer = parser.lexer;
	Token token = peek_token(lexer, 2); 
	if      (EqualStrings(token.name, "proc"_s)) return parse_proc_def(arena, parser);
	else if (EqualStrings(token.name, "struct"_s)) return parse_struct_def(arena, parser);
	// NOTE (Husam): I need to justify the existance of this aproch
	else if (EqualStrings(token.name, "template"_s)) assert(false && "Templates are Not Supported Yet");
	else if (EqualStrings(token.name, "enum"_s)) assert(false && "Enums are Not Supported Yet");
	else if (EqualStrings(token.name, "flag"_s)) assert(false && "Flags are Not Supported Yet");
	// Check for compile time statements ?? 
	else    return parse_var_def(arena, parser);
	return nullptr;
}

Ast_Node* 
parse_statement_expression(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	Token t1 = peek_token(lexer);// ident 
	Token t2 = peek_token(lexer, 1);
	
	if (t1.type == TOKEN_IDENT && t2.type == TOKEN_DOUBLE_COLON)
	{
		// Constant Variables, Structs, Procs, Flags, Enums
		return parse_const_def(arena, parser);
	}
	
	if (t1.type == TOKEN_IDENT && (t2.type == TOKEN_COLON ||  t2.type == TOKEN_COLON_EQUAL))
	{
		// Variable Declarations 
		return parse_var_def(arena, parser);
	}
	
	
	// Statements
	Ast_Node* exp = parse_expression(arena, parser);
	expect_and_eat(TOKEN_SEMI_COLON);
	return exp;
}

Ast_Node*
parse_if_statement(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	// Determine If it's a normal 'if' statement or switch 'statement' 
	
	AllocateNode(Ast_If, statement);
	statement->token = eat_token(lexer);
	
	expect_and_eat('(');
	
	statement->exp = parse_expression(arena, parser);
	
	expect_and_eat(')');
	
	statement->statement = parse_statement(arena, parser);
	
	Token token = peek_token(lexer);
	if (token.type == TOKEN_KEYWORD){ // this is faster than string compare, this way we maybe saving some time ??... I doubt that, but I'm gonna keep it :)
		if (EqualStrings(token.name, "else"_s)) {
			eat_token(lexer);
			arrput(statement->next, parse_statement(arena, parser));
			//statement->next = parse_statement();
		}
	}
	
	return statement;
}

Ast_Node*
parse_statement(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	//Ast_Node* return_node = nullptr;
	
	
	Token t1 = peek_token(lexer);// ident 
	//Token t2 = lexer.peek_token(1);
	
	// TODO: Handle comments 
	if (t1.type == TOKEN_COMMENT || t1.type == TOKEN_MULTILINE_COMMENT){
		eat_token(lexer);
		return nullptr;
	}
	
	// TODO: Handle Compile-time directives
	if (t1.type == TOKEN_DIRECTIVE) {
		return parse_directive(arena, parser);
	}
	
	// Scopes
	if (t1.type == '{'){
		return parse_block_of_statements(arena, parser);
	}
	
	
	if (t1.type == TOKEN_KEYWORD) {
		if (EqualStrings(t1.name, "if"_s)) { 
			return parse_if_statement(arena, parser);
		}
		
		if (EqualStrings(t1.name, "return"_s)){
			AllocateNode(Ast_Return, _return);
			_return->token = eat_token(lexer);
			
			t1 = peek_token(lexer);
			while (t1.type != TOKEN_SEMI_COLON)
			{
				arrput(_return->expressions, (Ast_Expression*)parse_expression(arena, parser)); 
				t1 = peek_token(lexer);
				if (t1.type != TOKEN_SEMI_COLON) expect_and_eat(',');
			}
			
			expect_and_eat(TOKEN_SEMI_COLON);
			return _return;
		}
	}
	return parse_statement_expression(arena, parser);
	
	
	
}

// TODO: Merge token_to_operation with get_binary_precedence
static AST_BINARY_TYPE token_to_operation(const Token& token){
	
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
	
	return AST_BINARY_NONE;
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
parse_expression(MemoryArena* arena, Parser& parser, S8 priority /* = -1*/ )
{
	auto& lexer = parser.lexer;
	Ast_Expression* left = parse_unary_expression(arena, parser);
	
	
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
		binary->right = parse_expression(arena, parser, new_priority);
		
		
		left = binary;
		
	}
}

Ast_Expression* 
parse_unary_expression(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	auto token = peek_token(lexer);
	
	Ast_Expression* result = nullptr;
	if (token.type == '(') // factors
	{
		eat_token(lexer); // '('
		result = parse_expression(arena, parser);
		expect_and_eat(')');
		
		return parse_suffix_expression(arena, parser, result);
	}
	else if (token.type == '-' || token.type == '+' || token.type == '!')
	{
		eat_token(lexer); //  eat the sign
		AllocateNode(Ast_Unary, unary);
		unary->token = token;
		unary->child = parse_suffix_expression(arena, parser, parse_primary_expression(arena, parser));
		return unary;
	}
	else if (token.type== '*') // addressof 
	{
		assert(false && "Pointers are not supported yet");
		
	}
	
	auto primary = parse_primary_expression(arena, parser);
	return parse_suffix_expression(arena, parser, primary);
}

Ast_Expression* 
parse_suffix_expression(MemoryArena* arena, Parser& parser , Ast_Expression* prev)
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
			Ast_Expression* exp = parse_expression(arena, parser);
			arrput(proc_call->arguments, exp);
			next = peek_token(lexer);
			if (next.type != ')') expect_and_eat(',');
		}
		
		expect_and_eat(')');
		proc_call->procedure = prev;
		
		return parse_suffix_expression(arena, parser, proc_call);
	}
	
	if (token.type == '['){
		eat_token(lexer);
		AllocateNode(Ast_Subscript, sub);
		sub->token = token;
		sub->value= parse_expression(arena, parser);
		sub->exp= prev;
		
		expect_and_eat(']');
		
		return parse_suffix_expression(arena, parser, sub);
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
		return parse_suffix_expression(arena, parser, mem_acc);
	}
	
	
	return prev; 
}

Ast_Expression* 
parse_primary_expression(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	auto token = eat_token(lexer);
	
	switch(token.type)
	{
	  case TOKEN_IDENT:  // this means we are refering to a value or a type
	  {
		  AllocateNode(Ast_Primary, primary);
		  primary->token = token;
		  primary->kind = AST_KIND_PRIMARY_IDENTIFIER;
		  return primary;
	  }
	  case TOKEN_LITERAL: 
	  {
		  AllocateNode(Ast_Literal, literal);

		  // TODO: HANDLE this from the lexer side?? 
		  if (token.kind == TOKEN_KIND_STRING_LITERAL) {
			  literal->kind = AST_KIND_LITERAL_STRING;
			  literal->resulting_type = _string;
		  }
		  else {
			  literal->kind = AST_KIND_LITERAL_NUMBER;
			  literal->resulting_type = _S64;
		  }
		  literal->token = token;
		  
		  return literal;
	  }
	  case TOKEN_BOOLEAN:
	  {
		  printf("TODO: Handel true/false constants ");
		  exit(-1);
		  break;
	  }
	  default:
	  {
		  String token_type = "TODO: "_s;
		  printf("Token [%.*s] At [Line: %d, Col: %d], Is not a primary expression [%.*s] \n",
				 SV_PRINT(token.name),
				 token.start_position.line,
				 token.start_position.index,
				 SV_PRINT(token_type));
		  exit(-1);
		  break;
	  }
	}
	
	// return nullptr;
}

Ast_Block*
parse_block_of_statements(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	
	Token token = expect_and_eat('{');
	enter_scope(arena, parser);
	
	AllocateNode(Ast_Block, block);
	
	block->token = token;
	block->scope = parser.current_scope; 
	
	token = peek_token(lexer);
	
	while(token.type != TOKEN_EOFA && token.type != '}')
	{
		Ast_Node* statement = parse_statement(arena, parser);
		
		if (statement) arrput(block->statements, statement);
		
		token = peek_token(lexer);
	}
	
	exit_scope(arena, parser);
	expect_and_eat('}');
	return block;
}

Ast_Node* 
parse_directive(MemoryArena* arena, Parser& parser)
{
	auto& lexer = parser.lexer;
	Token directive = eat_token(lexer);
	if (EqualStrings(directive.name, "#import"_s)) // Replace this with Enum
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

