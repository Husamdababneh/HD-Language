/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

enum Note(GenerateStrings, token_type)
{
	TOKEN_SEMI_COLON = ';',
	TOKEN_COMMA = ',',
	TOKEN_EQUAL = '=',
	
	TOKEN_IDENT = 256,
	TOKEN_HDTYPE,
	TOKEN_KEYWORD,
	TOKEN_LITERAL,
	TOKEN_COMMENT,
	TOKEN_MULTILINE_COMMENT,
	TOKEN_COLON,
	TOKEN_DOUBLE_COLON,
	TOKEN_COLON_EQUAL,// Not Used
	TOKEN_OPERATOR,
	TOKEN_DIRECTIVE,
	TOKEN_DOUBLEDOT,
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
	TOKEN_KIND_STRING_LITERAL,
	TOKEN_KIND_INT_LITERAL,
	TOKEN_KIND_FLOAT_LITERAL,
};


struct Position
{
	u32 line, index;
};

struct Token
{
	// TODO : Add string_in_source
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

struct LexerStateConfigs {
	bool ignore_comments = false;
};

struct LexerState
{
	LexerState(const StringView& str, bool isfile = true);
	~LexerState();
	
	void* data; // hack for string;
	StringView input = {0};				// Data
	Logger logger = Logger("Lexer"_s);
	u64 input_cursor;			// to keep track were we are
	
	u32 current_line_number;
	u32 current_char_index; 
	
	
	Arena strings = {0};
	Queue<Token> token_cache;
	Stack<u8> paranthases_stack;
	LexerStateConfigs config;
	Token eat_token();
	Token process_token();
	Token eat_token_cache();
	Token peek_token(u64 lookAhead = 0);
	Token peek_forward(u64 lookahead);
	Token eat_until_token_by_type(u64 token_type);
	
	Position get_current_position() { return {current_line_number, current_char_index}; }
	
	//private:
	inline u8& peek_next_character();
	inline u8& peek_character(u64 lookAhead = 0);
	
	inline u8 eat_until(u8 charr);
	inline u8  eat_character();
	inline u8 eat_until_character();
	inline void eat_characters(u64 count = 1);
	inline void eat_until_whitespace();
	
	void preprosses();
	
	
	//LexerState() = delete;					 // copy constructor
	LexerState(const LexerState& ) = delete; // copy constructor
	
};

bool isKeyword(StringView& string);

