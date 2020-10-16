/* ========================================================================
   $File: main.cpp
   $Date: 2020-10-02
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/


/*
  @Todo(): Parse comand line argument manualy using  GetCommandLineA() in Windows.h
*/


#include <stdio.h>

#include "main.h"
#include "lex.h"
#include "parser.h"
#include "auxiliary.h"
#include "common.h"
#include "logger.h"


#include "array.h"

constexpr bool verbos = false;
int allocation_count = 0;

void * operator new(u64 size){
	allocation_count++;
	return malloc(size);
}


//#define print(x, ...) print(CONCAT(x, _s), ...);

int main(int argc, char ** argv)
{
	int a = strlen(argv[1]);
	String filename = {(u8*)argv[1],  strlen(argv[1])};
	parse_file(filename);
	
	printf("allocation = %d\n", allocation_count);
	
}

