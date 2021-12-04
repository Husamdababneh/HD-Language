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
output_labels(Logger* logger)
{
	for(U64 i = 0; i < arrlenu(labels); i++)
	{
		auto& label = labels[i];
		logger->print("T_%x [shape=\"%s\" label=\"%s\"];\n"_s, label.hash, Shape_Names[(U64)label.type], 
					  label.str);
	}
	
	arrfree(labels);
	
}
static void
output_graph(Ast_Node* node, Logger* logger)
{
	if (node == nullptr) return; 
	if (node->type == AST_UKNOWN) assert(false);
	
	if (node->kind == AST_KIND_UNKNOWN)
	{
		printf("Unknown Node Kind\n");
		printf("Node Token (%.*s)\n", SV_PRINT(node->token.name));
		printf("Node type (%d)\n", node->type);
	}
	
	meow_u128 hash = node->token.hash;
	//auto hash = MeowU32From(full_hash, 3);
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
				logger->print("T_%x -> { "_s, hash);
				
				if (bin->left != nullptr){
					logger->print("T_%x "_s, bin->left->token.hash);
				}
				
				if (bin->right!= nullptr){
					logger->print("T_%x "_s, bin->right->token.hash);
				}
				logger->print("}\n"_s);
				
				output_graph(bin->left, logger);
				output_graph(bin->right, logger);
				
				Graph_Label label = { bin->token.name ,  hash, Shape_Type::GRAPH_DIAMOND};
				arrput(labels, label);
				break;
			}
			
			else if (node->kind == AST_KIND_EXP_UNARY)
			{
				Ast_Unary* unary = (Ast_Unary*)node;
				logger->print("T_%x -> { "_s, hash);
				
				if (unary->child != nullptr){
					logger->print("T_%x "_s, unary->child->token.hash);
				}
				
				logger->print("}\n"_s);
				
				output_graph(unary->child, logger);
				
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
				logger->print("T_%x -> { "_s, hash);
				
				for(U64 i = 0; i < arrlenu(_return->expressions); i++){
					logger->print("T_%x "_s, _return->expressions[i]->token.hash);
				}
				
				logger->print("}\n"_s);
				
				for(U64 i = 0; i < arrlenu(_return->expressions); i++){
					output_graph(_return->expressions[i], logger);
				}
				
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				
				break;
			}
			else if (node->kind == AST_KIND_EXP_PROC_CALL)
			{
				Ast_Proc_Call* call =  (Ast_Proc_Call*) node;
				logger->print("T_%x -> { "_s, hash);
				
				for(U64 i = 0; i < arrlenu(call->arguments); i++){
					logger->print("T_%x "_s, call->arguments[i]->token.hash);
				}
				
				logger->print("T_%x "_s, call->procedure->token.hash);
				
				
				logger->print("}\n"_s);
				
				for(U64 i = 0; i < arrlenu(call->arguments); i++){
					output_graph(call->arguments[i], logger);
				}
				output_graph(call->procedure, logger);
				
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				
			}
			else if (node->kind == AST_KIND_EXP_MEM_ACC)
			{
				auto mem = (Ast_Member_Access*) node;
				logger->print("T_%x -> { "_s, hash);
				
				logger->print("T_%x "_s, mem->_struct->token.hash);
				
				logger->print("T_%x "_s, mem->member.hash);
				
				logger->print("}\n"_s);
				
				//logger->print("T_%x -> T_%x\n"_s, mem->_struct->token.hash, mem->member.hash);
				
				output_graph(mem->_struct, logger);
				Graph_Label label = { node->token.name ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
				Graph_Label label2= { mem->member.name ,  mem->member.hash, Shape_Type::GRAPH_INVTRIANGLE};
				arrput(labels, label);
				arrput(labels, label2);
			}
			else if (node->kind == AST_KIND_EXP_SUBSCRIPT)
			{
				auto sub = (Ast_Subscript* )node;
				logger->print("T_%x -> { "_s, hash);
				
				logger->print("T_%x "_s, sub->exp->token.hash);
				logger->print("T_%x "_s, sub->value->token.hash);
				logger->print("}\n"_s);
				
				output_graph(sub->exp, logger);
				output_graph(sub->value, logger);
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
				logger->print("T_%x -> { "_s, hash);
				
				if (_struct->decls != nullptr && arrlenu(_struct->decls) > 0){
					for (U64 i = 0; i < arrlenu(_struct->decls); i++)
						logger->print("T_%x "_s, _struct->decls[i]->token.hash);
				}
				
				logger->print("}\n"_s);
				
				if (_struct->decls != nullptr && arrlenu(_struct->decls) > 0){
					for (U64 i = 0; i < arrlenu(_struct->decls); i++)
						output_graph(_struct->decls[i], logger);
				}
				
				Graph_Label label = { _struct->token.name ,  hash, Shape_Type::GRAPH_PENTAGON};
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_DECL_PROCEDURE)
			{
				Ast_Proc_Declaration * decl = (Ast_Proc_Declaration*) node; 
				logger->print("T_%x -> { "_s, hash);
				
				if (decl->return_type != nullptr && arrlenu(decl->return_type) > 0){
					for (U64 i = 0; i < arrlenu(decl->return_type); i++)
						logger->print("T_%x "_s, decl->return_type[i]->token.hash);
				}
				
				if (decl->body){
					logger->print("T_%x "_s, decl->body->token.hash);
				}
				
				logger->print("}\n"_s);
				
				output_graph(decl->body, logger);
				if (decl->return_type != nullptr && arrlenu(decl->return_type) > 0){
					for (U64 i = 0; i < arrlenu(decl->return_type); i++)
						output_graph(decl->return_type[i], logger);
				}
				
				Graph_Label label = { decl->token.name ,  hash, Shape_Type::GRAPH_PENTAGON};
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_DECL_VARIABLE)
			{
				Ast_Var_Declaration* decl = (Ast_Var_Declaration *) node; 
				logger->print("T_%x -> { "_s, hash);
				
				if (decl->data_type){
					logger->print("T_%x "_s, decl->data_type->token.hash);
				}
				
				if (decl->body){
					logger->print("T_%x "_s, decl->body->token.hash);
				}
				
				logger->print("}\n"_s);
				output_graph(decl->body, logger);
				output_graph(decl->data_type, logger);
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
			
			logger->print("T_%x -> { "_s, hash);
			
			
			for(U64 i = 0; i < arrlenu(block->statements); i++){
				logger->print("T_%x "_s, block->statements[i]->token.hash);
			}
			
			
			logger->print("}\n"_s);
			
			for(U64 i = 0; i < arrlenu(block->statements); i++){
				output_graph(block->statements[i], logger);
			}
			
			Graph_Label label = { "Block"_s ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
			arrput(labels, label);
			break;
		}
		case AST_IF:
		{
			Ast_If* ifnode =  (Ast_If *) node;
			logger->print("T_%x -> { "_s, hash);
			
			if (ifnode->exp)
				logger->print("T_%x "_s, ifnode->exp->token.hash);
			
			if (ifnode->statement)
				logger->print("T_%x "_s, ifnode->statement->token.hash);
			
			if (ifnode->next)
			{
				for(U64 i = 0; i < arrlenu(ifnode->next); i++){
					logger->print("T_%x "_s, ifnode->next[i]->token.hash);
				}
			}
			
			logger->print("}\n"_s);
			
			if (ifnode->exp)
				output_graph(ifnode->exp, logger);
			
			
			if (ifnode->statement)
				output_graph(ifnode->statement, logger);
			
			
			if (ifnode->next)
			{
				for(U64 i = 0; i < arrlenu(ifnode->next); i++){
					output_graph(ifnode->next[i], logger);
				}
			}
			
			Graph_Label label = {"If Statment"_s ,  hash, Shape_Type::GRAPH_INVTRIANGLE};
			arrput(labels, label);
			break;
		}
		case AST_WHILE:
		{
			logger->print("//Node Type AST_WHILE is not supported yet\n"_s);
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

#define PRINT_GRAPH(node, logger)  output_graph(node, logger); output_labels(logger);
#else
#define PRINT_GRAPH(node, logger)
#endif












