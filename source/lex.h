/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

#include "String.h"
#include "logger.h"
#include "array.h"
#include "constants.h"
#include "queue.h"

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
	u16 Type = TOKEN_NONE;
	u16 id = 0;
	Position 	start_position;
	Position 	end_position;
	
	union
	{
		String   name;
		bool     boolean;
		u64      integer;
		f32      _float32;
		f64 	 _float64;
		String   value;
	};
};

struct LexerState
{
	LexerState(const String& filepath);
	~LexerState();
	
	String input;				// Data
	Logger logger = Logger("Lexer"_s);
	u64 input_cursor;			// to keep track were we are
	
	u32 current_line_number;
	u32 current_char_index; 
	
	Queue<Token> token_cache;
	
	Token eat_token();
	Token peek_token(u64 lookAhead = 0);
	Token peek_next_token();
	
	inline u8& peek_next_character();
	inline u8& peek_character(u64 lookAhead = 0);
	
	inline u8  eat_character();
	inline u8 eat_until_character();
	inline void eat_characters(u64 count = 1);
	inline void eat_until_whitespace();
	
	Position get_current_position() { return {current_line_number, current_char_index}; }
	
	LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor
	
};


