/*
  main.cpp -- This project is aimed to create a languages which
  is capable of defineing (binray encoded data) data structures and proccess on them
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
	std::fstream outfile("tokens.txt", std::ios::out| std::ios::binary);
	// This is just a test
	for(auto token = lexer.peek_next_token();
		token.Type != ETOKEN::ERROR;
		token = lexer.peek_next_token())
	{
		if ((token.Type != ETOKEN::NONE) &&
			!(token.Type == ETOKEN::COMMENT ||
			 token.Type == ETOKEN::MULTILINE_COMMENT))
		{
			outfile << token << "\n";
			std::cout  << token << "\n";
		}
	}
}

