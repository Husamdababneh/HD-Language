/* ========================================================================
   $File: lex.cpp
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Defines Lexer functionality & Data Structures
   ========================================================================*/ 


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <cctype>

#include "lex.h"

#include "common.h"
#include "auxiliary.h"

const char* Keywords [] = {
	"u32", "u8", "char","if", "else", "while", "for",
	"continue", "delete", "new", "InputFile", "OutputDir"
};

const char* Seperators [] = {
	";", ":", "{", "}",
	"(", ")", "[", "]",
	"..", ".",
};

const char* TokenTypeName [] = {
	"NONE", "ID", "KEYWORD", "LITERAL",
	"COMMENT", "SEPARATOR"
};


constexpr extern int KeywordsCount = sizeof(Keywords)/sizeof(const char *);

bool IsReserved(std::string& string)
{
	for(int a = 0; a < KeywordsCount; a++)
		if(string.compare(Keywords[a]) == 0)
			return true;
	return false;
}

ELITERALTYPE IsLiteral(std::string& string)
{
    // String Guaranteed .. need proccessing 
	if(string[0] == '\"'  && string[string.size() - 1] == '\"')
		return ELITERALTYPE::STRING;

	// Number (Float Or Integer)
	for(char& ch : string)
	{
		switch(ch)
		{
		  case '.':
		  default:
			  // Report Error
			  break;
		}
		// @Todo(Husam) : Report Error
		if(std::isdigit(ch) == 0)
			return ELITERALTYPE::NONE;
	}
	
	return ELITERALTYPE::NONE;
}


// @CleanUp Is this usefull ?? 
Position LexerState::get_position_from_cursor(u64 cursor)
{
	Position result = {0};

	for(u64 temp = 0; temp < cursor; temp++)
	{
		switch(input[temp])
		{
		  case '\n':
			  result.line++;
			  result.index = 0;
			  break;
		  case '\t':
			  result.index+=4;
			  break;
		  default:
			  result.index++;
			  break;
		}
	}
	
	return result;
}

u8& LexerState::peek_next_character()
{
	return input[input_cursor + 1];
}

u8& LexerState::peek_character(u64 lookAhead)
{
	return input[input_cursor + lookAhead];
}

void LexerState::eat_characters(u64 count)
{
	for(int a = 0; a < count; a++)
		eat_character();
}


void LexerState::eat_character()
{
	
	switch(input[input_cursor])
	{
	  case '\n':
		  previous_token_line_number = current_line_number;
		  current_line_number++;
		  current_char_index = 0;
		  break;
	  case '\t':
		  current_char_index+=4;
		  break;
	  default:
		  current_char_index++;
		  break;
	}
	input_cursor++;
}

LexerState::LexerState(const char * filepath)
{
	std::cout << "initing Lexer\n";
	FILE* file = fopen(filepath, "rb");
	if(!file)
	{
		std::cout << "Couldn't find file [" << filepath  << "]\n" ;
		return;
	}
	
	int length = read_entire_file(file, (void**)&input.data);
	if (length < 0)
	{
		std::cout << "Couldn't read  file: [" << filepath  << "]\n" ;
		return;
	}
	input.count = length;
	input_cursor = 0;
	current_line_number = 1;
	current_char_index = 0;
	previous_token_line_number = 0;
	fclose(file);
}

LexerState::~LexerState()
{
	std::cout << "Getting Out\n";
	delete[] input.data;
	input.data = nullptr;
}
 
Token LexerState::peek_next_token()
{
	// @TODO: init a token and return it.
	Token token;
	bool stop = false;
	if(input_cursor >= input.count)
	{
		token.Type = ETOKEN::ERROR;
		return token;
	}
	char ch = peek_character();
	switch(ch)
	{
	  case '/':
	  {
		  char next = peek_next_character();
		  u64 temp = input_cursor + 2;
		  if(next  == '/'){
			  while(input[temp] != '\n'){temp++;}
			  token.Type = ETOKEN::COMMENT;
			  token.value = String{&input[input_cursor], temp - input_cursor};
			  token.start_position = get_current_position();
			  eat_characters(temp - input_cursor -1);
			  token.end_position = get_current_position();
			  return token;
		  }else if (next == '*'){
			  while(input[temp++] != '*' );
			  if(input[temp++] != '/')
				  abort();
			  token.Type = ETOKEN::COMMENT;
			  token.value = String{&input[input_cursor], temp - input_cursor};
			  token.start_position = get_current_position();
			  eat_characters(temp - input_cursor -1);
			  token.end_position = get_current_position();
			  return token;
		  }else{
			  // @Todo: Check for other types of tokens
			  eat_characters();
		  }
		  break;
	  }
	  default:
		  eat_characters();
		  break;
	}
	
	return token;
}


std::ostream& operator<<(std::ostream& stream, Token& token)
{
	stream << TokenTypeName[(int)token.Type] << ": ";

	switch(token.Type)
	{
	  case ETOKEN::IDENT:
		  //std::cout << token.name;
		  break;
	  case ETOKEN::KEYWORD:
		  //std::cout << token.name;
		  break;
	  case ETOKEN::COMMENT:
		  stream << "Starts at {" << token.start_position.line << ", "
				 << token.start_position.index << "}" << " Ends at {"
				 << token.end_position.line << ", " << token.end_position.index
				 << "}\n" << token.value;
		  //stream << token.value;
		  break;
	  case ETOKEN::LITERAL:
	  case ETOKEN::NONE:
	  default:
		  break;
	}
	return stream;
}


std::ostream& operator<<(std::ostream& stream,String& data)
{
    u64 temp = 0;
	while(temp < data.count)
		stream << data[temp++];
	
	return stream;
}
