/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Here are some data structures represents the lexer
   ========================================================================*/

#pragma once
#include <string>
#include "common.h"

extern const char* Keywords[];
extern const char* TokenTypeName[];


enum class ELITERALTYPE 
{
	NONE = 0,  // Error
	STRING,
	INTEGER,
	FLOAT
};

enum class  ETOKEN
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
		String name;
		bool boolean;
		int integer;
		float32 float32;
		float64 float64;
		String value;
	};
};

struct LexerState
{
	LexerState(const char * filepath);
	~LexerState();

	String input;				// Data
	int input_cursor;			// to keep track were we are


	int current_line_number;
	int current_char_index;
	int previous_token_line_number;
	//int numberOfTokens;


	Token peek_next_token();
	Token peek_token(int lookAhead);
	void eat_token(int count = 0);
	

	inline u8& peek_next_character();
	inline u8& peek_character(int lookAhead = 0);
	inline void eat_character();
	
	LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor
};


std::ostream& operator<<(std::ostream& stream, Token& token);
std::ostream& operator<<(std::ostream& stream,String& data);

