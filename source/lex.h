/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once
#include <string>

#include "String.h"
#include "logger.h"
#include "array.h"
#include "constants.h"

struct Position
{
	union
	{
		// Do we need bigger size ??
		// 
		struct {u32 line, index;};
		struct {u32 x, y;};
	};
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
	LexerState(const String& filepath);
	~LexerState();

	String input;				// Data
	Logger logger = Logger("Lexer"_s);
	u64 input_cursor;			// to keep track were we are
	//StringView cursor;			// new cursor type 

	u32 current_line_number;
	u32 current_char_index; 
	
	Token eat_token();
	Token peek_token(int lookAhead = 0);	

	inline u8& peek_next_character();
	inline u8& peek_character(u64 lookAhead = 0);
	
	inline u8  eat_character();
	inline u8 eat_until_character();
	inline void eat_characters(u64 count = 1);
	

	Position get_current_position() { return {current_line_number, current_char_index}; }
	
	LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor
	
};


