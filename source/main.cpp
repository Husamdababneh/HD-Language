/* ========================================================================
$File: main.cpp
$Date: 2020-10-02
$Creator: Husam Dababneh
$Description: main function
========================================================================*/

#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#pragma warning( disable : 4456 )


#include "main.h"

#include "hd/hdtest.cpp"


#include "lex.cpp"
#include "parser.cpp" 
// include "typer.cpp"
// include "ir.cpp"
// include "builder.cpp"

#pragma comment(lib, "User32.lib")

#if ARCH_AMD64 != 1
#error Sorry We only Support AMD64 Bit Architecture
#endif 



void Usage() {
	printf("Usage: hd.exe <filename>\n");
}


#define USE_REAL_MAIN 1
#if USE_REAL_MAIN == 1
int main(int argc, char ** argv)
{
	SetUnhandledExceptionFilter(unhandled_handler);
	ZoneScoped;
	
	if (argc < 2){
		Usage();
		return 0;
	}
	
	MemoryArena arena =	GET_NEW_ARENA(MB(500));

	Parser parser = {};
	LexerState  lexer  = {};
	//lexer.config.ignore_comments = true;
	lexer.filename = CStringToString(argv[1]);
	lexer.input = READ_ENTIRE_FILE(argv[1]);
	parser.lexer = lexer;
	
	
	// WHAT ??
	register_predefined_types(&arena, parser);
		
	//FrameMarkStart (sl_Parsing );
	parse(&arena, parser);
	
	
	FrameMarkEnd(sl_Parsing);

	RELEASE_ARENA(arena);	
	return 0;
}
#else



int main(int argc, char ** argv)
{
	//a();

	//exit(-1);
	for(int a = 0; a < KEYWORDS_COUNT; a++){
		String word = keywords[a];// try reference
		printf("Looking For : [%.*s], ", SV_PRINT(word));
		if (isKeyword(word))
		{
			printf("Found [%.*s]\n", SV_PRINT(word));
		}
	
	}

	
	
	/*
	auto a = djb2((unsigned char*)"Husam");
	auto b = sdbm((unsigned char*)"Husam");
	auto c = loselose((unsigned char*)"Husam");
	printf("Sizeof(int)  : %lld\n", sizeof(int));
	printf("Sizeof(long) : %lld\n", sizeof(long));
	printf("djb2(Husam)  : %#04X\n", a);
	printf("sdbm(Husam)  : %#04X\n", b);
	printf("lose(Husam)  : %#04X\n", c);
	*/
	//printf("Sizeof(int)  : %lld\n");
	//printf("Sizeof(int)  : %lld\n");
	//printf("Sizeof(int)  : %lld\n");
}
#endif

