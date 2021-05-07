#include "pch.h"

#include "ast.h"


static FILE* _outfile;

static StringView* funcs = NULL;
static Ast_Var_Declaration** vars  = NULL;

inline
FILE* getFile()
{
	if (_outfile == nullptr) {
		errno_t err = open_file_handle(_outfile, "first.asm", "wb");
		if (err != 0) assert(false && "Cannot Open File");
		
		fprintf(_outfile, "bits 64\n");
		fprintf(_outfile, "default rel\n");
		fprintf(_outfile, "extern ExitProcess\n");
		fprintf(_outfile, "extern _CRT_INIT\n");
		fprintf(_outfile, "extern printf\n");
		
		fprintf(_outfile, "\n");
	}
	
	return _outfile;
}

void close_file()
{
	if (_outfile == nullptr) return;
	
	fclose(_outfile);
	_outfile = nullptr;
}

void generate_statement() {
	
}

inline void push_rax() {
	fprintf(getFile(), "\tpush rax\n");
}

inline void push_rdi() {
	fprintf(getFile(), "\tpush rdi\n");
}

inline void pop_rdi() {
	fprintf(getFile(), "\tpop rdi\n");
}

inline void push_register(const StringView& str) {
	fprintf(getFile(), "\tpush %.*s\n", SV_PRINT(str));
}

inline void pop_register(const StringView& str) {
	fprintf(getFile(), "\tpush %.*s\n", SV_PRINT(str));
}

// Forward declerations 
void generate_expression(Ast_Expression* exp);
void generate_binary_expression(Ast_Binary* binary);
void generate_block(Ast_Block* block);

void generate_address(Ast_Expression* exp)
{
	fprintf(getFile(), "\tlea rax, [%.*s]\n", SV_PRINT(exp->token.name));
}

void generate_binary_expression(Ast_Binary* binary)
{
	
	FILE* file = getFile();
	
	// TODO: IF assign, 
	/*
	-  resolve (generate address for) the left hand side
	-  store in rax, aka (push rax)
	-  generate right hand side, (generate exp) // the value of this will be stored in stack ? 
-  pop rdi
-  store 
	*/
	
	if (binary->op == AST_BINARY_ASSIGN)
	{
		//generate_expression((Ast_Expression*)binary->left);
		generate_address((Ast_Expression*)binary->left);
        push_rax();
        
        generate_expression((Ast_Expression*)binary->right);
        pop_rdi();
		fprintf(file, "\tmov [rdi], rax\n");
        //store_to_rdi(expression->type);
		return;
	}
	
	generate_expression((Ast_Expression*)binary->right);
	push_rax();
	
	generate_expression((Ast_Expression*)binary->left);
	pop_rdi();
	
	switch(binary->op)
	{
		case AST_BINARY_PLUS: 
		{
			fprintf(file, "\tadd rax, rdi\n");
			break;
		}
		case AST_BINARY_MINUS: 
		{
			
			fprintf(file, "\tsub rax, rdi\n");
			break;
		}
		case AST_BINARY_MUL: 
		{
			fprintf(file, "\timul rax, rdi\n");
			break;
		}
		case AST_BINARY_DIV: 
		{
			//fprintf(file, "\tdiv rax, rdi\n");
			assert(false && "DIV operation is not handled yet");
			break;
		}
		default:
		{
			break;
		}
	}
	
}

void generate_expression(Ast_Expression* exp) {
	assert(exp); 
	assert(exp->type == AST_EXPRESSION); 
	
	if (exp->kind == AST_KIND_EXP_UNARY){
		assert(false && "Unary Operations are not supported yet");
		return;
	}
	
	if (exp->kind == AST_KIND_EXP_BINARY){
		Ast_Binary* bin = (Ast_Binary*)exp;
		generate_binary_expression(bin);
		return;
	}
	
	if (exp->kind == AST_KIND_EXP_PRIMARY)
	{
		Ast_Primary* primary = (Ast_Primary*)exp;
		if (primary->priamry_kind == AST_KIND_PRIMARY_IDENTIFIER){
			fprintf(getFile(), "\tmov rax, [%.*s] ; [Ident]\n", SV_PRINT(exp->token.name));
			// TODO: Add the ident to stack ?? 
		}
		// TODO handle strings ?? 
		else
		{
			fprintf(getFile(), "\tmov rax, %.*s ; [Literal]\n", SV_PRINT(exp->token.name));
		}
		return;
	}
	if (exp->kind == AST_KIND_EXP_LITERAL)
	{
		fprintf(getFile(), "\tmov rax, %.*s ; literal:\n", SV_PRINT(exp->token.name));
		return;
	}
	if (exp->kind == AST_KIND_EXP_RETURN)
	{
		auto _return = (Ast_Return*)exp;
		assert(arrlenu(_return->expressions) == 1 && "Multiple Return expressions are not implemented yet");
		//assert(arrlenu(_return->expressions) == 0 && "Proc Must return an expression ");
		generate_expression(_return->expressions[0]);
		//push_rax();
		return;
	}
	
}

void generate_proc(Ast_Proc_Declaration* decl) ;
void generate_decl(Ast_Declaration* decl) 
{
	if (decl->kind == AST_KIND_DECL_PROCEDURE){
		generate_proc((Ast_Proc_Declaration*) decl);
		arrput(funcs, decl->token.name);
		return ;
	}
	
	
	// Can we ignore this and add all local variables in current scope to stack ?? 
	if (decl->kind == AST_KIND_DECL_VARIABLE) {
		arrput(vars, (Ast_Var_Declaration*)decl);
		return ;
	}
	
	if (decl->kind == AST_KIND_DECL_STRUCT){
		//assert(false && "AST_KIND_DECL_STRUCT Not handled yet");
		return ;
	}
}

void generate_node(Ast_Node* node){
	
	switch(node->type)
	{
		case AST_EXPRESSION:
		{
			generate_expression((Ast_Expression*) node);
			break;
		}
		case AST_DECLARATION:
		{
			generate_decl((Ast_Declaration*) node);
			break;
		}
		case AST_TYPE:
		{
			printf("Node Type AST_IF is not supported yet\n");
			break;
		}
		case AST_BLOCK:
		{
			generate_block((Ast_Block*) node);
			break;
		}
		case AST_IF:
		{
			printf("Node Type AST_IF is not supported yet\n");
			break;
		}
		case AST_WHILE:
		{
			printf("Node Type AST_WHILE is not supported yet\n");
			break;
		}
		default:
		{
			
			printf("[Generator] Unhandled Node Type %d Token name [%.*s]\n", node->type, SV_PRINT(node->token.name));
			break;
		}
		
	}
}


void generate_block(Ast_Block* block)
{
	//printf("#Of Statements = %zd\n", arrlenu(block->statements));
	for(u64 i = 0; i < arrlenu(block->statements); i++)
	{
		generate_node(block->statements[i]);
	}
	
	//FILE* file = getFile();
	
}

void generate_proc(Ast_Proc_Declaration* decl)
{
	
	FILE * file = getFile();
	
	
	u32 frame_size = 32;
	StringView funcName = decl->token.name;
	// Generate Prework
	fprintf(file, "segment .code\n");
	fprintf(file, "func.%.*s:\n", SV_PRINT(funcName));
	fprintf(file, "\tpush rbp\n");
	fprintf(file, "\tmov rbp, rsp\n");
	fprintf(file, "\tsub rsp, %d\n", frame_size );
	
	// TODO: CleanUp
	fprintf(file, "\tcall _CRT_INIT\n");
	generate_block(decl->body);
	// Generate Work
	
	// Generate Postwork
	fprintf(file,"\tlea rcx, message\n");
	fprintf(file,"\tmov rdx, rax\n");
	fprintf(file,"\tcall printf\n");
	fprintf(file, "ende.%.*s:\n", SV_PRINT(funcName));
	fprintf(file, "\tadd rsp, %d\n",frame_size);
	fprintf(file, "\tpop rbp\n");
	fprintf(file, "\tret\n\n\n");
	
	//
	
	
	
	
	
	
	
	//close_file();
}

void 
generate(Ast_Node* node) 
{
	
	generate_node(node);
	FILE* file = getFile();
	
	fprintf(file, "\n");
	for(u64 i = 0; i < arrlenu(funcs); i++){
		fprintf(file, "global func.%.*s\n", SV_PRINT(funcs[i]));
	}
	arrfree(funcs);
	
	
	fprintf(file, "segment .data\n");
	for(u64 i = 0; i < arrlenu(vars); i++){
		printf("Adding [%.*s] \n", SV_PRINT(vars[i]->token.name));
		if (vars[i]->body)
			fprintf(file, "\t%.*s dq %.*s\n", SV_PRINT(vars[i]->token.name), SV_PRINT(vars[i]->body->token.name));
		else
			fprintf(file, "\t%.*s resq 1\n", SV_PRINT(vars[i]->token.name));
	}
	arrfree(vars);
	// TEMP: @Cleanup
	fprintf(file, "\tmessage db \'result = %%d\', 10, 13, 0\n");
}

/*
- RCX, RDX, R8, R9 are used for integer and pointer arguments in that order left to right.

- XMM0, 1, 2, and 3 are used for floating point arguments.

- Additional arguments are pushed on the stack left to right.

- Parameters less than 64 bits long are not zero extended; the high bits contain garbage.

- It is the caller‟s responsibility to allocate 32 bytes of “shadow space” (for storing RCX, RDX, R8, and R9 if needed) before calling the function.

- It is the caller‟s responsibility to clean the stack after the call.

- Integer return values (similar to x86) are returned in RAX if 64 bits or less.

- Floating point return values are returned in XMM0.

- Larger return values (structs) have space allocated on the stack by the caller, and RCX then contains a pointer to the return pace when the callee is called. Register usage for integer parameters is then pushed one to the right. RAX returns this address o the caller.

- The stack is 16-byte aligned. The “call” instruction pushes an 8-byte return value, so the all non-leaf functions must adjust the stack by a value of the form 16n+8 when allocating stack space.

- Registers RAX, RCX, RDX, R8, R9, R10, and R11 are considered volatile and must be considered destroyed on function calls.

- RBX, RBP, RDI, RSI, R12, R14, R14, and R15 must be saved in any function using them.

- Note there is no calling convention for the floating point (and thus MMX) registers.

- Further details (varargs, exception handling, stack unwinding) are at Microsoft‟s site.

*/