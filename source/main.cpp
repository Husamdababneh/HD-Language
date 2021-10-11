/* ========================================================================
$File: main.cpp
$Date: 2020-10-02
$Creator: Husam Dababneh
$Description: main function
========================================================================*/

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


void* operator new(U64  count);
void * operator new[](U64 count);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

void Usage() {
	Logger log;
	log.prefix = "USAGE"_s;
	log.print("Usage: hd.exe <filename>\n"_s);
}

struct CompilerState 
{
	Memory main_memory; // this is where the AST lives ? 
	//Memory temp_memory;
	
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
	
	CompilerState state = {};
	Memory& memory = state.main_memory;
	
	memory.memory_size = MB(500);
	memory.storage     = VirtualAlloc(0, memory.memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
	
	
	StringView filename = CStringToString(argv[1]);
	Parser parser = {};
	parser.lexer.input = readEntireFileToStringView(filename);
	register_predefined_types(parser);
	//FrameMarkStart (sl_Parsing );
	parse(parser);
	
	
	
	
	
	
	
	FrameMarkEnd(sl_Parsing);
	//printf("#Allocations Using New Keyword = %d\n", allocation_count);
	
#else
#endif
	return 0;
}




void* operator new(U64  count)
{
	allocation_count++;
	void* ptr = malloc(count);
	TracyAlloc (ptr , count);
	return ptr;
}

void * operator new[](U64 count){
	allocation_count++;
	void* ptr = malloc(count);
	TracyAlloc (ptr , count);
	return ptr;
}



void operator delete(void* ptr) noexcept
{
	TracyFree (ptr);
	free(ptr);
}


void operator delete[](void* ptr) noexcept
{
	TracyFree (ptr);
	free(ptr);
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

