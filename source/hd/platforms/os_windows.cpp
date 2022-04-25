/* date = November 12th 2020 2:03 pm */

#pragma once

#ifdef _MSC_VER

#include <sys/stat.h>

#define fileno _fileno
#define fstat _fstat
#define stat _stat
#define open_file(handle, path, mode) FILE* handle; fopen_s(&handle, path,mode);
#define open_file_handle(handle, path, mode) fopen_s(&handle, path,mode);

#include <windows.h>
#include <memoryapi.h>
#include <Dbghelp.h>

#else 
#error("We only support MSVC compiler on windows");
#endif



static inline
void printSystemInfo()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo (&sysInfo);
	printf("Page size                   : %dMB\n", sysInfo.dwPageSize);
	printf("wProcessorArchitecture      : %d\n", sysInfo.wProcessorArchitecture);
	printf("lpMinimumApplicationAddress : %p\n",sysInfo.lpMinimumApplicationAddress);
	printf("lpMaximumApplicationAddress : %p\n", sysInfo.lpMaximumApplicationAddress);
	printf("dwNumberOfProcessors        : %d\n", sysInfo.dwNumberOfProcessors);
	printf("dwProcessorType             : %d\n", sysInfo.dwProcessorType);
	printf("PTR                         : %p\n", (void*)sysInfo.dwActiveProcessorMask) ;
	printf("dwNumberOfProcessors        : %d\n", sysInfo.dwNumberOfProcessors);
	printf("dwProcessorType             : %d\n", sysInfo.dwProcessorType);
	printf("dwAllocationGranularity     : %d\n", sysInfo.dwAllocationGranularity);
	printf("wProcessorLevel             : %d\n", sysInfo.wProcessorLevel);
	printf("wProcessorRevision          : %#X\n", sysInfo.wProcessorRevision);
}


static inline
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

static inline
LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
{
    make_minidump(e);
    return EXCEPTION_CONTINUE_SEARCH;
}

static inline
void PrintLastWin32Error()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return;
    }
    
	
    LPSTR messageBuffer = nullptr;
	
    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us
	// (because we don't yet know how long the message string will be).
	
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
								 FORMAT_MESSAGE_FROM_SYSTEM |
								 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
								 errorMessageID,
								 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    printf("messageBuffer: %s\n", messageBuffer);
    LocalFree(messageBuffer);
}

static inline 
String windows_read_entire_file(const char* path)
{
    // Abstract the following to separate routine

	HANDLE hFile;
	BOOL bErrorFlag = FALSE;

	hFile = CreateFileA(path,                   // name of the write
						GENERIC_READ,           // open for reading
						FILE_SHARE_READ,        // share for reading only 
						NULL,                   // default security
						OPEN_EXISTING,          // create new file only
						FILE_ATTRIBUTE_NORMAL,  // normal file
						NULL);                  // no attr. template

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// TODO: handle erros here
		PrintLastWin32Error();
		assert(false);
		exit(-1);
	}

	
	LARGE_INTEGER fileSize; // 64-Bit wide 
	if (GetFileSizeEx(hFile, &fileSize) == 0)
	{
		// TODO: handle erros here
		PrintLastWin32Error();
		assert(false);
		exit(-1);
	}
	// request memeory or pass arena ?? let's just allocate here for now
	//                        // Not sure about this part

	if(fileSize.u.HighPart != 0)
	{
		printf("File size is too big to handle");
		assert(false);
		exit(-1);
	}
	
	PTR data = VirtualAlloc(0, fileSize.QuadPart, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

	DWORD bytesRead;
	if(ReadFile(hFile, data, fileSize.u.LowPart, &bytesRead, NULL) == 0)
	{
		PrintLastWin32Error();
		assert(false);
		exit(-1);
	}
	
    CloseHandle(hFile);

	return {(U64)fileSize.u.LowPart, (char*)data};
}

static inline 
String windows_read_entire_file(String filename)
{
	if (filename.isNullTerminated()) return windows_read_entire_file(filename.str_char);
	char temp[256] = {};
	memcpy(temp, filename.str_char, filename.size);
	temp[filename.size] = 0;
	return 	windows_read_entire_file(temp);
}


static inline
void windows_free_filedata(String filedata)
{
	// TODO: check for errors
	VirtualFree((PTR)filedata.str, 0, MEM_RELEASE);
}


// TODO: Better Error handleing 
static inline PTR
windows_request_memory(Size size, LPVOID baseAddress)
{
	PTR memory = VirtualAlloc(baseAddress, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if (memory == nullptr) 
	{
		PrintLastWin32Error();
		exit(-1);
	}
	return memory;
}

static inline 
void windows_release_arena(MemoryArena arena)
{
	// TODO: check for errors
	VirtualFree(arena.base, 0, MEM_RELEASE);
}



