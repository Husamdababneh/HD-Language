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

void consumeComment(LexerState* state)
{
	char ch = state->input.data[state->input_cursor];
	if(ch == '*')
	{
		state->input_cursor++;
		while(true)
		{
			if(state->input.data[state->input_cursor++] == '*')
			{
				if(state->input.data[state->input_cursor] == '/')
					state->input_cursor++;
				// report error
				//else
				break;
			}
		}		  
	}
	else if(ch == '/')
	{
		state->input_cursor++;
		while(true)
		{
			if(state->input.data[state->input_cursor++] == '\n')
				break;
		}
	}
	else
	{
		//TODO: report error 
	}
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
		if(std::isdigit(ch) == 0)
			return ELITERALTYPE::NONE;
	}
	
	return ELITERALTYPE::NONE;
}

Token CreateToken(std::stringstream& string)
{
	// Check if it's reserved ?
	Token token;
	if(IsReserved(string.str()))
	{
		token.Type  =  ETOKEN::KEYWORD;
	}
	else if (string.str().c_str()[0] == '/')
	{
		token.Type  =  ETOKEN::COMMENT;

	}
	if (IsLiteral(string.str()) != ELITERALTYPE::NONE)
	{
		token.Type  =  ETOKEN::COMMENT;
	}
	else
	{
		token.Type  =  ETOKEN::IDENT;
	}
	return token;
	
}
#ifdef WIN32
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif
static int read_entire_file(FILE* file, void** data_return)
{
	assert(file);
    int descriptor = fileno(file);

    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1) return -1;

    int length = file_stats.st_size;

    unsigned char *data = new unsigned char[length];

    fseek(file, 0, SEEK_SET);
    int success = fread((void *)data, length, 1, file);
    if (success < 1) {
        delete [] data;
        return -1;
    }

    *data_return = data;
    return length;
}

static int read_entire_file(const char *filepath , void** data_return)
{
	
	FILE* file = fopen(filepath, "rb");
	if(!file)
	{
		std::cout << "Couldn't find file [" << file  << "]\n" ;
		return false;
	}
	assert(file);
    int descriptor = fileno(file);

    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1) return -1;

    int length = file_stats.st_size;

    unsigned char *data = new unsigned char[length];

    fseek(file, 0, SEEK_SET);
    int success = fread((void *)data, length, 1, file);
    if (success < 1) {
        delete [] data;
        return -1;
    }

    *data_return = data;
    return length;
}

#ifdef WIN32
#undefine fileno
#undefine fstat 
#undefine stat
#endif

u8& LexerState::peek_next_character()
{
	return input[input_cursor + 1];
}

u8& LexerState::peek_character(int lookAhead)
{
	return input[input_cursor + lookAhead];
}

void LexerState::eat_character()
{
	input_cursor++;
	if(input[input_cursor] == '\n')
	{
		previous_token_line_number = current_line_number;
		current_line_number++;
		current_char_index = 1;
	}
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
	current_line_number = 0;
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
	assert(input.count > input_cursor);
	char ch = peek_character();
	switch(ch)
	{
	  case '/':
	  {
		  char next = peek_next_character();
		  int temp = input_cursor + 2;
		  if(next  == '/'){
			  while(input[temp++] != '\n');
			  token.Type = ETOKEN::COMMENT;
			  token.value = String{&input[input_cursor], (u64)temp - input_cursor - 1 };
			  input_cursor = temp;
			  return token;
		  }else if (next == '*'){
			  while(input[temp++] != '*' );
			  if(peek_character(temp - input_cursor) != '/')
				  abort();
			  temp++;
			  token.Type = ETOKEN::COMMENT;
			  token.value = String{&input[input_cursor], (u64)temp - input_cursor};
			  input_cursor = temp;
			  return token;
		  }else{
			  // @Todo: Report Errors.:
		  }
		  break;
	  }
	  case ':' : case '{': case '}':
	  case '[' : case ']': case '#':
	  case '"' : case '.': case '=':
	  case '(' : case ')': case ',':
	  case '\t': case ' ': case ';':
	  case '\n':
		  // [[fallthrough]]
	  default:
		  input_cursor++;
		  break;
	}
	
	return token;
}


std::ostream& operator<<(std::ostream& stream, Token& token)
{
	// @Todo:  
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
		  stream << token.value;
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
    u8 temp = 0;
	while(temp < data.count)
		stream << data[(int)temp++];
	
	return stream;
}
