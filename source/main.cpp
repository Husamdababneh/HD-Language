/* ========================================================================
$File: main.cpp
$Date: 2020-10-02
$Creator: Husam Dababneh
$Description: main function
========================================================================*/

#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#pragma warning( disable : 4456 )

#define HD_BUILD_INTERNAL 1

#include "main.h"
#include "base.cpp"


#include "lex.cpp"
#include "generated.cpp"

// TODO: Add all "Compiler-Specific files"
#include "parser.cpp"
#include "ir.cpp"


// TODO(Husam Dababneh): Insure that we allocate enough zero-filled memory 

#if ARCH_AMD64 != 1
#error Sorry We only Support AMD64 Bit Architecture
#endif 

int allocation_count = 0;

void Usage() {
	Logger log;
	log.prefix = "USAGE"_s;
	log.print("Usage: hd.exe <filename>\n"_s);
}


struct Dummy
{
	U64 randomi;
	F32 randomf;
};

template<typename Type>
struct Array
{
	Size size;
	Type* data;
};


int main(int argc, char ** argv)
{
	ZoneScoped;
	if (argc < 2){
		Usage();
		return 0;
	}
	
	// Allocate Enough Memory and pass it down to other systems
#if OS_WINDOWS
# if HD_BUILD_INTERNAL == 1
	const LPVOID BASE_ADDRESS = (LPVOID)TB((U64)2);
# else
	const LPVOID BASE_ADDRESS = 0;
# endif
	
	auto arena_size = MB(500);
	PTR memory = VirtualAlloc(BASE_ADDRESS, arena_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
	MemoryArena arena = InitializeMemoryArena(arena_size, memory);
	
	Parser parser = {};
	LexerState  lexer  = {};
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





/* 	
		Padding lexerStatePadding[] = {
			{ offsetof(LexerState, input), member_size(LexerState, input) },
			{ offsetof(LexerState, input_cursor), member_size(LexerState, input_cursor) },
			{ offsetof(LexerState, current_line_number), member_size(LexerState, current_line_number) },
			{ offsetof(LexerState, current_char_index), member_size(LexerState, current_char_index) },
			{ offsetof(LexerState, strings), member_size(LexerState, strings) },
			{ offsetof(LexerState, config), member_size(LexerState, config) },
		};
		
		//printf("%.*s\n",SV_PRINT("LexerState"_s));
		printf("Size of LexerState = %lld\n", sizeof(LexerState));
		
		const U64 CHACE_LINE = 64;
		// 64 size of cache lane + 2 for pretty printing 
		for(U64 a = 0; a < CHACE_LINE  ; a++){
			putchar('+');
		}
		putchar('\n');
		//printf("%+*d\n", sizeof(LexerState));
		for(U64 a = 0; a < (sizeof(lexerStatePadding)/sizeof(lexerStatePadding[0])); a++)
		{
			U64 offset = lexerStatePadding[a].offset;
			U64 size   = upper_power_of_two(lexerStatePadding[a].size);
			bool newline = (offset / 64) > 1;
			//printf("| %lld with size %lld is newline %d \n", lexerStatePadding[a].offset, lexerStatePadding[a].size, newline);
			printf("| %lld with size %lld \n", lexerStatePadding[a].offset, lexerStatePadding[a].size);
			
			if (newline)
				printf("\n");
			
			//printf("Size = %lld \n", size);
			for(U64 b = 0; b < size - 1; b++)
			{
				if (b == 0) putchar('1');
				else        putchar('0');
			}
			
			
			//if (!sameLine)
			putchar('\n');
		}
		putchar('\n');
		for(U64 a = 0; a < CHACE_LINE ; a++){
			putchar('+');
		}
		putchar('\n');
		 */


/* 	
	U64 sum = 0;
	for(U64 a = 0; a < (sizeof(lexerStatePadding)/sizeof(lexerStatePadding[0])); a++)
	{
		printf("%lld with size %lld\n", lexerStatePadding[a].offset, lexerStatePadding[a].size);
		
		sum += lexerStatePadding[a].size;
	}
	
	printf("Sum = %lld", sum);
	 */

//StringView filename = CStringToString(argv[1]);

/*

*/
//generate_proc();

