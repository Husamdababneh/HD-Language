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

constexpr bool verbos = false;
// @Robusstness(Husam):Very error prone


int main(int argc, char ** argv)
{

	Arguments args = ParseArguments(argc, argv);

	// NOTE(Husam):  ParseArguments should report the errors.
	if(!args.isSet)
		return -1;

	Parse
	LexerState lexer;
	initLexer(lexer, args.structureFile);
	auto token = getNextToken(lexer);
	while(token.Type != ETOKEN::NONE)
	{
		std::cout << token << "\n";
		token = getNextToken(lexer);
	}

	
	defer(finitLexer(lexer));

}

