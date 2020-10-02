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
#include <cstdlib>

#include "lex.h"

#include "common.h"
#include "String.h"
#include "auxiliary.h"

const char* Keywords [] = {
	"u32", "u8", "char","if", "else", "while", "for",
	"continue", "delete", "new", "InputFile", "OutputDir"
};


constexpr extern int KeywordsCount = sizeof(Keywords)/sizeof(const char *);

static inline bool isWhitechar(u8 c){
	if (c == '\t' ||
		c ==  '\n' ||
		c ==  ' ')
		return true;
	return false;
}

static inline bool isAlphabet(u8 c){
	if (c >= 97 && c <= 122) return true;
	if (c >= 65 && c <= 90)	 return true;
	return false;
}

static inline bool isInLireralChar(u8 c){
	if (c >= 97 && c <= 122) return true;
	if (c >= 65 && c <= 90)	 return true;
	if (c == '_')  return true;
	if (c >= 48 && c <= 57) return true;
	return false;
}

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
inline Position LexerState::get_position_from_cursor(u64 cursor)
{
	return { current_line_number ,  current_char_index };	
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

u8 LexerState::eat_until_character()
{
	// TODO: checks
	while(true){
		auto ch = eat_character();
		if (isWhitechar(ch)) continue;
		return ch;
	}
	return 0;
}

u8 LexerState::eat_character()
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
	return input[input_cursor - 1];
}

LexerState::LexerState(const char * filepath)
{
	std::cout << "initing Lexer\n";
	FILE* file = fopen(filepath, "rb");
	if(!file)
	{
		std::cout << "Couldn't find file [" << filepath  << "]\n" ;
		exit(1);
	}
	
	int length = read_entire_file(file, (void**)&input.data);
	if (length < 0)
	{
		std::cout << "Couldn't read  file: [" << filepath  << "]\n" ;
		exit(1);
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


// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// @(TEMP):
#define SS(x) String { sizeof(x), (u8*)x }
#define ERRSTRING(x) String { sizeof(x), (u8*)x }

// Rename to eat_next_token
Token LexerState::peek_next_token()
{
	// @TODO: init a token and return it.
	
	if(input_cursor >= input.count)
		return { ETOKEN::EOFA , 0 , 0 };
	
	Token token = {};
	u8 ch = eat_character();
	int temp = input_cursor;
	token.start_position = get_current_position();
	switch(ch)
	{
		// @Todo(Husam):Handle nested multiline comments.
	  case '/':
	  {
		  u8 next = peek_character();
		  
		  if(next  == '/'){
			  token.Type = ETOKEN::COMMENT;
			  while(eat_character() != '\n');
			  token.value = String { input_cursor  - temp + 1 , &input[temp - 1]};
			  return token;
		  } else if (next == '*'){
			  token.Type = ETOKEN::MULTILINE_COMMENT;
			  int nested_level = 1;			  
			  while(input_cursor < input.count ){
				  u8 eaten = eat_character();
				  u8 next_to_be_eaten = peek_character();
				  if (eaten == '/' && next_to_be_eaten == '*'){
					  eat_character();
					  nested_level++;
				  }
				  if(eaten == '*' && next_to_be_eaten == '/'){
					  eat_character();
					  nested_level--;
					  continue;
				  }
				  if (nested_level == 0) break;	  
			  };
			  token.value = String { input_cursor  - temp + 1 , &input[temp - 1]};
			  break;	  
		  } else {
			  // division ?? 
			  token.Type = ETOKEN::ERROR;
			  eat_character();
			  token.value = String { input_cursor  - temp + 1 , &input[temp - 1]};
			  //token.value = ERRSTRING("Not an error !! :-) ");
			  break;	  
			  // eat_characters();
		  }
	  }
	  break;
	  case '+': case '-': case '*': case '{': case '}':
	  case '[': case ']': case '=': case ';': case '.':
	  case '(': case ')': case ',':
		  token.Type = (ETOKEN) ch;
		  token.value = String{1, &input[input_cursor]};
		  break;
	  case '#':
		  // Compile Time Execution operator
		  // What follows # should be executed at compile time (Just Like Jai)
		  token.Type = ETOKEN::DIRECTIVE;
		  // This is not right .. the behaviour should be to stop on first whitespace
		  // but since i'm getting used to the lexer i wanted to try some stuff - Husam 9/28/2020
		  while(true){
			  auto eaten = eat_character();
			  if(eaten == ' ' || eaten == '\t' || eaten == ';' || eaten == '\n')
				  break;
		  };
		  token.value = String { input_cursor  - temp , &input[temp - 1]};
		  break;
	  case '"':
	  {
		  while(eat_character() != '"');
		  token.Type = ETOKEN::LITERAL;
		  token.value = String{input_cursor - temp + 1, &input[temp - 1]};
		  break;	  
	  }
	  break;
	  case ':':
	  {
		  auto next = eat_until_character();
		  token.value = String{input_cursor - temp, &input[temp - 1]};
		  if (next == ':')  token.Type = ETOKEN::DOUBLECOLON; // uninitailizaed 
		  else if (next == '=')  token.Type = ETOKEN::COLONEQUAL; // initialized
		  else  {
			  token.Type = ETOKEN::COLON;
			  token.value = String{1, &input[temp - 1]};
		  } 
		  eat_character();
		  
		  break;	  
		  
	  }
	  break;
	  case ' ': case '\t': case '\n': case '\r': break;
	  default:
		  if (isAlphabet(ch)){
			  while(true){
				  auto next = eat_character();
				  if (!isInLireralChar(next))
					  break;
				  
			  };
			  token.Type = ETOKEN::LITERAL;
			  token.value = String{input_cursor - temp , &input[temp - 1]};
		  }
		  break;
	}
	token.end_position = get_current_position();	
	return token;
}


std::ostream& operator<<(std::ostream& stream, Token& token)
{

	// auto name = "Unknown";
	// if(token.Type == ETOKEN::ERROR)
	// 	name = "Error";
	
	
	switch(token.Type)
	{
	  case ETOKEN::IDENT:
		  stream << "IDENT: " << token.name << "--\n";
		  break;
	  case ETOKEN::KEYWORD:
		  stream << "KEYWORD: " << token.name << "--\n";
		  break;
	  case ETOKEN::COMMENT:
		  break;
	  case ETOKEN::MULTILINE_COMMENT:
		  // stream << "Position: "  << "samples\\sample.hd("
		  // 		 << token.start_position.line << ","
		  // 		 << token.start_position.index << ")->("
		  // 		 << token.end_position.line << ","
		  // 		 << token.end_position.index << ")\n";
		  //stream << "MULTILINE_COMMENT: \n" << token.value << "--\n";
		  break;
	  case ETOKEN::OPERATOR:
		  break;
	  case ETOKEN::LITERAL:
		  stream << "LITERAL: " << token.value << "--\n";
		  break;
		  // case ETOKEN::DEFINE:
		  // 	  break;
		  // case ETOKEN::DEFINEANDASSIGN:
		  // 	  break;
	  case ETOKEN::COLON:
		  stream << "COLON: " << token.value << "--\n";
		  break;
	  case ETOKEN::DOUBLECOLON:
		  stream << "DOUBLECOLON: " << token.value << "--\n";
		  break;
	  case ETOKEN::COLONEQUAL:
		  stream << "COLONEQUAL: " << token.value << "--\n";
		  break;
	  case ETOKEN::DIRECTIVE:
		  stream << "DIRECTIVE: " << token.value << "--\n";
		  break;
	  case ETOKEN::ERROR:
		  stream << "ERROR: " << token.value << "--\n";
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
