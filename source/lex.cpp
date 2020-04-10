/*
  lex.cpp -- 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include "lex.h"

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

void consumeComment(std::fstream& file, std::stringstream& string)
{
	if(file.peek() == '*')
	{
		file.get();
		while(true)
		{
			char ch = file.get();
			if(ch == '*')
			{
				if(file.peek() == '/')
					file.get();
				// report error
				//else
				break;
			}
			else
				string << ch;
		}		  
	}
	else if(file.peek() == '/')
	{
		file.get();
		while(true)
		{
			char ch = file.get();
			if(ch == '\n')
				break;
			string << ch;
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

Token CreateToken(std::stringstream& string)
{
	// Check if it's reserved ?
	Token token;
	if(IsReserved(string.str()))
	{
		token.Type  =  ETOKEN::KEYWORD;
		token.value = 0;
	}
	else if (string.str().c_str()[0] == '/')
	{
		token.Type  =  ETOKEN::COMMENT;
		memcpy(token.strValue,string.str().c_str(), strlen(string.str().c_str()));
		token.strValue[strlen(string.str().c_str())] = '\0';
	}
	else
	{
		token.Type  =  ETOKEN::IDENT;
		memcpy(token.strValue,string.str().c_str(), strlen(string.str().c_str()));
		token.strValue[strlen(string.str().c_str())] = '\0';
	}

	return token;
	
}

bool initLexer(LexerState& state, const char * file)
{
	std::cout << "initing Lexer\n";
	state.file = std::fstream(file,std::ios::in);
	if(!state.file.is_open())
	{
		std::cout << "Couldn't find file [" << file  << "]\n" ;
		return false;
	}

	state.numberOfTokens = 0;
	return true;
}

void finitLexer(LexerState& state)
{
	std::cout << "Getting Out\n";
	if(state.file.is_open())
		state.file.close();
	else
		std::cout << "This must Not Happen EVER ???\n";
}

Token getNextToken(LexerState& state)
{
	assert(("No file associated with the lexer", state.file.is_open()));
	// @TODO: init a token and return it.
	auto& file = state.file;
	std::stringstream lexeme;
	Token token;
	while(!file.eof())
	{
		// Should we peek before consume ? 
		char ch = file.get();
		switch(ch)
		{
		  case '/':
			  if(auto next = file.peek();
				 next  == '/' || next == '*')
			  {
				  
				  // Return value ?
				  // if this faild then that comment is writen wrong 
				  consumeComment(file,lexeme); 
				  token = CreateToken(lexeme);
				  token.Type = ETOKEN::COMMENT;
				  return token;
			  }
			  break;
		  case ':' : case '{': case '}':
		  case '[' : case ']': case '#':
		  case '"' : case '.': case '=':
		  case '(' : case ')': case ',':
		  case '\t': case ' ': case ';':
		  case '\n':
			  if(lexeme.str() != "")
			  {
				  token = CreateToken(lexeme);
				  //std::cout << TokenTypeName[token.Type] << ": " << lexeme.str();
				  return token;
			  }
			  lexeme.str("");
			  break;
		  default:  
			  lexeme << ch;
			  break;
		}
	}
	return token;
}

Token peekToken(LexerState& state)
{
	auto& pos = state.file.tellg();
	auto token = getNextToken(state);
	state.file.seekg(pos);
	return token;
}

std::ostream& operator<<(std::ostream& stream, Token& token)
{
	// @Todo:  
	stream << TokenTypeName[token.Type] << ": ";

	switch(token.Type)
	{
	  case ETOKEN::IDENT:
		  std::cout << token.strValue;
		  break;
	  case ETOKEN::KEYWORD:
		  std::cout << token.strValue;
		  break;
	  case ETOKEN::COMMENT:
		  std::cout << token.strValue;
		  break;
	  case ETOKEN::LITERAL:
		  //std::cout << token.strValue << "\n";
	  case ETOKEN::NONE:
	  default:
		  break;
	}
	return stream;
}
