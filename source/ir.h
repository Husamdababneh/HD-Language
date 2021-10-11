/* ========================================================================
   $File: ir.h
   $Date: June 29th 2021 8:17 pm
   $Creator: Husam Dababneh
   $Description: 
   ========================================================================*/


#ifndef IR_H
#define IR_H


// OPERATION set ?? 
enum {
	
	OP_BIN_ADD,
	OP_BIN_SUB
		
};


// 0 Operand 
struct Instruction {
	U8 opcode;
};

// 1 Operand 
struct Inst1OP: public Instruction  {
	S64 operand;
};

// 2 Operand s
struct Inst2OP : public Instruction {
	S64 operand1;
	S64 operand2;
};

#endif //IR_H
