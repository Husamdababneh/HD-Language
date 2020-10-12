/* ========================================================================
   $File: main.cpp
   $Date: 2020-10-02
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: main function
   ========================================================================*/

#include <stdio.h>

#include "main.h"
#include "lex.h"
#include "auxiliary.h"
#include "common.h"
#include "logger.h"

constexpr bool verbos = false;


int main(int argc, char ** argv)
{
	Arguments args = ParseArguments(argc, argv);
	// NOTE(Husam):  ParseArguments should report the errors.
	if(!args.isSet)
		return -1;

	int b = 101;
	Logger logger("Husam");
	String str = "\n\tThis %% is a beautiful string!!\n"_s;
	logger.print("hey %u %s\n"_s,  b, str);
	LexerState lexer(args.structureFile);
	int a = 0;
	// This is just a test
	for(auto token = lexer.peek_next_token();
		token.Type != ETOKEN::EOFA;
		token = lexer.peek_next_token())
	{
		//outfile << token << "\n";
		//std::cout  << token;
		a++;
	}
	printf("# Tokens : %d\n",  a );

	//outfile.close();
	
}

