/* ========================================================================
   $File: auxiliary.h
   $Date: 2020-04-05
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Contains common procedures that doesn't have specific place
    ========================================================================*/
#pragma once

#include <fstream>
extern constexpr int MAX_ARG_COUNT = 3 + 1; // + 1 for executable call string
#define BINARY "binary:"
#define INPUT  "input:"
#define OUTPUT "output:"

struct Arguments
{
	bool isSet = false;
	char * inputFile;
	char * outputFile;
	char * structureFile;
};

Arguments ParseArguments(int argc, char ** argv);
int read_entire_file(FILE* file, void** data_return);
int read_entire_file(const char* file, void** data_return);	
template <typename F>
struct Defer
{
	Defer(F f): f(f) {}
	~Defer() { f(); }
	F f;
};

#define CONCAT0(a, b) a##b
#define CONCAT(a, b) CONCAT0(a, b)
#define defer(body) Defer CONCAT(defer, __LINE__)([&]() { body; })
