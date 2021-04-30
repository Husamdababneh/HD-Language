/* ========================================================================
   $File: String.cpp
   $Date: 2020-04-23
   $Creator: Husam Dababneh
   $Description: Custom String Type
   ========================================================================*/

#include "pch.h"
#include "string.h"

//typedef myString StringView;

StringView operator "" _s(const char* a, size_t s){
    return { (u8*)a, s};
}
