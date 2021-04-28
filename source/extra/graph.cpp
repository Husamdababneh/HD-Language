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
		case AST_BINARY_EXP:
		{
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
		case AST_DECLARATION:
		{
			Ast_Declaration* decl =  (Ast_Declaration*) node;
			logger->print("T_%x -> { "_s, hash);
			
			if (decl->data_type) {
				logger->print("T_%x "_s, decl->data_type->token.hash);
			}
			
			if (decl->params){
				logger->print("T_%x "_s, decl->params->token.hash);
			}
			
			if (decl->body){
				logger->print("T_%x "_s, decl->body->token.hash);
			}
			
			logger->print("}\n"_s);
			
			output_graph_v2(decl->data_type, logger);
			output_graph_v2(decl->params, logger);
			output_graph_v2(decl->body, logger);
			Graph_Label label = { hash, Shape_Type::GRAPH_PENTAGON, decl->token.name };
			arrput(labels, label);
			break;
		}
		case AST_IDENT:
		{
			Ast_Ident* ident = (Ast_Ident*) node;
			Graph_Label label = { hash, Shape_Type::GRAPH_SQUARE, ident->token.name};
			arrput(labels, label);
			break;
		}
		case AST_LITERAL:
		{
			Ast_Literal * literal = (Ast_Literal*) node;
			Graph_Label label = { hash, Shape_Type::GRAPH_BOX, literal->token.name};
			arrput(labels, label);
			break;
		}
		case AST_ASSIGN:
		{
			Ast_Assign* assign = (Ast_Assign*) node;
			logger->print("T_%x -> { "_s, hash);
			if (assign->left) {
				logger->print("T_%x "_s, assign->left->token.hash);
			}
			
			if (assign->right) {
				logger->print("T_%x "_s, assign->right->token.hash);
			}
			logger->print("}\n"_s);
			output_graph_v2(assign->left, logger);
			output_graph_v2(assign->right, logger);
			
			Graph_Label label = { hash, Shape_Type::GRAPH_BOX, assign->token.name};
			arrput(labels, label);
			break;
		}
		case AST_PORCDECLARATION:
		{
			logger->print("Node Type AST_PORCDECLARATION is not supported yet\n"_s);
			break;
		}
		case AST_TYPE:
		{
			logger->print("Node Type AST_TYPE is not supported yet\n"_s);
			break;
		}
		case AST_BLOCK:
		{
			Ast_Block* block =  (Ast_Block*) node;
			logger->print("T_%x -> { "_s, hash);
			
#if GRAPH_SHOW_BLOCK_PARENT
			if (block->enclosing_scope) {
				logger->print("T_%x "_s, block->enclosing_scope->token.hash);
			}
#endif
			for(u64 i = 0; i < arrlenu(block->statements); i++){
				logger->print("T_%x "_s, block->statements[i]->token.hash);
			}
			
			logger->print("}\n"_s);
			
			for(u64 i = 0; i < arrlenu(block->statements); i++){
				output_graph_v2(block->statements[i], logger);
			}
			//logger->print("Node Type AST_BLOCK is not supported yet\n"_s);
			Graph_Label label = { hash, Shape_Type::GRAPH_INVTRIANGLE, "Block"_s };
			arrput(labels, label);
			break;
		}
		case AST_DEFINETION:
		{
			logger->print("Node Type AST_DEFINETION is not supported yet\n"_s);
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
		case AST_UNARY_EXP:
		{
			logger->print("Node Type AST_UNARY_EXP is not supported yet\n"_s);
			break;
		}
		case AST_FUNCALL:
		{
			logger->print("Node Type AST_FUNCALL is not supported yet\n"_s);
			break;
		}
		case AST_ARGUMENT:
		{
			logger->print("Node Type AST_ARGUMENT is not supported yet\n"_s);
			break;
		}
		case AST_PARMETER:
		{
			logger->print("Node Type AST_PARMETER is not supported yet\n"_s);
			break;
		}
		case AST_FACTOR:
		{
			logger->print("Node Type AST_FACTOR is not supported yet\n"_s);
			break;
		}
		case AST_PROC:
		{
			logger->print("Node Type AST_PROC is not supported yet\n"_s);
			break;
		}
		case AST_STRUCT:
		{
			logger->print("Node Type AST_STRUCT is not supported yet\n"_s);
			break;
		}
		case AST_SUBSCRIPT:
		{
			logger->print("Node Type AST_SUBSCRIPT is not supported yet\n"_s);
			break;
		}
		case AST_LIST:
		{
			Ast_List* list = (Ast_List*)node;
			logger->print("T_%x -> { "_s, hash);
			
			for(u64 a = 0; a < arrlenu(list->list); a++ )
			{
				logger->print("T_%x "_s,list->list[a]->token.hash);
			}
			
			logger->print("}\n"_s);
			
			for(u64 a = 0; a < arrlenu(list->list); a++ )
			{
				output_graph_v2(list->list[a], logger);
			}
			
			Graph_Label label = { hash, Shape_Type::GRAPH_STAR, list->token.name };
			arrput(labels, label);
			//logger->print("Node Type AST_LIST is not supported yet\n"_s);
			break;
		}
		case AST_MEBMER_ACCESS:
		{
			Ast_MemberAccess* ma = (Ast_MemberAccess*)node;
			
			logger->print("T_%x -> { "_s, hash);
			if (ma->left) {
				logger->print("T_%x "_s, ma->left->token.hash);
			}
			
			if (ma->right) {
				logger->print("T_%x "_s, ma->right->token.hash);
			}
			logger->print("}\n"_s);
			output_graph_v2(ma->left, logger);
			output_graph_v2(ma->right, logger);
			
			Graph_Label label = { hash, Shape_Type::GRAPH_BOX, ma->token.name};
			arrput(labels, label);
			break;
			break;
		}
		default:
		{
			//output_graph_v2(node, logger);
			logger->print("Unknown Node Type %d?? \n"_s, node->type);
			break;
		}
	}
	return;
}

#define PRINT_GRAPH(node, logger)  output_graph_v2(node, logger); output_labels(logger);
#else
#define PRINT_GRAPH(node, logger)
#endif












