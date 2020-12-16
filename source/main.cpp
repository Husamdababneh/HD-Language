/* ========================================================================
   $File: main.cpp
   $Date: 2020-10-02
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/

#include "pch.h"

#include "main.h"
#include "parser.h"


constexpr bool verbos = false;
int allocation_count = 0;

void * operator new(u64 size){
	allocation_count++;
	
	return malloc(size);
}

void * operator new[](u64 size){
	allocation_count++;
	return malloc(size);
}


void Usage() {
	Logger log("USAGE"_s);
	log.print("Usage: hd.exe <filename>\n"_s);
}

int main(int argc, char ** argv)
{
	if (argc < 2){
		Usage();
		return 0;
	}
	
	String filename = {(u8*)argv[1],  strlen(argv[1])};
	parse_file(filename);
	printf("#Allocations Using New KeyWord = %d\n", allocation_count);
	
}

