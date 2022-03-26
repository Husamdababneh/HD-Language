/* ========================================================================
   $File: platfrom.h
   $Date: November 12th 2020 10:09 pm
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/

#pragma once

#ifdef OS_WINDOWS
#include "os_windows.h"
#elif OS_LINUX
#include "os_linux.h"
#else
#errror Unsupported Operation System
#endif
