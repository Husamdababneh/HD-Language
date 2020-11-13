/* date = November 12th 2020 2:03 pm */

#pragma once


#include <windows.h>


#ifdef _WIN32
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif
