/* ========================================================================
   $File: lex.h
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Here are the data structures represents the lexer
   ========================================================================*/

#pragma once

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
	TOKEN_NOTE,
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

	TOKEN_KEYWORDS_START,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_NEW,
	TOKEN_KEYWORD_DELETE,
	TOKEN_KEYWORD_DEFER,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_SWITCH,
	TOKEN_KEYWORD_CONTINUE,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_CAST,
	TOKEN_KEYWORD_IT,
	TOKEN_KEYWORD_TYPE_INFO,
	TOKEN_KEYWORD_SIZE_OF,
	TOKEN_KEYWORD_INLINE,
	TOKEN_KEYWORD_INTERNAL,
	TOKEN_KEYWORD_EXTERNAL,
	TOKEN_KEYWORD_SCOPE,
	TOKEN_KEYWORD_NULL,
	TOKEN_KEYWORD_TRUE,
	TOKEN_KEYWORD_FALSE,
	TOKEN_KEYWORD_PROC,
	TOKEN_KEYWORD_ENUM,
	TOKEN_KEYWORD_FLAG,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_TEMPLATE,
	TOKEN_KEYWORD_IMPORT,
	TOKEN_KEYWORD_LOAD,
	TOKEN_KEYWORD_AS,
	TOKEN_KEYWORD_RUN,
	TOKEN_KEYWORD_COOK,
	TOKEN_KEYWORD_NO_INLINE,	
	TOKEN_KEYWORDS_END,
	
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


	TOKEN_KIND_BOOL_TRUE,
	TOKEN_KIND_BOOL_FALSE,

	
};

// This will enforce us to only handle files that are 4GB or less, which is a reasonable 
struct Position
{
	U32 line, index;
};
//#include <meow_hash\meow_hash.h> 
struct Token
{
	// TODO : Add string_in_source
	String        name;
	Position      start_position;
	TokenKind     kind;
	HDTokenType   type;
	char          unused[4];
};

struct LexerState
{
	String filename;
	String input;
	U64 input_cursor;
	U32 current_line_number;
	U32 current_char_index;

};

HDTokenType isKeyword(StringView& string);

