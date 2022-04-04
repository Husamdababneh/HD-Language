/* ========================================================================
$File: main.cpp
$Date: 2020-10-02
$Creator: Husam Dababneh
$Description: main function
========================================================================*/

#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#pragma warning( disable : 4456 )


#include "base.cpp"
#include "lex.cpp"

#include "parser.cpp" 

#define HD_BUILD_INTERNAL 1

#pragma comment(lib, "User32.lib")

#if ARCH_AMD64 != 1
#error Sorry We only Support AMD64 Bit Architecture
#endif 


int allocation_count = 0;


void Usage() {
	printf("Usage: hd.exe <filename>\n");
}

#include "hd_hashtable.h"

int main(int argc, char ** argv)
{
	SetUnhandledExceptionFilter(unhandled_handler);
	ZoneScoped;
	if (argc < 2){
		Usage();
		return 0;
	}
	
	//GetLastErrorAsString(487);
	//return 1;
	// Allocate Enough Memory and pass it down to other systems
#if OS_WINDOWS
# if HD_BUILD_INTERNAL == 0
	const LPVOID BASE_ADDRESS = (LPVOID)TB((U64)2);
# else
	const LPVOID BASE_ADDRESS = 0;
# endif
	
	auto arena_size = MB(500);
	PTR memory = VirtualAlloc(BASE_ADDRESS, arena_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
	if (memory == nullptr) 
	{
		GetLastErrorAsString();
		exit(-1);
	}
	
	MemoryArena arena = InitializeMemoryArena(arena_size, memory);
	
	
	//printf("Parsing %s\n", argv[1]);
	Parser parser = {};
	LexerState  lexer  = {};
	lexer.config.ignore_comments = true;
	lexer.filename = CStringToString(argv[1]);
	lexer.input = read_entire_file(argv[1]);
	parser.lexer = lexer;
	
	
	// WHAT ??
	register_predefined_types(&arena, parser);
	
	
	
	//FrameMarkStart (sl_Parsing );
	parse(&arena, parser);
	
	
	FrameMarkEnd(sl_Parsing);
	//printf("#Allocations Using New Keyword = %d\n", allocation_count);
	U64 result = VirtualFree(memory, 0, MEM_RELEASE);
	assert(result != 0);
# else
#endif
	return 0;
}


