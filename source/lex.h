/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Here are some data structures represents the lexer
   ========================================================================*/

#pragma once
#include <string>

extern const char* Keywords[];
extern const char* TokenTypeName[];

enum ETOKEN
{
	NONE = 0,
	IDENT,
	KEYWORD,
	LITERAL,
	COMMENT,
};

struct Token
{
	ETOKEN Type = ETOKEN::NONE;
	union
	{
		bool boolean;
		int value;
		char strValue[1024]; // ?????
	};
};

struct LexerState
{
	std::fstream file; // to keep track were we are
	int numberOfTokens; // statistics ??
};

Token getNextToken(LexerState& state);
bool initLexer(LexerState& state, const char * file);
void finitLexer(LexerState& state);
std::ostream& operator<<(std::ostream& stream, Token& token);
