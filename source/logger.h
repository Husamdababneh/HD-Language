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
#include "String.h"
#include "common.h"




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
	void print(String str, ...) {
		// print header
		putchar('[');
		for(size_t i = 0; i < prefix.count; i++) putchar(prefix[i]);
		putchar(']');
		putchar(':');
		putchar(' ');
		
		// print body
		va_list	args;
		va_start(args, str);
		for(size_t i = 0; i < str.count; i++) {
			if (str[i] == '%'){
				i++;
				switch (str[i]){
				  case 'u':
				  {
					  unsigned long number = va_arg(args, unsigned long);
					  printf("%ld", number);
					  break;
				  }
				  case 'd':
				  {
					  long number = va_arg(args, long);
					  printf("%ld", number);
					  break;
				  }
				  case 's':
				  {
					  String string = va_arg(args, String);
					  for(int a = 0; a < string.count; a++)
						  putc(string[a], out);
					  break;
				  }
				  default:
					  if (str[i] == '%'){
						  if (i + 1 < str.count && str[i+1] != '%')
							  continue;
					  }
					  putchar(str[i]);
					  break;
				}
			}
			else{
				putchar(str[i]);
			}
			
		}
		va_end(args);
		
		return;
	}	
};
