/* ========================================================================
   $File: common.h
   $Date: 2020-04-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Common Definitions!! 
   ========================================================================*/
#pragma once

#include <cassert>
#include <iostream>
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef signed char       s8;
typedef signed short      s16;
typedef signed int        s32;
typedef signed long long  s64;

typedef float  float32;
typedef double float64;

struct myString
{
	u8* data;
	u64 count;
	u8& operator[]  (int i) {
		assert(i < count);
		return data[i];
	}

};
typedef myString String;


