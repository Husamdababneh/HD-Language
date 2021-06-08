/* ========================================================================
   $File: logger.cpp
   $Date: 2020-10-13
   $Creator: Husam Dababneh
   $Description: Logger.cpp
   ========================================================================*/

#include "logger.h"
#include "lex.h"

void Logger::print_token_pos(Token * token ) {
	putchar('[');
	for(size_t i = 0; i < prefix.count; i++) putchar(prefix[i]);
	putchar('(');
	printf("%d", token->start_position.line);
	putchar(',');
	printf("%d", token->start_position.index);
	putchar(')');
	putchar(']');
	putchar(':');
	
	putchar(' ');
}


void Logger::print_with_location(Token * token , StringView str, ...) {
	//print_prefix();
	print_token_pos(token);
	// print body
	va_list args;
	va_start(args, str);
	vprint(str, args);
	va_end(args);
	return;
}


void Logger::print(StringView str, ...) {
	// print body
	va_list	args;
	va_start(args, str);
	vprint(str, args);
	va_end(args);
	
	return;
}

void Logger::vprint(StringView str, va_list args) {
	// @NOCHECKIN 
	// TODO: 
#if 0
	vprintf((const char*)str.data, args);
#else
	// print body
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
					meow_u128 hash = va_arg(args, meow_u128);
					u32 x = MeowU32From(hash, 3);
					printf("%08X", x);
					break;
				}
				case 'd':
				{
					long number = va_arg(args, long);
					printf("%ld", number);
					break;
				}
				case 'c':
				{
					char number = va_arg(args, char);
					printf("%c", number);
					break;
				}
				case 's':
				{
					StringView string = va_arg(args, StringView);
					
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
#endif
	return;
	
}

inline void Logger::print_prefix() {
	putchar('[');
	for(size_t i = 0; i < prefix.count; i++) putchar(prefix[i]);
	putchar(']');
	putchar(':');
	putchar(' ');		
	
}