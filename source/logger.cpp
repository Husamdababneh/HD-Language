/* ========================================================================
   $File: logger.cpp
   $Date: 2020-10-13
   $Creator: Husam Dababneh
   $Description: Logger.cpp
   ========================================================================*/

#include "pch.h"
#include "logger.h"
#include "lex.h"

void Logger::print_token_pos(Token * token ) {
	putchar('[');
	for(size_t i = 0; i < prefix.count; i++) putchar(prefix[i]);
	putchar('(');
	printf("%d", token->start_position.x);
	putchar(',');
	printf("%d", token->start_position.y);
	putchar(')');
	putchar(']');
	putchar(':');
	
	putchar(' ');
}


void Logger::print_with_location(Token * token , String str, ...) {
	//print_prefix();
	print_token_pos(token);
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
				case 'x':
				{
					u32 number = va_arg(args, int);
					printf("%08X", number);
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


void Logger::print(String str, ...) {
	//print_prefix();
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
				case 'x':
				{
					u32 number = va_arg(args, int);
					printf("%08X", number);
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
				{
					if (str[i] == '%'){
						if (i + 1 < str.count && str[i+1] != '%')
							continue;
					}
					putchar(str[i]);
					break;
				}
			}
		}
		else{
			putchar(str[i]);
		}
	}
	va_end(args);
	
	return;
}

inline void Logger::print_prefix() {
	putchar('[');
	for(size_t i = 0; i < prefix.count; i++) putchar(prefix[i]);
	putchar(']');
	putchar(':');
	putchar(' ');		
}
