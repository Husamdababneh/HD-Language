/* ========================================================================
   $File: auxiliary.h
   $Date: 2020-04-05
   $Creator: Husam Dababneh
   $Description: Contains common procedures that doesn't have specific place
   ========================================================================*/
#pragma once

#include <cassert>
#include <stdio.h>
#include <string.h>


template <typename F>
struct Defer
{
	Defer(F f) : f(f) {}
	~Defer() { f(); }
	F f;
};

#define CONCAT0(a, b) a##b
#define CONCAT(a, b) CONCAT0(a, b)
#define defer(body) Defer CONCAT(defer, __LINE__)([&]() { body; })


/* 

struct Memory {
	U64 size;
	U64 count;
	U8* data;
};


struct MemorySlot {
	U64 size;
	U8* data;
	Memory* parent;
};



 */
