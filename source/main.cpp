/*
  main.cpp -- This project is aimed to create a languages which
  is capable of defineing (binray encoded data) data structures and proccess on them
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>


#include "main.h"
#include "lex.h"
#include "auxiliary.h"
#include "common.h"


constexpr bool verbos = false;
// @Robusstness(Husam):Very error prone


int main(int argc, char ** argv)
{
	Arguments args = ParseArguments(argc, argv);
	// NOTE(Husam):  ParseArguments should report the errors.
	if(!args.isSet)
		return -1;


	//std::clock_t c_start = std::clock();
    
	
	LexerState lexer(args.structureFile);
	//std::fstream outfile("tokens.txt", std::ios::out| std::ios::binary);
	auto t_start = std::chrono::high_resolution_clock::now();
	// This is just a test
	for(auto token = lexer.peek_next_token();
		token.Type != ETOKEN::EOFA;
		token = lexer.peek_next_token())
	{
		//outfile << token << "\n";
		std::cout  << token;

	}
	
	//std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();
	
	// std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
    //           << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n";
	
	std::cout << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
			  << " ms\n";
	//outfile.close();
	
}

