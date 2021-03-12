/* ========================================================================
$File: graph.cpp
$Date: December 16th 2020 9:35 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#if ENABLE_GRAPH_PRINTING == 1
#include "graph.h"

String Shape_Names [] = {
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

static Array<Graph_Label> labels = init_array<Graph_Label>(50);
static void 	
output_labels(Logger* logger)
{
	for(u64 i = 0; i < labels.occupied; i++)
	{
		auto& label = labels[i];
		logger->print("T_%x [shape=\"%s\" label=\"%s\"];\n"_s, label.hash, Shape_Names[(u64)label.type], 
					  label.str);
	}
}

#define Hash(x)  MeowHash(MeowDefaultSeed, sizeof(u16),(void*)&x->token.id); 
// Make this more sophisticated 
static void
output_graph(Ast_Node* node, Logger* logger)
{
	
	if (node == nullptr)
		return;
	
	if (node->type == AST_UKNOWN)
		abort();
	
	if (node->type == AST_BINARY_EXP){
		Ast_Binary* bin = (Ast_Binary*)node;
		
		meow_u128 hash = Hash(bin);
		
		
		if (bin->left != nullptr){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->left, logger);
			logger->print("\n"_s);
		}
		
		if (bin->right != nullptr){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->right, logger);
			logger->print("\n"_s);
		}
		
		
		array_add<Graph_Label>(&labels, { MeowU32From(hash, 3), Shape_Type::DIAMOND,
								   bin->token.name });
	} else if (node->type == AST_LITERAL) {
		Ast_Literal * literal = (Ast_Literal*) node;
		meow_u128 hash_left = Hash(literal);
		logger->print("T_%x "_s, MeowU32From(hash_left, 3));
		array_add(&labels, { MeowU32From(hash_left, 3), Shape_Type::BOX, literal->token.name});
		
	} else if (node->type == AST_UNARY_EXP) {
		Ast_Unary * unary = (Ast_Unary*) node;
		meow_u128 hash_left = Hash(unary);
		
		if (unary->child != nullptr){
			logger->print("T_%x -> "_s, MeowU32From(hash_left, 3));
			output_graph(unary->child, logger);
		}
		
		array_add(&labels, { MeowU32From(hash_left, 3), Shape_Type::RECT, unary->token.name});
		
	} else if (node->type == AST_IDENT) {
		Ast_Ident* ident = (Ast_Ident*) node;
		meow_u128 hash = Hash(ident);
		
		logger->print("T_%x "_s, MeowU32From(hash, 3));
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::SQUARE, ident->token.name});
		
	} else if (node->type == AST_DECLARATION) {
		Ast_Declaration* decl =  (Ast_Declaration*) node;
		meow_u128 hash = Hash(decl);
		
		if (decl->data_type){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(decl->data_type, logger);
		}
		
		if (decl->params){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(decl->params, logger);
			logger->print("\n"_s);
		}
		
		if (decl->body){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(decl->body, logger);
			logger->print("\n"_s);
		}
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::PENTAGON, decl->token.name});
		
	} else if (node->type == AST_FUNCALL) {
		Ast_FunctionCall* ident = (Ast_FunctionCall*) node;
		meow_u128 hash = Hash(ident);
		
		if (ident->arguments.occupied > 0){
			for(u64 i = 0; i < ident->arguments.occupied; i++){
				logger->print("T_%x -> "_s, MeowU32From(hash, 3));
				output_graph(ident->arguments[i], logger);
				logger->print(" \n"_s);
			}
		}else{
			logger->print("T_%x \n"_s, MeowU32From(hash, 3));
		}
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::TRIANGLE, ident->token.name});
		
	} else if (node->type == AST_FACTOR) {
		Ast_Factor* factor = (Ast_Factor*)node;
		meow_u128 hash = Hash(factor);
		
		
		logger->print("T_%x -> "_s, MeowU32From(hash, 3),MeowU32From(hash, 3));
		output_graph(factor->node, logger);
		
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::COMPONENT, "Factor"_s});
	} else if (node->type == AST_BLOCK) {
		Ast_Block* block =  (Ast_Block*) node;
		meow_u128 hash = Hash(block);
		
		for(u64 i = 0; i < block->statements.occupied; i++){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(block->statements[i], logger);
			logger->print(" \n"_s);
		}
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::INVTRIANGLE, "Block"_s });
		
	} else if (node->type == AST_SUBSCRIPT) {
		Ast_Subscript* st  =  (Ast_Subscript*) node;
		meow_u128 hash = Hash(st);
		
		logger->print("T_%x -> "_s, MeowU32From(hash, 3));
		output_graph(st->exp, logger);
		logger->print("\n"_s);
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::INVHOUSE, st->token.name });
		
	} else if (node->type == AST_LIST) {
		Ast_List* st  =  (Ast_List*) node;
		meow_u128 hash = Hash(st);
		
		for(u64 i = 0; i < st->list.occupied; i++){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(st->list[i], logger);
			logger->print(" \n"_s);
		}
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::STAR, st->token.name });
		
	} else {
		logger->print("Unsupported Node Element [%d]!!!!!!!!!!!!!!!!!!\n"_s, node->type);
		abort();
	}
	//logger->print("Unsupported Node Element!!!!!!!!!!!!!!!!!!\n"_s);
}


#define PRINT_GRAPH(node, logger)  labels.occupied = 0;output_graph(node, logger); output_labels(logger);
#else
#define PRINT_GRAPH(node, logger)
#endif


