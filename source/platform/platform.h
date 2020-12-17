/* ========================================================================
   $File: platfrom.h
   $Date: November 12th 2020 10:09 pm
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/

#pragma once

#ifdef _WIN32
#include "os_windows.h"
#else
#include "os_linux.h"
#endif
