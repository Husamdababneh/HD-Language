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
#include "String.h"
#include "auxiliary.h"

const char* Keywords [] = {
	"u32", "u8", "char","if", "else", "while", "for",
	"continue", "delete", "new", "InputFile", "OutputDir"
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
		if(std::isdigit((unsigned char)ch) == 0)
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

u8& LexerState::peek_character(u64 lookAhead /* = 0 */)
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
	cursor.string = &input;
	cursor.cursor = 0;
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
	
	if(input_cursor >= input.count)
		return {ETOKEN::ERROR , 0 , 0};
	
	Token token;
	u8 ch = peek_character();

	switch(ch)
	{
		// @Todo(Husam):Handle nested multiline comments.
	  case '/':
	  {
		  u8 next = peek_next_character();
		  u64 temp = input_cursor + 2;
		  if(next  == '/'){
			  while(input[temp] != '\n'){temp++;}
			  token.Type = ETOKEN::COMMENT;
			  token.value = String{temp - input_cursor, &input[input_cursor]};
			  token.start_position = get_current_position();
			  eat_characters(temp - input_cursor -1);
			  token.end_position = get_current_position();
			  return token;
		  }else if (next == '*'){
			  while(input[temp++] != '*' );
			  if(input[temp++] != '/')
				  abort();
			  token.Type = ETOKEN::MULTILINE_COMMENT;
			  token.value = String{temp - input_cursor, &input[input_cursor]};
			  token.start_position = get_current_position();
			  eat_characters(temp - input_cursor - 1);
			  token.end_position = get_current_position();
			  return token;
		  }else{
			  // @Todo: Check for other types of tokens.
			  eat_characters();
		  }
	  }
	  break;
	  case '+':  case '-':  case '*':
		  token.Type = (ETOKEN) ch;
		  token.value = String{1, &input[input_cursor]};
		  token.start_position = get_current_position();
		  eat_character();
		  token.end_position = get_current_position();
		  break;
	  case '#':
		  // Compile Time Execution operator
		  // What follows # should be executed at compile time (Just Like Jai)
		  eat_character();		// this is here so we don't break the lexer for now
		  break;
	  case '"':
	  {
		  u64 temp = input_cursor + 1;
		  //u64 index = find_first_character('"');
		  while(input[temp++] != '"');//{temp++;}
		  token.Type = ETOKEN::LITERAL;
		  token.value = String{temp - input_cursor - 2, &input[input_cursor+1]};
		  token.start_position = get_current_position();
		  eat_characters(temp - input_cursor);
		  token.end_position = get_current_position();
		  return token;
	  }
	  break;
	  case ':':
	  {
		  u8 next = peek_next_character(); 
		  if(next == ':') { // "define" TOKEN :
			  u64 temp = input_cursor + 2;
			  token.Type = ETOKEN::DEFINE;
			  token.value = String{2, &input[input_cursor]};
			  token.start_position = get_current_position();
			  eat_characters(2);
			  token.end_position = get_current_position();
			  return token;
		  } else if(next == '=')  {				// "define and assign" TOKEN :=
			  u64 temp = input_cursor + 2;
			  token.Type = ETOKEN::DEFINEANDASSIGN;
			  token.value = String{2, &input[input_cursor]};
			  token.start_position = get_current_position();
			  eat_characters(2);
			  token.end_position = get_current_position();
			  return token;
		  }
		  eat_character();
	  }
	  break;
	  case ' ': case '\t':
		  eat_character();
		  break;
	  default:
		  eat_character();
		  break;
	}
	
	return token;
}


std::ostream& operator<<(std::ostream& stream, Token& token)
{

	auto& name = magic_enum::enum_name(token.Type); 
	u64 size =  name.size();
	stream << name << ": "
		   << "samples\\sample.hd(" << token.start_position.line << ","
		   << token.start_position.index << ")\n";
	
	switch(token.Type)
	{
	  case ETOKEN::IDENT:
		  //std::cout << token.name;
		  break;
	  case ETOKEN::KEYWORD:
		  //std::cout << token.name;
		  break;
	  case ETOKEN::COMMENT:
	  case ETOKEN::MULTILINE_COMMENT:
		  break;
	  case ETOKEN::OPERATOR:
	  case ETOKEN::LITERAL:
	  case ETOKEN::DEFINE:
	  case ETOKEN::DEFINEANDASSIGN:
		  stream// << std::string(size, ' ')
			  << token.value << '\n';
		  break;
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
	{
		if (data[temp] == '\r')
		{
			temp++;
			continue;
		}
		stream << data[temp++];
	}
	
	return stream;
}
