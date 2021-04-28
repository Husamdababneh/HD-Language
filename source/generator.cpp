#include "pch.h"

#include "ast.h"


void generate_proc(Ast_Declaration* decl)
{
	
	open_file(file, "first.asm", "wb");
	
	
	u32 frame_size = 32;
	StringView funcName = decl->token.name;
	// Generate Prework
	fprintf(file, "func.%.*s:\n", (int)funcName.count, funcName.str);
	fprintf(file, "\tpush rbp\n");
	fprintf(file, "\tmov rbp, rsp\n");
	fprintf(file, "\tsub rsp, %d\n", frame_size );
	
	
	// Generate Work
	
	// Generate Postwork
	fprintf(file, "ende.%.*s:\n", (int)funcName.count, funcName.str);
	fprintf(file, "\tadd rsp, %d\n",frame_size);
	fprintf(file, "\tpop rbp\n");
	fprintf(file, "\tret\n");
	fclose(file);
	
}

/* 
static open_file(outfile, "first.asm", "wb");
void generate_binary_expression(Ast_Binary* binary)
{
	//open_file(file, "first.asm", "wb");
	
	
	sprintf(outfile, );
	
	
	
	//fclose(file);
}
 */