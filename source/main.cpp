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


#include <meow_hash.h>

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

// static void
// PrintHash(meow_u128 Hash)
// {
//     printf("    %08X-%08X-%08X-%08X\n",
//            MeowU32From(Hash, 3),
//            MeowU32From(Hash, 2),
//            MeowU32From(Hash, 1),
//            MeowU32From(Hash, 0));
// }

// static void
// HashTestBuffer(void)
// {
//     // NOTE(casey): Make a buffer with repeating numbers.
//     int Size = 16000;
//     char *Buffer = (char *)malloc(Size);
//     for(int Index = 0;
//         Index < Size;
//         ++Index)
//     {
//         Buffer[Index] = (char)Index;
//     }
    
//     // NOTE(casey): Ask Meow for the hash
//     meow_u128 Hash = MeowHash(MeowDefaultSeed, Size, Buffer);
    
//     // NOTE(casey): Extract example smaller hash sizes you might want:
//     long long unsigned Hash64 = MeowU64From(Hash, 0);
//     int unsigned Hash32 = MeowU32From(Hash, 0);
    
//     // NOTE(casey): Print the hash
//     printf("  Hash of a test buffer:\n");
//     PrintHash(Hash);
    
//     free(Buffer);
// }


//#define print(x, ...) print(CONCAT(x, _s), ...);


void Usage() {
	Logger log("USAGE"_s);
	log.print("Usage: hd.exe <filename>\n"_s);
}

int main(int argc, char ** argv)
{

	//HashTestBuffer();
	if (argc < 2){
		Usage();
		return 0;
	}
	
	String filename = {(u8*)argv[1],  strlen(argv[1])};
	parse_file(filename);
	printf("allocation = %d\n", allocation_count);
	
}

