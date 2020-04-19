/*
  main.cpp -- This project is aimed to create a languages which
  is capable of defineing (binray encoded data) data structures
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <any>

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

	LexerState lexer(args.structureFile);

	// This is just a test
	for(auto token = lexer.peek_next_token();
		token.Type != ETOKEN::ERROR;
		token = lexer.peek_next_token())
	{
		if (token.Type == ETOKEN::COMMENT)
			std::cout << token << "\n";
	}

}

