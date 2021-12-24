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


#include <Dbghelp.h>
#pragma comment(lib, "User32.lib")

// TODO(Husam Dababneh): Insure that we allocate enough zero-filled memory 

#if ARCH_AMD64 != 1
#error Sorry We only Support AMD64 Bit Architecture
#endif 

int allocation_count = 0;

void Usage() {
	Logger log;
	//log.prefix = "USAGE"_s;
	log.print("Usage: hd.exe <filename>\n"_s);
}


void make_minidump(EXCEPTION_POINTERS* e)
{
    auto hDbgHelp = LoadLibraryA("dbghelp");
    if(hDbgHelp == nullptr)
        return;
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(pMiniDumpWriteDump == nullptr)
        return;
	
    char name[MAX_PATH];
    {
        auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(0), name, MAX_PATH);
        SYSTEMTIME t;
        GetSystemTime(&t);
        wsprintfA(nameEnd - strlen(".exe"),
				  "_%4d%02d%02d_%02d%02d%02d.dmp",
				  t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    }
	
    auto hFile = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return;
	
    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;
	
    auto dumped = pMiniDumpWriteDump(
									 GetCurrentProcess(),
									 GetCurrentProcessId(),
									 hFile,
									 //
									 MINIDUMP_TYPE(MiniDumpWithFullMemoryInfo | MiniDumpWithFullMemory),
									 e ? &exceptionInfo : nullptr,
									 nullptr,
									 nullptr);
	
    CloseHandle(hFile);
	
    return;
}

LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
{
    make_minidump(e);
    return EXCEPTION_CONTINUE_SEARCH;
}

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
void GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return;
    }
    
	
    LPSTR messageBuffer = nullptr;
	
    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    //Copy the error message into a std::string.
    //std::string message(messageBuffer, size);
    printf("messageBuffer: %s\n", messageBuffer);
    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
	
    //return message;
}


int main(int argc, char ** argv)
{
	//return 0;
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

