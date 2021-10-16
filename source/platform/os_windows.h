/* date = November 12th 2020 2:03 pm */

#pragma once

#ifdef _MSC_VER


#define fileno _fileno
#define fstat _fstat
#define stat _stat
#define open_file(handle, path, mode) FILE* handle; fopen_s(&handle, path,mode);
#define open_file_handle(handle, path, mode) fopen_s(&handle, path,mode);

#include <windows.h>
#include <memoryapi.h>

#else 
#error("We only support MSVC compiler on windows");"
#endif


#if 0

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

#endif