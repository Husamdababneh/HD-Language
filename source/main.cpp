/* ========================================================================
$File: main.cpp
$Date: 2020-10-02
$Creator: Husam Dababneh
$Description: main function
========================================================================*/

#include "main.h"
#include "base.h"


#include "lex.cpp"
#include "generated.cpp"
// TODO: Add all "Compiler-Specific files"
#include "parser.cpp"


int allocation_count = 0;


void* operator new(u64  count)
{
	allocation_count++;
	void* ptr = malloc(count);
	TracyAlloc (ptr , count);
	return ptr;
}

void * operator new[](u64 count){
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




void Usage() {
	Logger log("USAGE"_s);
	log.print("Usage: hd.exe <filename>\n"_s);
}

int main(int argc, char ** argv)
{
	ZoneScoped;
	if (argc < 2){
		Usage();
		return 0;
	}
	//FrameMarkStart (sl_Parsing );
	
	
	//__debugbreak();
	Parser::register_predefined_types();
	StringView filename = {(u8*)argv[1],  strlen(argv[1])};
	Parser parser(filename, filename, true);
	parser.parse();
	parser.free();
	
	//generate_proc();
	
	//FrameMarkEnd(sl_Parsing);
	//printf("#Allocations Using New Keyword = %d\n", allocation_count);
	
	return 0;
}

