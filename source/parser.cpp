/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "pch.h"
#include "parser.h"
#include "lex.h"

// @TEMP
#include <extra/graph.h>
//

#ifdef PARSE_COMMAND
#error "PARSE_COMMAND Is Already Defined"
#else
#define PARSE_COMMAND(name) name(LexerState* lexer, Logger* logger)
#endif

Arena ast_arena = make_arena(1024 * 1024); // 1MB ?? 



inline Ast_Binary*
PARSE_COMMAND(parse_operator)
{
	Token token = lexer->eat_token();
	
	
	u32 op = 0; 
	switch(token.Type)
	{
		case '+':
		case '-':
		{
			op = 1;
			break;
		}
		case '*':
		case '/':
		{
			op = 2;
			break;
		}
		case ';':
		{
			return nullptr;
		}
		default: 
		{
			logger->print_with_location(&token, "Unexpected Token [%s] :"_s, token.name);
			abort();
			break;
		}
	}
	
	Ast_Binary* exp = allocate<Ast_Binary>(&ast_arena);
	exp->token = token;
	exp->op = op;
	return exp;
	
}

static Ast_Node*
PARSE_COMMAND(parse_subexpression)
{
	
	Token token = lexer->eat_token();
	switch(token.Type){
		case TOKEN_IDENT:
		case TOKEN_LITERAL:
		{
			Ast_Literal* literal = allocate<Ast_Literal>(&ast_arena);
			literal->token = token;
			return literal;
			break;
		}
		case '!':
		case '-':
		{
			Ast_Unary* unary = allocate<Ast_Unary>(&ast_arena);
			//unary->op = token.Type;
			unary->token = token;
			unary->child = parse_subexpression(lexer,logger);
			return unary;
		}
		default: 
		{
			logger->print_with_location(&token, "Unexpected Token [%s] :"_s, token.name);
			abort();
			break;
		}
	}
}

static Ast_Node*
PARSE_COMMAND(parse_expression)
{
	Ast_Node* left = parse_subexpression(lexer, logger);
	if (!left) return nullptr;
	
	Ast_Binary* exp = parse_operator(lexer, logger);
	if (!exp) return left;
	
	Ast_Node* right = parse_expression(lexer, logger);
	if (!right) return nullptr;;
	
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
PARSE_COMMAND(parse_directive)
{
	return nullptr;
}

static String 
get_operator_name(const Token& token) {
	return "PLUS"_s;
}

// @Temp
static Array<Graph_Label> labels = init_array<Graph_Label>(50);
//static u64 graph_labels_counter = 0;
//

static void 
output_labels(Logger* logger)
{
	for(u64 i = 0; i < labels.occupied; i++)
	{
		auto& label = *labels[i];
		logger->print("T_%x [shape=Mdiamond, label=\"%s\"];\n"_s, label.hash, label.str);
		//logger->print();
	}
}

// Make this more sophisticated 
static void
output_graph(Ast_Node* node, Logger* logger)
{
	
	if (node == nullptr)
		return;
	
	if (node->type == AST_BINARY_EXP){
		Ast_Binary* bin = (Ast_Binary*)node;
		
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(Ast_Binary), bin);
		
		
		if (bin->left != nullptr){
			//if (bin->left->type == AST_LITERAL)
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->left, logger);
		}
		
		array_add<Graph_Label>(&labels, { MeowU32From(hash, 3), Shape_Type::NORMAL,
								   bin->token.name });
		if (bin->right != nullptr){
			//if (bin->right->type == AST_LITERAL)
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->right, logger);
		}
		
		
		
	} else if (node->type == AST_LITERAL) {
		Ast_Literal * literal = (Ast_Literal*) node;
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*literal), literal);
		logger->print("T_%x\n"_s, MeowU32From(hash_left, 3));
		array_add<Graph_Label>(&labels, { MeowU32From(hash_left, 3), Shape_Type::NORMAL, literal->token.name});
		
	} else if (node->type == AST_UNARY_EXP) {
		Ast_Unary * unary = (Ast_Unary*) node;
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*unary), unary);
		
		
		if (unary->child != nullptr){
			//if (bin->left->type == AST_LITERAL)
			logger->print("T_%x -> "_s, MeowU32From(hash_left, 3));
			output_graph(unary->child, logger);
		}
		
		array_add<Graph_Label>(&labels, { MeowU32From(hash_left, 3), Shape_Type::NORMAL, unary->token.name});
		
	} else if (node->type == AST_ASSIGN) {
		Ast_Assign* assign =  (Ast_Assign*) node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*assign), assign);
		
		output_graph(assign->ident, logger);
		logger->print("-> T_%x\n"_s, MeowU32From(hash, 3));
		
		logger->print("T_%x -> "_s, MeowU32From(hash, 3));
		output_graph(assign->exp, logger);
		
		array_add<Graph_Label>(&labels, { MeowU32From(hash, 3), Shape_Type::NORMAL, assign->token.name});
		
	}else if (node->type == AST_IDENT) {
		Ast_Ident* ident = (Ast_Ident*) node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*ident), ident);
		
		logger->print("T_%x "_s, MeowU32From(hash, 3));
		array_add<Graph_Label>(&labels, { MeowU32From(hash, 3), Shape_Type::NORMAL, ident->token.name});
		
	}
	
}

#define AllocateNode(type, name) type* name =  allocate<type>(&ast_arena);

static Ast_Node*
PARSE_COMMAND(parse_statement)
{
	//Ast_Declaration* decl = allocate<Ast_Declaration>(&ast_arena);
	Token ident_token = lexer->eat_token();
	//decl->name = token.name;
	
	Token token = lexer->peek_next_token();
	switch(token.Type)
	{
		case TOKEN_DOUBLECOLON: 
		{
			// Constatnts, Function and Lambda eventually
			//decl->decl_type = "FUNCTION OR CONSTANT"_s;
			break;
		}
		case TOKEN_ASSIGN:
		{
			AllocateNode(Ast_Assign, assign);
			assign->token = lexer->eat_token();
			
			AllocateNode(Ast_Ident, ident);
			ident->token = ident_token;
			
			Ast_Node* node = parse_expression(lexer, logger); 
			
			assign->ident = ident;
			assign->exp = node;
			
			return assign;
			//decl->decl_type = "NOT FUNCTION NOR  CONSTANT"_s;
			break;
		}
		
	}
	
	//logger->print("%s \n"_s, decl->decl_type);
	
	//token = lexer->eat_token();
	
	//return decl;
	return nullptr;
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
#if 0
			case TOKEN_LITERAL:
			// @NOCHECKIN
			case '-':
			case '!':
#endif
			{
				logger->print("Token Name = [%s]\n"_s, token.name);
				
				
				Ast_Node* node = parse_statement(lexer, logger);
				
				// @CleanUp(Husam): reset the array ... Do we need a "method for this";
				labels.occupied = 0;
				output_graph(node, logger);
				output_labels(logger);
				//Ast_Node* node = parse_declaration(lexer, logger);
				/*Ast_Node* node  = parse_expression(lexer, logger
);
*/
				//lexer->eat_token();
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




