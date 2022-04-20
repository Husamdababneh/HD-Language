/* ========================================================================
$File: graph.cpp
$Date: December 16th 2020 9:35 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#if ENABLE_GRAPH_PRINTING == 1
#include "graph.h"

StringView Shape_Names [] = {
	"box"_s,
	"polygon"_s,
	"ellipse"_s,
	"oval"_s,
	"circle"_s,
	"egg"_s,
	"triangle"_s,
	"diamond"_s,
	"trapezium"_s,
	"parallelogram"_s,
	"house"_s,
	"pentagon"_s,
	"hexagon"_s,
	"septagon"_s,
	"octagon"_s,
	"doublecircle"_s,
	"doubleoctagon"_s,
	"tripleoctagon"_s,
	"invtriangle"_s,
	"invtrapezium"_s,
	"invhouse"_s,
	"Mdiamond"_s,
	"Msquare"_s,
	"Mcircle"_s,
	"rect"_s,
	"rectangle"_s,
	"square"_s,
	"star"_s,
	"cylinder"_s,
	"note"_s,
	"tab"_s,
	"folder"_s,
	"box3d"_s,
	"component"_s,	
};

static Graph_Label* labels = nullptr;
static void 	
output_labels()
{
	for(U64 i = 0; i < arrlenu(labels); i++)
	{
		auto& label = labels[i];
		printf("T_%x [shape=\"%.*s\" label=\"%.*s\"];\n", 
			   label.hash,
			   SV_PRINT(Shape_Names[(U64)label.type]), 
			   SV_PRINT(label.str));
	}
	
	arrfree(labels);
	
}
static void
output_graph(Ast_Node* node)
{
	if (node == nullptr) return; 
	if (node->type == AST_UKNOWN) assert(false);
	
	if (node->kind == AST_KIND_UNKNOWN)
	{
		printf("Unknown Node Kind\n");
		printf("Node Token (%.*s)\n", SV_PRINT(node->token.name));
		printf("Node type (%d)\n", node->type);
	}
	
	
	U32 hash = MeowU32From(node->token.hash, 3);
	switch(node->type)
	{
		case AST_UKNOWN:
		{
			printf("Unknown Type Or Kind\n");
			break;
		}
		case AST_EXPRESSION:
		{
			
			if (node->kind == AST_KIND_EXP_BINARY){
				
				Ast_Binary* bin = (Ast_Binary*)node;
				printf("T_%x -> { ", hash);
				
				if (bin->left != nullptr){
					printf("T_%x ", MeowU32From(bin->left->token.hash, 3));
				}
				
				if (bin->right!= nullptr){
					printf("T_%x ", MeowU32From(bin->right->token.hash, 3));
				}
				printf("}\n");
				
				output_graph(bin->left);
				output_graph(bin->right);
				
				Graph_Label label = { bin->token.name ,  hash, Shape_Type::GRAPH_DIAMOND};
				arrput(labels, label);
				break;
			}
			
			else if (node->kind == AST_KIND_EXP_UNARY)
			{
				Ast_Unary* unary = (Ast_Unary*)node;
				printf("T_%x -> { ", hash);
				
				if (unary->child != nullptr){
					printf("T_%x ", MeowU32From(unary->child->token.hash, 3));
				}
				
				printf("}\n");
				
				output_graph(unary->child);
				
				Graph_Label label = { unary->token.name,  hash, Shape_Type::GRAPH_DIAMOND};
				arrput(labels, label);
				break;
			}
			else if (node->kind == AST_KIND_LITERAL_NUMBER || 
					 node->kind == AST_KIND_LITERAL_STRING || 
					 node->kind == AST_KIND_PRIMARY_IDENTIFIER)
			{
				Ast_Primary* primary = (Ast_Primary*) node;
				Graph_Label label = { primary->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				break;
			}
			else if (node->kind == AST_KIND_EXP_LITERAL)
			{
				Ast_Literal * literal = (Ast_Literal*) node;
				// TODO : if AST_KIND_LITERAL_STRING escape charachters 
				Graph_Label label = { literal->token.name,  hash, Shape_Type::GRAPH_BOX};
				arrput(labels, label);
				break;
			}
			else if (node->kind == AST_KIND_EXP_RETURN)
			{
				Ast_Return* _return =  (Ast_Return*) node;
				printf("T_%x -> { ", hash);
				
				for(U64 i = 0; i < arrlenu(_return->expressions); i++){
					printf("T_%x ", MeowU32From(_return->expressions[i]->token.hash, 3));
				}
				
				printf("}\n");
				
				for(U64 i = 0; i < arrlenu(_return->expressions); i++){
					output_graph(_return->expressions[i]);
				}
				
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				
				break;
			}
			else if (node->kind == AST_KIND_EXP_PROC_CALL)
			{
				Ast_Proc_Call* call =  (Ast_Proc_Call*) node;
				printf("T_%x -> { ", hash);
				
				for(U64 i = 0; i < arrlenu(call->arguments); i++){
					printf("T_%x ", MeowU32From(call->arguments[i]->token.hash, 3));
				}
				
				printf("T_%x ", MeowU32From(call->procedure->token.hash, 3));
				
				
				printf("}\n");
				
				for(U64 i = 0; i < arrlenu(call->arguments); i++){
					output_graph(call->arguments[i]);
				}
				output_graph(call->procedure);
				
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				
			}
			else if (node->kind == AST_KIND_EXP_MEM_ACC)
			{
				auto mem = (Ast_Member_Access*) node;
				printf("T_%x -> { ", hash);
				
				printf("T_%x ", MeowU32From(mem->_struct->token.hash, 3));
				
				printf("T_%x ", MeowU32From(mem->member.hash,3 ));
				
				printf("}\n");
				
				//printf("T_%x -> T_%x\n", mem->_struct->token.hash, mem->member.hash);
				
				output_graph(mem->_struct);
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				Graph_Label label2= { mem->member.name ,  (U32)MeowU32From(mem->member.hash, 3), Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				arrput(labels, label2);
			}
			else if (node->kind == AST_KIND_EXP_SUBSCRIPT)
			{
				auto sub = (Ast_Subscript* )node;
				printf("T_%x -> { ", hash);
				
				printf("T_%x ", MeowU32From(sub->exp->token.hash, 3));
				printf("T_%x ", MeowU32From(sub->value->token.hash, 3));
				printf("}\n");
				
				output_graph(sub->exp);
				output_graph(sub->value);
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
			}
			else {
				printf("node->type = %d\n", node->type);
				printf("node->kind = %d\n", node->kind);
				assert(false && "Not supported Expression type");
			}
			break;
			//assert(false);
		}
		case AST_DECLARATION:
		{
			
			if (node->kind == AST_KIND_DECL_STRUCT)
			{
				Ast_Struct_Declaration* _struct = (Ast_Struct_Declaration*)node;
				printf("T_%x -> { ", hash);
				
				if (_struct->decls != nullptr && arrlenu(_struct->decls) > 0){
					for (U64 i = 0; i < arrlenu(_struct->decls); i++)
						printf("T_%x ", MeowU32From(_struct->decls[i]->token.hash,3));
				}
				
				printf("}\n");
				
				if (_struct->decls != nullptr && arrlenu(_struct->decls) > 0){
					for (U64 i = 0; i < arrlenu(_struct->decls); i++)
						output_graph(_struct->decls[i]);
				}
				
				Graph_Label label = { _struct->token.name ,  hash, Shape_Type::GRAPH_PENTAGON};
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_DECL_PROCEDURE)
			{
				Ast_Proc_Declaration * decl = (Ast_Proc_Declaration*) node; 
				printf("T_%x -> { ", hash);
				
				if (decl->return_type != nullptr && arrlenu(decl->return_type) > 0){
					for (U64 i = 0; i < arrlenu(decl->return_type); i++)
						printf("T_%x ", MeowU32From(decl->return_type[i]->token.hash, 3));
				}
				//&& arrlenu(decl->body->statements) > 0
				if (decl->body ){
					printf("T_%x ", MeowU32From(decl->body->token.hash,3));
				}
				
				printf("}\n");
				
				output_graph(decl->body);
				if (decl->return_type != nullptr && arrlenu(decl->return_type) > 0){
					for (U64 i = 0; i < arrlenu(decl->return_type); i++)
						output_graph(decl->return_type[i]);
				}
				
				Graph_Label label = { decl->token.name ,  hash, Shape_Type::GRAPH_PENTAGON};
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_DECL_VARIABLE)
			{
				Ast_Var_Declaration* decl = (Ast_Var_Declaration *) node; 
				
				if (decl->data_type || decl->body){
					printf("T_%x -> { ", hash);
					
					if (decl->data_type){
						printf("T_%x ", MeowU32From(decl->data_type->token.hash, 3));
					}
					
					if (decl->body){
						printf("T_%x ", MeowU32From(decl->body->token.hash, 3));
					}
					
					printf("}\n");
					output_graph(decl->body);
					output_graph(decl->data_type);
				}
				Graph_Label label = { decl->token.name ,  hash, Shape_Type::GRAPH_PENTAGON};
				arrput(labels, label);
				break;
			}
			
			break;
		}
		case AST_TYPE:
		{
			Ast_Type* type = (Ast_Type*) node;
			Graph_Label label = { type->token.name,  hash, Shape_Type::GRAPH_BOX};
			arrput(labels, label);
			break;
		}
		case AST_BLOCK:
		{
			Ast_Block* block =  (Ast_Block*) node;
			
			if (arrlenu(block->statements) <= 0)
				break;
			printf("T_%x -> { ", hash);
			
			
			for(U64 i = 0; i < arrlenu(block->statements); i++){
				printf("T_%x ", MeowU32From(block->statements[i]->token.hash,3));
			}
			
			
			printf("}\n");
			
			
			for(U64 i = 0; i < arrlenu(block->statements); i++){
				output_graph(block->statements[i]);
			}
			
			Graph_Label label =  { block->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
			//{ "Block"_s ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
			arrput(labels, label);
			break;
		}
		case AST_IF:
		{
			Ast_If* ifnode =  (Ast_If *) node;
			printf("T_%x -> { ", hash);
			
			if (ifnode->exp)
				printf("T_%x ", MeowU32From(ifnode->exp->token.hash, 3));
			
			if (ifnode->statement)
				printf("T_%x ", MeowU32From(ifnode->statement->token.hash, 3));
			
			if (ifnode->next)
			{
				for(U64 i = 0; i < arrlenu(ifnode->next); i++){
					printf("T_%x ", MeowU32From(ifnode->next[i]->token.hash, 3));
				}
			}
			
			printf("}\n");
			
			if (ifnode->exp)
				output_graph(ifnode->exp);
			
			
			if (ifnode->statement)
				output_graph(ifnode->statement);
			
			
			if (ifnode->next)
			{
				for(U64 i = 0; i < arrlenu(ifnode->next); i++){
					output_graph(ifnode->next[i]);
				}
			}
			
			Graph_Label label = {"If Statment"_s ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
			arrput(labels, label);
			break;
		}
		case AST_WHILE:
		{
			printf("//Node Type AST_WHILE is not supported yet\n");
			break;
		}
		default:
		{
			//output_graph(node, logger);
			printf("[Graph] Unhandled Node Type [%d] Token name [%d]\n", node->type, node->kind);
			//printf("[Graph] Unhandled Node Type [%*s] Token name [%.*s]\n", //SV_PRINT(ast_kind_to_string(node->type)), SV_PRINT(node->token.name));
			break;
		}
	}
	return;
}

#define PRINT_GRAPH(node)  output_graph(node); output_labels();
#else
#define PRINT_GRAPH(node, logger)
#endif












