/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

enum {
	TOKEN_IDENT = 256,
	TOKEN_HDTYPE,
	TOKEN_KEYWORD,
	TOKEN_LITERAL,
	TOKEN_COMMENT,
	TOKEN_MULTILINE_COMMENT,
	TOKEN_COLON,
	TOKEN_OPERATOR,
	TOKEN_DIRECTIVE,
	TOKEN_DOUBLEDOT,
	TOKEN_STRING_LITERAL,
	TOKEN_INT_LITERAL,
	TOKEN_FLOAT_LITERAL,
	TOKEN_ARROW,
	TOKEN_ASSIGN,
	TOKEN_EQL,
	TOKEN_GT,
	TOKEN_LT,
	TOKEN_GT_OR_EQL,
	TOKEN_LT_OR_EQL,
	TOKEN_SHIFT_LEFT,
	TOKEN_SHIFT_RIGHT,
	TOKEN_NONE,
	TOKEN_EOFA,
	TOKEN_ERROR,
};


enum {
	// Token Kinds 
	TOKEN_KIND_BINARY,
	TOKEN_KIND_UNARY,
	TOKEN_KIND_DOT,
	
	//TOKEN_KIND_
};

struct Position
{
	union
	{
		// Do we need bigger size ??
		struct {u32 line, index;};
		struct {u32 x, y;};
	};
};

struct Token
{
	u16          type = TOKEN_NONE;
	u16          kind = 0;
	Position 	start_position = {0};
	Position 	end_position = {0};
	meow_u128    hash;
	
	union
	{
		StringView   name;
		bool     boolean;
		u64      integer;
		f32      _float32;
		f64 	 _float64;
		StringView   value;
	};
};

struct LexerState
{
	LexerState(const StringView& str, bool isfile = true);
	~LexerState();
	
	void* data; // hack for string;
	StringView input;				// Data
	Logger logger = Logger("Lexer"_s);
	u64 input_cursor;			// to keep track were we are
	
	u32 current_line_number;
	u32 current_char_index; 
	
	Queue<Token> token_cache;
	Stack<u8> paranthases_stack;
	
	Token eat_token();
	Token process_token();
	Token eat_token_cache();
	Token peek_token(u64 lookAhead = 0);
	Token peek_next_token();
	Token peek_forward(u64 lookahead);
	
	inline u8& peek_next_character();
	inline u8& peek_character(u64 lookAhead = 0);
	
	inline u8  eat_character();
	inline u8 eat_until_character();
	inline void eat_characters(u64 count = 1);
	inline void eat_until_whitespace();
	
	void preprosses();
	
	Position get_current_position() { return {current_line_number, current_char_index}; }
	
	//LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor
	
};


