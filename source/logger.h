/* ========================================================================
   $File: logger.h
   $Date: 2020-10-10
   $Creator: Husam Dababneh
   $Description: Logger
   ========================================================================*/
#pragma once

#include <stdarg.h>
#include <stdio.h>
// Forward Declaration.. this should not be here i think, 
// try making this more  generic and add specialization in the lexer .. 
// this is how everything should work 
//struct Token;

//struct String;
// TODO: Change putchar to putc ?? to make the user able to output to files 
struct Logger {
	//String prefix = "Temp"_s;
	FILE * out = stdout ;
	
	Logger() = default;
	
	//inline void print_prefix();
	void print(String str, ...);
	void vprint(String str,va_list args);
	//void print_token_pos(Token * token );
	//void print_with_location(Token * token , String str, ...);
};
