/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

#define Note(...) 
enum Note(GenerateStrings, token_type)
{
	TOKEN_NONE = 0,
	TOKEN_SEMI_COLON = ';',
	TOKEN_COMMA = ',',
	TOKEN_EQUAL = '=',
	
	TOKEN_IDENT = 256,
	TOKEN_KEYWORD,
	TOKEN_HDTYPE,
	TOKEN_LITERAL,
	TOKEN_COMMENT,
	TOKEN_MULTILINE_COMMENT,
	TOKEN_COLON,
	TOKEN_DOUBLE_COLON,
	TOKEN_COLON_EQUAL,
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
	
	//
	TOKEN_KIND_IDENT_HDTYPE,
	TOKEN_KIND_IDENT_KEYWORD,
};

// This will enforce us to only handle files that are 4GB or less, which is a reasonable 
struct Position
{
	U32 line, index;
};
#include <meow_hash\meow_hash.h> 
struct Token
{
	// TODO : Add string_in_source
	U16          type;
	U16          kind;
	Position 	start_position;
	//Position 	end_position;
	meow_u128    hash;
	
	union
	{
		StringView   name;
		bool     boolean;
		U64      integer;
		F32      _float32;
		F64 	 _float64;
		StringView   value;
	};
};

struct LexerStateConfigs {
	bool ignore_comments;
};

padding_check
struct LexerState
{
	StringView input;
	U64 input_cursor;
	U32 current_line_number;
	U32 current_char_index; 
	LexerStateConfigs config;
};

bool isKeyword(StringView& string);

