/* ========================================================================
   $File: logger.h
   $Date: 2020-10-10
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Logger
   ========================================================================*/
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "common.h"

//#include "lex.h"

struct Token;

// TODO: Change putchar to putc ?? to make the user able to output to files 
struct Logger {
	String prefix;
	FILE * out;
	Logger(const char * name, FILE* stream = stdout ) {
		prefix.count = c_str_size(name);
		prefix.data  = (u8*) name;
		out = stream;
	}
	
	Logger(String name, FILE* stream = stdout ) {
		prefix = name;
		out = stream;
	}


	inline void print_prefix();
	void print(String str, ...);
	void print_token_pos(Token * token );
	void print_line(Token * token , String str, ...);
};
