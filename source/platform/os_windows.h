/* date = November 12th 2020 2:03 pm */

#pragma once


#include <windows.h>


#ifdef _MSC_VER


#define fileno _fileno
#define fstat _fstat
#define stat _stat
#define open_file(handle, path, mode) FILE* handle; fopen_s(&handle, path,mode);
#define open_file_handle(handle, path, mode) fopen_s(&handle, path,mode);

#else 
#error("We only support MSVC compiler on windows");"
#endif
