/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

#define Note(...) 
Note(GenerateStrings, token_type)
enum HDTokenType : U16 {
	TOKEN_NONE = 0,
	TOKEN_SEMI_COLON = ';',
	TOKEN_COMMA = ',',
	TOKEN_EQUAL = '=',
	
	TOKEN_IDENT = 256,
	TOKEN_KEYWORD,
	TOKEN_HDTYPE,
	TOKEN_LITERAL,
	TOKEN_BOOLEAN,
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


// TODO: Add kinds for keywords, that way we don't have to compare strings
enum TokenKind : U16 {
	TOKEN_KIND_NONE,
	
	TOKEN_KIND_ELSE,
	TOKEN_KIND_IF,
	
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
	HDTokenType   type;
	TokenKind   kind;
	// this is just so we don't recalculate the line&index into the file, but do i actually need it ?? 
	Position 	start_position;
	StringView   name;
	meow_u128    hash;
	
};

struct LexerStateConfigs {
	bool ignore_comments;
};

padding_check
struct LexerState
{
	StringView filename;
	StringView input;
	U64 input_cursor;
	U32 current_line_number;
	U32 current_char_index; 
	LexerStateConfigs config;
};

bool isKeyword(StringView& string);
//bool operator==(HDTokenType left, HDTokenType right);
//bool operator!=(HDTokenType left, HDTokenType right);

