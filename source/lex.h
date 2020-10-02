/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Here are some data structures represents the lexer
   ========================================================================*/

#pragma once
#include <string>

#include "String.h"

extern const char* Keywords[];
extern const char* TokenTypeName[];


enum class ELITERALTYPE 
{
	NONE = 0,  // Error
	STRING,
	INTEGER,
	FLOAT
};

enum class  ETOKEN : u64
{
	ZERO = 0,
	IDENT = 256,
	KEYWORD,
	LITERAL,
	COMMENT,
	MULTILINE_COMMENT,
	/*
	DEFINE,
	DEFINEANDASSIGN,
	*/
	COLON,
	DOUBLECOLON,
	COLONEQUAL,
	OPERATOR,
	DIRECTIVE,
	NONE,
	// EOF Not working
	EOFA,
	ERROR
};

struct Position
{
	u64 line, index;
};

struct Token
{
	ETOKEN Type = ETOKEN::NONE;

	Position start_position;
	Position end_position;
	
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
	u64 input_cursor;			// to keep track were we are
	StringView cursor;			// new cursor type 

	u64 current_line_number;
	u64 current_char_index;
	u64 previous_token_line_number;
	//int numberOfTokens;


	Token peek_next_token();

	Token peek_token(int lookAhead);
	void eat_token(int count = 0);
	

	inline u8& peek_next_character();
	inline u8& peek_character(u64 lookAhead = 0);
	
	inline u8  eat_character();
	inline u8 eat_until_character();
	inline void eat_characters(u64 count = 1);
	

	Position get_current_position() { return {current_line_number, current_char_index}; }
	Position get_position_from_cursor(u64 cursor);
	
	LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor

};

// @Todo(Husam): I should create my own printing function....
std::ostream& operator<<(std::ostream& stream, Token& token);
std::ostream& operator<<(std::ostream& stream,String& data);

