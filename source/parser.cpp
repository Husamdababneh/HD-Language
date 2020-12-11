/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "pch.h"
#include "parser.h"
#include "lex.h"

#ifdef PARSE_COMMAND
#error "PARSE_COMMAND Is Already Defined"
#else
#define PARSE_COMMAND(name) name(LexerState* lexer, Logger* logger)
#endif

Arena ast_arena = make_arena(1024 * 1024); // 1MB ?? 

static void
PrintHash(meow_u128 Hash)
{
    printf("%08X-%08X\n",
		   MeowU32From(Hash,1),
		   MeowU32From(Hash,0));
	
}


static Ast_Declaration
PARSE_COMMAND(parse_declaration)
{
	Ast_Declaration result = {};
	auto token = lexer->eat_token();
	
	
	if (token.Type != TOKEN_IDENT)
		logger->print_with_location(&token, "Error: expected an identifer\n"_s);
	
	result.name = token.name;
	
	token = lexer->eat_token();
	if (token.Type != TOKEN_COLON)
		logger->print_with_location(&token, "Error: expected ':'\n"_s);
	
	token = lexer->eat_token();
	// this is wrong we need split the keywords to "type keyword" and  "language keyword"
	if (token.Type != TOKEN_IDENT &&  token.Type != TOKEN_KEYWORD)
		logger->print_with_location(&token, "Error: expected a Type\n"_s);
	result.type = token.name;
	
	
	token = lexer->peek_token();
	if (token.Type == TOKEN_ASSIGN){
		lexer->eat_token();
		token = lexer->eat_token(); // this token should be known before assebmleing 
		result.value = token.name;
	}
	
	return result;
}

static Ast_Declaration 
PARSE_COMMAND(parse_caller_argument)
{
	Ast_Declaration result = {} ;
	auto token = lexer->eat_token();
	
	if (token.Type == TOKEN_IDENT){
		result.name = token.name;
		if (lexer->peek_token().Type == TOKEN_ASSIGN){
			lexer->eat_token();
			token = lexer->peek_token();
			if (token.Type == TOKEN_IDENT ||
				token.Type == TOKEN_KEYWORD ||
				token.Type == TOKEN_LITERAL)
			{
				lexer->eat_token();
				result.type = token.name;
				return result;
			}
		}
	}
	
	return {};
}

static Ast_ParmeterList 
PARSE_COMMAND(parse_caller_arguemnts_list)
{
	
	Ast_ParmeterList list = {};
	auto token = lexer->peek_token();
	if (token.Type == ')')
		return list;
	
	list.declerations = init_array<Ast_Declaration>();	
	u8 argumentIndex = 0;
	while(true)
	{
		Ast_Declaration dec = parse_caller_argument(lexer,logger);
		if (dec.name.data == nullptr)
			break;
		array_add<Ast_Declaration>(&list.declerations, dec);
		
		token = lexer->eat_token();
		if (token.Type == ','){	
			continue;
		}
		else if (token.Type == ')') {
			break;
		}
		else {
			logger->print_with_location(&token, "Error: unexpected token %s\n"_s ,token.name);
			exit(-1);
		}
		
	}
	return list;
}

static Ast_ParmeterList 
PARSE_COMMAND(parse_declaration_list)
{
	
	Ast_ParmeterList list = {};
	auto token = lexer->peek_token();
	if (token.Type == ')')
		return list;
	
	if (token.Type != '('){
		logger->print_with_location(&token, "Error: expected '('\n"_s);
	}
	
	lexer->eat_token();
	list.declerations = init_array<Ast_Declaration>();
	
	u8 argumentIndex = 0;
	while(true)
	{
		Ast_Declaration dec = parse_declaration(lexer,logger);
		if (dec.name.data == nullptr)
			break;
		array_add<Ast_Declaration>(&list.declerations, dec);
		
		token = lexer->eat_token();
		if (token.Type == ','){	
			continue;
		}
		else if (token.Type == ')') {
			break;
		}
		else {
			logger->print_with_location(&token, "Error: unexpected token %s\n"_s ,token.name);
			exit(-1);
		}
		
	}
	return list;
}



static Ast_Node*
PARSE_COMMAND(parse_expression)
{	
	Ast_Node* node = nullptr;
	
	switch(lexer->peek_token().Type)
	{
		case '(':
		{
			//node.hasBracets = true;
			lexer->eat_token();
			//return parse_expression(lexer,logger);
			break;
		}
		case TOKEN_LITERAL: 
		case TOKEN_STRING_LITERAL:
		case TOKEN_FLOAT_LITERAL:
		case TOKEN_IDENT:
		{
			auto ident = lexer->eat_token();
			auto next = lexer->eat_token();
			if (next.Type == TOKEN_DOUBLECOLON){
				
			}
			else  if (next.Type == TOKEN_COLON){
				node = parse_expression(lexer,logger);
				//if (node.Type != 1A)
			}
			else  if (next.Type == TOKEN_COLONEQUAL){
			}
			else{
			}
			
			
			//return parse_expression(lexer,logger);
			break;
		}
	}
	
	return node;
	
}


#if 0
&&
token.Type != TOKEN_STRING_LITERAL &&
token.Type != TOKEN_INT_LITERAL &&
token.Type != TOKEN_FLOAT_LITERAL
#endif
//#define new_node()
static Ast_Node*
PARSE_COMMAND(parse_subexpression)
{
	
	Token token = lexer->peek_next_token();
	
	if (token.Type == ';'){
		lexer->eat_token();
		return nullptr;
	}
	
	// TODO(Husam): Make it better ... duh!! -_-'
	if (token.Type != TOKEN_LITERAL){
		assert(true);
	}
	
	
	Ast_Literal* left = allocate<Ast_Literal>(&ast_arena);
	left->token = lexer->eat_token();
	
	
	token = lexer->peek_next_token();
	
	if (token.Type == ';'){
		lexer->eat_token();
		return left;
	}
	
	// token.Type == ?? 
	Ast_Binary* exp = allocate<Ast_Binary>(&ast_arena);
	exp->token = lexer->eat_token();
	
	Ast_Node* right = parse_subexpression(lexer, logger);
	
	exp->left = left;
	exp->right = right;
	return exp;
}

static Ast_Node*
PARSE_COMMAND(parse_directive)
{
	return nullptr;
}

static String 
get_operator_name(const Token& token) {
	return "PLUS"_s;
}

static void output_graph(Ast_Node* node, Logger* logger)
{
	if (node == nullptr)
		return;
	
	if (node->type == AST_BINARY_EXP){
		Ast_Binary* bin = (Ast_Binary*)node;
		
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(Ast_Binary), bin);
		
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*bin->left), bin->left);
		logger->print("T_%x -> _%s_%x\n"_s,
					  MeowU32From(hash, 3),
					  bin->left->token.name,
					  MeowU32From(hash_left, 3));
		
		logger->print("T_%x -> "_s, 
					  MeowU32From(hash, 3));
		
		output_graph(bin->right, logger);
		
		logger->print("T_%x [shape=Mdiamond, label=\"+\"];\n"_s,
					  MeowU32From(hash, 3));
		
	}else if (node->type == AST_LITERAL) {
		Ast_Literal * literal = (Ast_Literal*) node;
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*literal), literal);
		logger->print("_%s_%x\n"_s, literal->token.name , MeowU32From(hash_left, 3));
	}
}

static Ast 
PARSE_COMMAND(parse)
{
	int a = 0;
	auto token = lexer->peek_next_token();
	while(token.Type != TOKEN_EOFA){
		switch(token.Type)
		{
			case TOKEN_IDENT:
			{
				logger->print("Token Name = [%s]\n"_s, token.name);
				Ast_Node* node  = parse_subexpression(lexer, logger);
				output_graph(node, logger);
				break;
				
			}
			default:
			{
				// This should be an error eventually.. since we are not really handleing it
				lexer->eat_token();
				break;
			}
		}
		token = lexer->peek_next_token();
	}	
	return {0};
}

void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);
	
	parse(&lexer, &logger);
}





/*
// meow_u128 Hash = MeowHash(MeowDefaultSeed, token.value.count, token.value.data);
				// logger.print("%s hash is :"_s , token.value);
				// PrintHash(Hash);
				auto next = lexer->peek_token();
				if (next.Type == TOKEN_DOUBLECOLON)
				{
					// Functions are considered constants since thet will nerver change
					lexer->eat_token();
					
					//logger.print("Function or Constant Or Struct [%s]\n"_s , token.value);
					next = lexer->peek_token();
					
					// TODO:@Clean: cleanup String class
					if (next.value.isEqual("struct"_s)){
						logger->print("Struct [%s]\n"_s , token.value);
					}
					else if (next.value.isEqual("proc"_s)) {
						logger->print("Function [%s]\n"_s , token.value);
						lexer->eat_token();
						Ast_ParmeterList arguments = parse_declaration_list(lexer, logger);
						for (int a = 0; a < arguments.declerations.occupied; a++){
							logger->print("IDENT: %s is %s\n"_s,
										  arguments.declerations[a]->name,
										  arguments.declerations[a]->type);
						}
					}
					else if (next.Type == '(') {
						
						
					}
				}
				else if (next.Type == TOKEN_COLONEQUAL){
					lexer->eat_token();
					logger->print("Declaration With inferred type ident[%s]\n"_s , token.value);
				}
				else if (next.Type == TOKEN_COLON){
					lexer->eat_token();
					logger->print_with_location(&token, "Possible Declaration ident[%s]"_s , token.value);
					//parse_possible_type();
					
					parse_declaration(lexer, logger);
#if 0
					if(lexer->peek_token().Type == '['){
						lexer->eat_token();
						next = lexer->eat_token();
						logger->print(" type[  [%s]  ]\n"_s , next.value);
						next = lexer->eat_token();
					}
					else{
						next = lexer->eat_token();
						logger->print(" type[%s]\n"_s , next.value);
					}
#endif
					
				}
				else if (next.Type == '('){
					lexer->eat_token();
					logger->print("Function Call [%s]\n"_s , token.value);
					
					Ast_ParmeterList arguments = parse_caller_arguemnts_list(lexer, logger);
					for (int a = 0; a < arguments.declerations.occupied; a++){
						logger->print("IDENT: %s is %s\n"_s,
									  arguments.declerations[a]->name,
									  arguments.declerations[a]->type);
					}
				}
				else{
					// TODO: report errors
				}
*/