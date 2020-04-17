/* ========================================================================
   $File: auxiliary.cpp
   $Date: 2020-04-05
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: see auxiliary.h
   ========================================================================*/
#include "auxiliary.h"

#include <iostream>

Arguments ParseArguments(int argc, char ** argv)
{
	Arguments args;
	if(argc > MAX_ARG_COUNT)
		return args;
	
    // TODO@TODO(Husam): Report error
	int haha = 3;
	for(int a = 1; a < argc; a++)
	{

		//std::cout << "Proccessing " << argv[a] << "\n";
		if(argv[a][0] != '-')
			return {};

		if(char *sub = strstr(argv[a],INPUT);sub)
		{
			args.inputFile = &sub[strlen(INPUT)];
			haha--;
		}

		if(char *sub = strstr(argv[a],OUTPUT);sub)
		{
			args.outputFile = &sub[strlen(OUTPUT)];
			haha--;
		}
		// we are looking to parse this file
		if(char *sub = strstr(argv[a],BINARY);sub)
		{
			args.structureFile = &sub[strlen(BINARY)];
			haha--;
		}
	}
	if(haha < 3)
		args.isSet = true;
	else
	{
		std::cout << "Usage : " << argv[0]
				  << " -inputfile:<inputfilename> "
				  << " -outputfile:<outputfilename> "
				  << " -binary:<binarystructurefilename> \n";
	}
	return args;
}
