/* ========================================================================
   $File: parser.cpp
   $Date: April 23th 2021 2:13 am
   $Creator: Husam Dababneh
   $Description: typer.cpp
   ========================================================================*/

#include "typer.h"

#define UNREACHABLE assert(false && "Unreachable Code");

/*
  NOTEs:

*/

#define MHash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
// TODO: Remove this
Token create_token(StringView name)
{
	static U32 id = 0;
	// This assumes we will only have 3772 scopes, needs to be fixed 
	id = (id + 3772) % 1000; // worst random number generator ever :).
	Token token = {0};
	token.name = name;
	token.start_position = {id+22, id+9};
	token.hash = MHash(token);
	
	return token;
}

inline 
Ast_Type* 
create_type(MemoryArena*arena, 
			const U32 size, 
			const U32 alignment, 
			const bool is_signed, 
			const StringView& name)
{
	AllocateNode(Ast_Type, type);
	type->size = size;
	type->alignment = alignment;
	type->is_signed = is_signed;
	type->token = create_token(name);
	
	return type;
}


inline 
Ast_Type* 
create_type(MemoryArena*arena, 
			const U32 size, 
			const U32 alignment, 
			const bool is_signed, 
			const Token& token)
{
	AllocateNode(Ast_Type, type);
	type->size = size;
	type->alignment = alignment;
	type->is_signed = is_signed;
	type->token = token;
	
	return type;
}


static Type_Map* types   = nullptr;
static Ast_Type* _U8     = nullptr;
static Ast_Type* _U16    = nullptr;
static Ast_Type* _U32    = nullptr;
static Ast_Type* _U64    = nullptr;
static Ast_Type* _S8     = nullptr;
static Ast_Type* _S16    = nullptr;
static Ast_Type* _S32    = nullptr;
static Ast_Type* _S64    = nullptr;
static Ast_Type* _string = nullptr;

static inline void
add_type_to_map(Ast_Type* type)
{
	constexpr U64 MAX_NUMBER = 256;
	char temp[MAX_NUMBER];
	String& type_name = type->token.name;
	memcpy(temp, type_name.str, type_name.length);
	if (!type_name.isNullTerminated) temp[type_name.length] = 0;

	shput(types, temp, type);
}

static inline Ast_Type* 
get_type_from_map(U8* type_name)
{
	auto index = shgeti(types, type_name);

	if (index == -1) printf("Type [%s] cannot be found in the map\n", type_name);
	
	return index == -1 ? nullptr : types[index].value; 
}


static inline Ast_Type* 
get_type_from_map(String type_name)
{
	constexpr U64 MAX_NUMBER = 256;
	U8 temp[MAX_NUMBER];
	memcpy(temp, type_name.str, type_name.length);
	if (!type_name.isNullTerminated) temp[type_name.length] = 0;	
	return get_type_from_map(temp);
}



void 
register_predefined_types(MemoryArena* arena, Parser& parser)
{

	sh_new_arena(types);
	
	_U8 = create_type(arena, 1, 1, false, "U8"_s);
	add_type_to_map(_U8);
	
	_U16 = create_type(arena, 2, 2, false, "U16"_s);
	add_type_to_map(_U16);
	
	_U32 = create_type(arena, 4, 4, false, "U32"_s);
	add_type_to_map(_U32);
	
	_U64 = create_type(arena, 8, 8, false, "U64"_s);
	add_type_to_map(_U64);
	
	_S8 = create_type(arena, 1, 1, true, "S8"_s);
	add_type_to_map(_S8);
	
	_S16 = create_type(arena, 2, 2, true, "S16"_s);
	add_type_to_map(_S16);
	
	_S32 = create_type(arena, 4, 4, true, "S32"_s);
	add_type_to_map(_S32);
	
	_S64 = create_type(arena, 8, 8, true, "S64"_s);
	add_type_to_map(_S64);

	_string = create_type(arena, 16, 16, true, "String"_s);
	add_type_to_map(_string);

	// TODO: Add all predefined types here
	
}



// forward declaration
static void type_check_node(Ast_Node* node); 
static void type_check_block(Ast_Block* block);
static void type_check_declaration(Ast_Declaration* decl);



static Ast_Type* type_expression(Ast_Expression* node)
{

	if (node->resulting_type != nullptr) return node->resulting_type;
	
	switch(node->kind)
	{
	  case AST_KIND_EXP_BINARY:
	  {
		  //printf("Node type is : AST_KIND_EXP_BINARY\n");

		  // get The operation

		  Ast_Binary* binary = (Ast_Binary*) node;
		  //printf("Binary Operation is : %d\n", binary->op); 

		  // Get Left type
		  Ast_Type* left_type = type_expression(binary->left);
		  
		  // Get right Type
		  Ast_Type* right_type = type_expression(binary->right);


		  // Here we need to check if they are compatable not just if they are equal
		  if (right_type != left_type)
		  {
			  printf("Type Mismatch between left and right expressions in binary operation\n");
			  exit(0);
		  }

		  else return right_type;
		  // if the operation works then success otherwise report 
		  //break;
	  }
	  case AST_KIND_EXP_UNARY:
	  {
		  printf("Node type is : AST_KIND_EXP_UNARY\n");
		  break;
	  }
	  case AST_KIND_EXP_LITERAL:
	  {
		  printf("Node type is : AST_KIND_EXP_LITERAL\n");
		  break;
	  }
	  case AST_KIND_EXP_RETURN:
	  {
		  printf("Node type is : AST_KIND_EXP_RETURN\n");
		  break;
	  }
	  case AST_KIND_EXP_PROC_CALL:
	  {
		  printf("Node type is : KIND_EXP_PROC_CALL\n");
		  break;
	  }
	  case AST_KIND_EXP_MEM_ACC:
	  {
		  printf("Node type is : KIND_EXP_MEM_ACC\n");
		  break;
	  }
	  case AST_KIND_EXP_SUBSCRIPT:
	  {
		  printf("Node type is : AST_KIND_EXP_SUBSCRIPT\n");
		  break;
	  }
	  case AST_KIND_LITERAL_NUMBER:
	  {
		  node->resulting_type = _U64;
	  }
	  case AST_KIND_LITERAL_STRING:
	  {
		  node->resulting_type = _string;
	  }
	  default:
	  {
		  // @Unreachable:
		  printf("Unknown Node Type in TypeCheckExpression: [%.*s], AST Kind: [%d]\n",
				 SV_PRINT(node->token.name), node->kind);
		  UNREACHABLE;
		  break;
	  }
	}


	// TODO: More details
	assert(node->resulting_type && "Could Not infer type");
	return node->resulting_type;
}



static void type_check_declaration(Ast_Declaration* decl)
{
	auto type = decl->kind;

	if (AST_KIND_DECL_VARIABLE == type)
	{
		auto* var_decl = (Ast_Var_Declaration*)decl;

		if (var_decl->data_type == nullptr)
		{
			printf("Infering Type of variable [%.*s]\n", SV_PRINT(var_decl->token.name));
			var_decl->data_type = type_expression(var_decl->body);
			assert(var_decl->data_type && "Where is the type" );

			printf("Variable Declaration: Name[%.*s], Type[%.*s]\n",
				   SV_PRINT(var_decl->token.name),
				   SV_PRINT(var_decl->data_type->token.name));
		}
	}
}

	
static void type_check_block(Ast_Block* block)
{
	auto statements_len = arrlenu(block->statements);
	for(auto a = 0; a < statements_len; a++)
	{
		//printf("Number of statements [%lld]\n", arrlenu(block->statements));
		type_check_node(block->statements[a]);
	}	
}


static void type_check_node(Ast_Node* node)
{
	switch(node->type)
	{
	  case AST_BLOCK:
	  {
		  //UNREACHABLE;
		  //printf("Node type is : AST_BLOCK\n");
		  type_check_block((Ast_Block*)node);
		  break;
	  }
	  case AST_EXPRESSION:
	  {
		  //printf("Node type is : AST_EXPRESSION\n");
		  //TypeCheckExpression((Ast_Expression*)node);
		  break;
	  }
	  case AST_TYPE:
	  {
		  printf("Node type is : AST_TYPE\n");
		  break;
	  }
	  case AST_DECLARATION:
	  {
		  type_check_declaration((Ast_Declaration*)node);
		  break;
	  }
	  case AST_IF:
	  {
		  printf("Node type is : AST_IF\n");
		  break;
	  }
	  case AST_NOTE:
	  {
		  printf("Node type is : AST_NOTE\n");
		  break;
	  }
	  case AST_DIRECTIVE:
	  {
		  printf("Node type is : AST_DIRECTIVE\n");
		  break;
	  }
	  default:
	  {
		  printf("Unknown AST Node type\n");
	  }
	}
}




// void TypeCheck(Ast_Scope* scope, int level = 0)
// {
// 	if(scope == nullptr) return;
// 	// Print All variable declarations in this scope
// 	if (scope->variables)
// 	{
// 		auto variables_count = arrlenu(scope->variables);
// 		for(auto i = 0; i < variables_count; i++)
// 		{
// 			Ast_Var_Declaration* decl = scope->variables[i];
// 			
// 			printf("[%d] - %.*s\n", level, SV_PRINT(decl->token.name));
// 		}
// 	}
// 	
// 	
// 	if(scope->procedures)
// 	{
// 		//Ast_Var_Declaration
// 		auto procedures_count = arrlenu(scope->procedures);
// 		
// 		for(auto i = 0; i < procedures_count; i++)
// 		{
// 			// 
// 			Ast_Proc_Declaration* decl = scope->procedures[i];
// 			
// 			printf("[%d] - %.*s\n", level, SV_PRINT(decl->token.name));
// 		}
// 	}
// 	
// 	if (scope->structs)
// 	{
// 		auto structs_count = arrlenu(scope->structs);
// 		for(auto i = 0; i < structs_count; i++)
// 		{
// 			Ast_Struct_Declaration* decl = scope->structs[i];
// 			
// 			printf("[%d] - %.*s\n", level, SV_PRINT(decl->token.name));
// 		}
// 		
// 	}
// 	
// 	if (scope->children)
// 	{
// 		auto children_count = arrlenu(scope->children);
// 		for(auto i = 0; i < children_count; i++)
// 		{
// 			Ast_Scope* child_scope  = scope->children[i];
// 			TypeCheck(child_scope, level+1);
// 		}
// 	}
// }
// 
