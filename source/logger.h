/* ========================================================================
   $File: logger.h
   $Date: 2020-10-10
   $Creator: Husam Dababneh
   $Description: Logger
   ========================================================================*/
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "common.h"

struct Token;

// TODO: Change putchar to putc ?? to make the user able to output to files 
struct Logger {
	StringView prefix;
	FILE * out;
	Logger(const char * name, FILE* stream = stdout ) {
		prefix.count = strlen(name);
		prefix.data  = (u8*) name;
		out = stream;
	}
	
	Logger(StringView name, FILE* stream = stdout ) {
		prefix = name;
		out = stream;
	}
	
	
	inline void print_prefix();
	void print(StringView str, ...);
	void vprint(StringView str,va_list args);
	void print_token_pos(Token * token );
	void print_with_location(Token * token , StringView str, ...);
};
