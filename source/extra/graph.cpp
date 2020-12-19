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
		auto& label = *labels[i];
		logger->print("T_%x [shape=\"%s\" label=\"%s\"];\n"_s, label.hash, Shape_Names[(u64)label.type], 
					  label.str);
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
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->left, logger);
		}
		
		array_add<Graph_Label>(&labels, { MeowU32From(hash, 3), Shape_Type::DIAMOND,
								   bin->token.name });
		if (bin->right != nullptr){
			logger->print("T_%x -> "_s, MeowU32From(hash, 3));
			output_graph(bin->right, logger);
		}
		
		
		
	} else if (node->type == AST_LITERAL) {
		Ast_Literal * literal = (Ast_Literal*) node;
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*literal), literal);
		logger->print("T_%x \n"_s, MeowU32From(hash_left, 3));
		array_add(&labels, { MeowU32From(hash_left, 3), Shape_Type::BOX, literal->token.name});
		
	} else if (node->type == AST_UNARY_EXP) {
		Ast_Unary * unary = (Ast_Unary*) node;
		meow_u128 hash_left = MeowHash(MeowDefaultSeed, sizeof(*unary), unary);
		
		
		if (unary->child != nullptr){
			logger->print("T_%x -> "_s, MeowU32From(hash_left, 3));
			output_graph(unary->child, logger);
		}
		
		array_add(&labels, { MeowU32From(hash_left, 3), Shape_Type::RECT, unary->token.name});
		
	} else if (node->type == AST_ASSIGN) {
		Ast_Assign* assign =  (Ast_Assign*) node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*assign), assign);
		
		output_graph(assign->ident, logger);
		logger->print("-> T_%x\n"_s, MeowU32From(hash, 3));
		
		logger->print("T_%x -> "_s, MeowU32From(hash, 3));
		output_graph(assign->exp, logger);
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::MDIAMOND, assign->token.name});
		
	} else if (node->type == AST_IDENT) {
		Ast_Ident* ident = (Ast_Ident*) node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*ident), ident);
		
		logger->print("T_%x \n"_s, MeowU32From(hash, 3));
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::SQUARE, ident->token.name});
		
	} else if (node->type == AST_FUNCALL) {
		Ast_FunctionCall* ident = (Ast_FunctionCall*) node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*ident), ident);
		
		if (ident->arguments.occupied > 0){
			for(u64 i = 0; i < ident->arguments.occupied; i++){
				logger->print("T_%x -> "_s, MeowU32From(hash, 3));
				output_graph(*ident->arguments[i], logger);
				logger->print(" \n"_s);
			}
		}else{
			logger->print("T_%x \n"_s, MeowU32From(hash, 3));
		}
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::TRIANGLE, ident->token.name});
		
	} else if (node->type == AST_FACTOR) {
		Ast_Factor* factor = (Ast_Factor*)node;
		meow_u128 hash = MeowHash(MeowDefaultSeed, sizeof(*factor), factor);
		//logger->print("T_%x -> T_%x_left\n"_s, MeowU32From(hash, 3),MeowU32From(hash, 3));
		
		logger->print("T_%x -> "_s, MeowU32From(hash, 3),MeowU32From(hash, 3));
		output_graph(factor->node, logger);
		
		//logger->print("T_%x -> T_%x_right\n"_s, MeowU32From(hash, 3),MeowU32From(hash, 3));
		
		array_add(&labels, { MeowU32From(hash, 3), Shape_Type::COMPONENT, "Factor"_s});
	} else {
		logger->print("Unsupported Node Element!!!!!!!!!!!!!!!!!!\n"_s);
		abort();
	}
	//logger->print("Unsupported Node Element!!!!!!!!!!!!!!!!!!\n"_s);
}


#define PRINT_GRAPH(node, logger)  labels.occupied = 0;output_graph(node, logger); output_labels(logger);
#else
#define PRINT_GRAPH(node, logger)
#endif


