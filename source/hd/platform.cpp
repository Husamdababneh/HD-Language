/* ========================================================================
   $File: platfrom.h
   $Date: November 12th 2020 10:09 pm
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/

#pragma once

// String read_entire_file(FILE* file, void** data_return);
// String read_entire_file(const char* file, void** data_return);
// String read_entire_file(const StringView& filename, void** data_return);
#include "hdbase.h"

#ifdef OS_WINDOWS
#include "platforms/os_windows.cpp"
#elif OS_LINUX
#include "platforms/os_linux.h"
#else
#errror Unsupported Operating System
#endif


/*
 This should be platform agnostic interface
 it should deal with
 - Requesting memory from OS
 - Handles IO operation -> mainly reading files to StringViews 

 *** IT IS YOUR RESPONSABILTTY TO CLEAN WHAT YOU GET FROM THE PLATFORM ***

*/


// Consider This at some point 
/*
#if OS_WINDOWS
# if HD_BUILD_INTERNAL == 0
	const LPVOID BASE_ADDRESS = (LPVOID)TB((U64)2);
# else
	const LPVOID BASE_ADDRESS = 0;
# endif
*/

#ifdef OS_WINDOWS
#define GET_NEW_ARENA_AT_ADDRESS(size, baseAdd) InitializeMemoryArena(size,windows_request_memory(size, baseAdd));
#define GET_NEW_ARENA(size) GET_NEW_ARENA_AT_ADDRESS(size, 0)
#define RELEASE_ARENA(arena) windows_release_arena(arena)

#define READ_ENTIRE_FILE(filename) windows_read_entire_file(filename)
#define FREE_FILE_DATA(filedata) windows_free_filedata(filedata)
#else
#errror Unsupported Operating System
#endif


// END OF INTERFACE
// WRITE YOUR CODE BELLOW THIS LINE


