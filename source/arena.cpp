/* ========================================================================
$File: arena.cpp
$Date: December 9th 2020 1:21 am 
$Creator: Husam 
$Desc:  
=========================================================================*/

#include "pch.h" 
#include "arena.h"

Arena 
make_arena(u64 size)
{
	Arena result;
	result.data = new u8[size];
	result.cursor = result.data;
	result.cap = size;
	memset((void*)result.data, 0, result.cap);
	return result;
}

