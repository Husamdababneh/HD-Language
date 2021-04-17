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

#include "common.h"
#include "mystring.h"


//extern constexpr int MAX_ARG_COUNT = 5 + 1; // + 1 for executable call string
#define BINARY "binary:"
#define INPUT  "input:"
#define OUTPUT "output:"

struct Arguments
{
	char* inputFile;
	char* outputFile;
	char* structureFile;
	bool isSet = false;
};

//Arguments ParseArguments(int argc, char** argv);
u64 read_entire_file(FILE* file, void** data_return);
u64 read_entire_file(const char* file, void** data_return);
u64 read_entire_file(const StringView& filename, void** data_return);

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



struct Memory {
	u64 size;
	u64 count;
	u8* data;
};


struct MemorySlot {
	u64 size;
	u8* data;
	Memory* parent;
};



