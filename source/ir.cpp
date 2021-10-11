/* ========================================================================
   $File: ir.cpp
   $Date: June 29th 2021 8:17 pm
   $Creator: Husam Dababneh
   $Description: 
   ========================================================================*/

#include "ir.h"

void execute(Instruction* inst);

void test_ir()
{
	Inst2OP a = {OP_BIN_ADD, 1,2};
	open_file(file, "test.ir", "wb");
	fwrite(&a, sizeof(a), 1, file);
	fclose(file);
	execute(&a);
}

void execute(Instruction* inst)
{
	switch (inst->opcode)
	{
		case OP_BIN_ADD:
		{
			Inst2OP* instruction = (Inst2OP*)inst;
			printf("[%lld] + [%lld]", instruction->operand1, instruction->operand2);
			break;
		}
		case OP_BIN_SUB:
		{
			break;
		}
		
		default:
		{
			printf("UNKNOWN Operation\n");
			exit(-1);
		}
	}
}