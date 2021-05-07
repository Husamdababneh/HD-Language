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
	for(u64 i = 0; i < arrlenu(labels); i++)
	{
		auto& label = labels[i];
		logger->print("T_%x [shape=\"%s\" label=\"%s\"];\n"_s, label.hash, Shape_Names[(u64)label.type], 
					  label.str);
	}
	
	arrfree(labels);
	
}
static void
output_graph_v2(Ast_Node* node, Logger* logger)
{
	if (node == nullptr) return; 
	if (node->type == AST_UKNOWN) return;
	
	meow_u128 hash = node->token.hash;
	//auto hash = MeowU32From(full_hash, 3);
	switch(node->type)
	{
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
				
				output_graph_v2(bin->left, logger);
				output_graph_v2(bin->right, logger);
				
				Graph_Label label= { hash, Shape_Type::GRAPH_DIAMOND, bin->token.name };
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_EXP_UNARY)
			{
				Ast_Unary* unary = (Ast_Unary*)node;
				logger->print("T_%x -> { "_s, hash);
				
				if (unary->child != nullptr){
					logger->print("T_%x "_s, unary->child->token.hash);
				}
				
				logger->print("}\n"_s);
				
				output_graph_v2(unary->child, logger);
				
				Graph_Label label = { hash, Shape_Type::GRAPH_DIAMOND, unary->token.name};
				arrput(labels, label);
				break;
			}
			
			if (node->kind == AST_KIND_EXP_PRIMARY)
			{
				Ast_Primary* primary = (Ast_Primary*) node;
				Graph_Label label = { hash, Shape_Type::GRAPH_INVTRIANGLE, primary->token.name };
				arrput(labels, label);
				break;
			}
			if (node->kind == AST_KIND_EXP_LITERAL)
			{
				Ast_Literal * literal = (Ast_Literal*) node;
				Graph_Label label = { hash, Shape_Type::GRAPH_BOX, literal->token.name};
				arrput(labels, label);
				break;
			}
			if (node->kind == AST_KIND_EXP_RETURN)
			{
				Ast_Return* _return =  (Ast_Return*) node;
				logger->print("T_%x -> { "_s, hash);
				
				for(u64 i = 0; i < arrlenu(_return->expressions); i++){
					logger->print("T_%x "_s, _return->expressions[i]->token.hash);
				}
				
				logger->print("}\n"_s);
				
				for(u64 i = 0; i < arrlenu(_return->expressions); i++){
					output_graph_v2(_return->expressions[i], logger);
				}
				
				Graph_Label label = { hash, Shape_Type::GRAPH_INVTRIANGLE, node->token.name };
				arrput(labels, label);
				
				break;
			}
			break;
			//assert(false);
		}
		case AST_DECLARATION:
		{
			
			if (node->kind == AST_KIND_DECL_STRUCT)
			{
				// TODO:
				logger->print("AST_KIND_DECL_STRUCT is not Supported Yet\n"_s);
				assert(false);
			}
			
			if (node->kind == AST_KIND_DECL_PROCEDURE)
			{
				Ast_Proc_Declaration * decl = (Ast_Proc_Declaration*) node; 
				logger->print("T_%x -> { "_s, hash);
				
				if (decl->return_type != nullptr && arrlenu(decl->return_type) > 0){
					for (u64 i = 0; i < arrlenu(decl->return_type); i++)
						logger->print("T_%x "_s, decl->return_type[i]->token.hash);
				}
				
				if (decl->body){
					logger->print("T_%x "_s, decl->body->token.hash);
				}
				
				logger->print("}\n"_s);
				
				output_graph_v2(decl->body, logger);
				for (u64 i = 0; i < arrlenu(decl->return_type); i++)
					output_graph_v2(decl->return_type[i], logger);
				
				Graph_Label label = { hash, Shape_Type::GRAPH_PENTAGON, decl->token.name };
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
				output_graph_v2(decl->body, logger);
				output_graph_v2(decl->data_type, logger);
				Graph_Label label = { hash, Shape_Type::GRAPH_PENTAGON, decl->token.name };
				arrput(labels, label);
				break;
			}
			
			break;
		}
		case AST_TYPE:
		{
			Ast_Type* type = (Ast_Type*) node;
			Graph_Label label = { hash, Shape_Type::GRAPH_BOX, type->token.name};
			arrput(labels, label);
			break;
		}
		case AST_BLOCK:
		{
			Ast_Block* block =  (Ast_Block*) node;
			
			logger->print("T_%x -> { "_s, hash);
			
			
			for(u64 i = 0; i < arrlenu(block->statements); i++){
				logger->print("T_%x "_s, block->statements[i]->token.hash);
			}
			
			
			logger->print("}\n"_s);
			
			for(u64 i = 0; i < arrlenu(block->statements); i++){
				output_graph_v2(block->statements[i], logger);
			}
			
			Graph_Label label = { hash, Shape_Type::GRAPH_INVTRIANGLE, "Block"_s };
			arrput(labels, label);
			break;
		}
		case AST_IF:
		{
			logger->print("Node Type AST_IF is not supported yet\n"_s);
			break;
		}
		case AST_WHILE:
		{
			logger->print("Node Type AST_WHILE is not supported yet\n"_s);
			break;
		}
		default:
		{
			//output_graph_v2(node, logger);
			printf("[Graph] Unhandled Node Type %d Token name [%.*s]\n", node->type, SV_PRINT(node->token.name));
			break;
		}
	}
	return;
}

#define PRINT_GRAPH(node, logger)  output_graph_v2(node, logger); output_labels(logger);
#else
#define PRINT_GRAPH(node, logger)
#endif












