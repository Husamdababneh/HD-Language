/* ========================================================================
$File: cpp
$Date: 2020-04-04
$Creator: Husam Dababneh
$Description: Defines Lexer functionality & Data Structures
========================================================================*/ 

#include "lex.h"

#ifndef LEXER_ENABLE_C_CHAR_TOKEN
#define LEXER_ENABLE_C_CHAR_TOKEN 0
#endif

#include "common.h"
#include "auxiliary.h"
#include "stack.h"


// NOTE(Husam Dababneh): Forward Declaration of basic procedures
Token eat_token(LexerState&);
Token process_token(LexerState&);
Token peek_token(LexerState&, U64 lookAhead = 0);
Token peek_forward(LexerState&, U64 lookahead);
Token eat_until_token_by_type(LexerState&, U64 token_type);

S8  eat_character(LexerState&);
S8  peek_character(LexerState&, U64 count = 0);
void eat_characters(LexerState&, U64 count = 0);

[[nodiscard]] 
Position 
get_current_position(LexerState& lex) 
{ 
	return {lex.current_line_number + 1, lex.current_char_index}; 
}
/*
TODO(Husam Dababneh): I want to change the way the lexer works, but just after finishing the base
*/

#define FOR_RANGE(x) for(U64 it = 0; it < x; it++)

// TODO : Remove them from reserved[]  
StringView predefined_types [] = {
	
	// TODO(Husam Dababneh): Do I need Uppercase version of these ?? 
	// types
	"U8"_s , "S8"_s,
	"u16"_s, "s16"_s,
	"U32"_s, "s32"_s,
	"U64"_s, "S64"_s,	
	"float32"_s,
	"float64"_s,
	"string"_s,
	"bool"_s,
	"void"_s
};

StringView keywords[] = {
	// loop
	"for"_s,
	
	// memory related
	"new"_s,
	"delete"_s,
	
	// control 
	"defer"_s,
	"if"_s,
	"else"_s,
	"switch"_s,
	"continue"_s,
	"break"_s,
	"return"_s,
	
	// Builtins
	"cast"_s,
	"it"_s,
	"type_info"_s,
	"size_of"_s,
	"inline"_s,
	"internal"_s,
	"external"_s,
	"scope"_s,
	
	// Constants
	"null"_s,
	"true"_s,
	"false"_s,
	
	// Compiler Stuff
	"as"_s,
	
	// not used
	"no_inline"_s,
};

constexpr int PREDEFINED_TYPES_COUNT = sizeof(predefined_types) / sizeof(predefined_types[0]);
constexpr int KEYWORDS_COUNT = sizeof(keywords) / sizeof(keywords[0]);

static inline bool isWhiteSpace(U8 c){
	return (c ==  '\t' || 
			c ==  '\n' ||
			c ==  '\r' || 
			c ==  ' '  || 
			c ==  '\f' ||
			c ==  '\v' );
}

static inline bool isAlphabet(U8 ch){
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	return false;
}

static inline bool isDigit(U8 ch){
	if (ch >= '0' && ch <= '9') return true;
	return false;
}

static inline bool isLiteralChar(U8 ch){
	if (isAlphabet(ch)) return true;
	if (ch == '_')  return true;
	if (isDigit(ch)) return true;
	return false;
}

bool isKeyword(StringView& string)
{
	// TODO: Generate Hashmap for the keywords
	for(int a = 0; a < KEYWORDS_COUNT; a++){
		if (EqualStrings(string, keywords[a])) return true;
	}
	return false;
}

bool isHDtype(StringView& string)
{
	// TODO: Generate Hashmap for the keywords
	for(int a = 0; a < PREDEFINED_TYPES_COUNT; a++){
		if(EqualStrings(string, predefined_types[a])) {
			return true;
		}
	}
	return false;
}

S8 peek_next_character(LexerState& lex)
{
	return lex.input[lex.input_cursor + 1];
}

S8 peek_character(LexerState& lex, U64 lookAhead /* = 0 */)
{
	return lex.input[lex.input_cursor + lookAhead];
}

// TODO(Husam Dababneh): Merge this with the basic one
void eat_characters(LexerState& lex, U64 count)
{
	for(int a = 0; a < count; a++)
		eat_character(lex);
}

S8 eat_until_character(LexerState& lex)
{
	
	while(isWhiteSpace(peek_character(lex))) eat_character(lex);
	return eat_character(lex);
}

S8 eat_until(LexerState& lex, S8 charr)
{
	
	while(peek_character(lex) != charr) eat_character(lex);
	return eat_character(lex);
}


inline 
void eat_until_whitespace(LexerState& lex)
{
	while(!isWhiteSpace(peek_character(lex))) eat_character(lex);
}

inline 
void eat_ident(LexerState& lex)
{
	while(isLiteralChar(peek_character(lex)))
		eat_character(lex);
}


S8 eat_character(LexerState& lex)
{
	// TODO: Check for end of file
	switch(lex.input[lex.input_cursor])
	{
		case '\n':
		lex.current_line_number++;
		lex.current_char_index = 0;
		break;
		default:
		lex.current_char_index++;
		break;
	}
	lex.input_cursor++;
	if (lex.input_cursor >= lex.input.length) return 0;
	
	return lex.input[lex.input_cursor - 1];
}

#define Hash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
Token process_token(LexerState& lex)
{
	// @TODO: init a token and return it.
	//static int COUTNER = 1;
	if(lex.input_cursor >= lex.input.length)
		return { TOKEN_EOFA , 0, 0 , 0 };
	
	Token token = {0};
	//COUTNER++;
	
	U8 ch = eat_until_character(lex);
	U64 temp = lex.input_cursor - 1;
	token.start_position = get_current_position(lex);
	switch(ch)
	{
		case '/':
		{
			U8 next = peek_character(lex);
			if(next  == '/'){
				token.type = TOKEN_COMMENT;
				while(peek_character(lex) != '\r' && 
					  peek_character(lex) != '\n'){eat_character(lex);}
			}
			else if (next == '*'){
				// TODO: 
				token.type = TOKEN_MULTILINE_COMMENT;
				int nested_level = 1;
				while(lex.input_cursor < lex.input.length){
					U8 eaten = eat_character(lex);
					U8 next_to_be_eaten = peek_character(lex);
					if (eaten == '/' && next_to_be_eaten == '*'){
						eat_character(lex);
						nested_level++;
					}
					if(eaten == '*' && next_to_be_eaten == '/'){
						eat_character(lex);
						nested_level--;
						continue;
					}
					if (nested_level == 0) break;	  
				};
				if (nested_level != 0) {
					// TODO : Report location where it started and 
					printf("Uncontinued Multiline comment\n");
					exit(-1);
				}
				break;
			} 
			else {
				// division ?? 
				token.type = ch;
				break;	  
			}
			break;
		}
		break;
		case '+': case '*': case ';': 
		case '`': case '|': case '$':
		case ',': case '%': case '^':
		case '~': case '!': case '\\':
		case '&': case '?': 
		{
			token.type = ch;
			break;
		}
		
#if LEXER_ENABLE_C_CHAR_TOKEN == 0
		case '\'':
		{
			token.type = ch;
			break;
		}
#endif
		case '[': 
		case '{': 
		case '(': 
		{
			
			token.type = ch;
			break;
		}
		case ']': 
		{
			token.type = ch;
			break;
		}
		case '}':
		{
			token.type = ch;
			break;
		}
		case ')': 
		{
			token.type = ch;
			break;
		}
		case '-':
		{
			auto next = peek_character(lex);
			if (next == '>'){ 
				token.type = TOKEN_ARROW;
				eat_character(lex);
			} else {
				token.type = ch;
			}
			break;
			
		}
		// Notes 
		case '@': 
		{
			//TODO : Notes
			token.type = TOKEN_DIRECTIVE;
			eat_until_whitespace(lex);
			break;
		}
		// Compiler Directives
		case '#':
		{
			token.type = TOKEN_DIRECTIVE;
			eat_until_whitespace(lex);
			break;
		}
		case '=':
		{
			auto next = peek_character(lex);
			if (next == '='){
				eat_character(lex);
				token.type = TOKEN_EQL;
			}
			else {
				token.type = ch;
			}
			break;
		}
		case '<':
		{
			auto next = peek_character(lex);
			if (next == '='){
				eat_character(lex);
				token.type = TOKEN_LT_OR_EQL;
			} else if (next == '<'){
				eat_character(lex);
				token.type = TOKEN_SHIFT_LEFT;
			} else {
				token.type = TOKEN_LT;
			}
			break;
		}
		case '>':
		{
			auto next = peek_character(lex);
			if (next == '='){
				eat_character(lex);
				token.type = TOKEN_GT_OR_EQL;
			} else if (next == '>'){
				eat_character(lex);
				token.type = TOKEN_SHIFT_RIGHT;
			} else {
				token.type = TOKEN_GT;
			}
			break;
		}
		case '.':
		{
			auto next = peek_character(lex);
			if (next == '.') {
				eat_character(lex);
				token.type = TOKEN_DOUBLEDOT;
			}
			else {
				token.type = ch;
			}
			break;
		}
#if LEXER_ENABLE_C_CHAR_TOKEN == 1
		case '\'':
#endif
		case '"':
		{
			// @Cleanup: We could check the prev instead ?? wont it be easer ??
			// @TODO: Make sure this works ?? 
			// U8 escape = 0;
			const U8& cu = ch;
			while(true) {
				auto peeked = peek_character(lex);
				auto ahead =  peek_character(lex, 1);
				if (peeked == '\\' && ahead == cu){
					eat_characters(lex, 2);
				}
				eat_character(lex);
				if(peeked == cu) break;
			}	  
			token.type = TOKEN_LITERAL;
			token.kind = TOKEN_KIND_STRING_LITERAL;
			break;	  
		}
		case ':':
		{
			if (peek_character(lex) == ':')
			{
				eat_character(lex);
				token.type = TOKEN_DOUBLE_COLON;
			}
			else if (peek_character(lex) == '='){
				token.type = TOKEN_COLON_EQUAL;
				eat_character(lex);
			}
			else {
				token.type = TOKEN_COLON;
			}
			break;
		}
		break;
		case '\0':
		{
			return { TOKEN_EOFA , 0 , 0 , 0};
			break;
		}
		default:
		{
			if (isAlphabet(ch) || ch == '_'){
				//eat_until_whitespace();
				eat_ident(lex);
				token.value = CStringToString((lex.input.str_char+ temp), lex.input_cursor  - temp);
				token.type = TOKEN_IDENT;
				if (isKeyword(token.value))
					token.type = TOKEN_KEYWORD;
			}
			else if (isDigit(ch))
			{
				// TODO: Handle floating points ... 
				// TODO: handle the real values instead of the string
				// We assume that this will be only numbers
				// (0x -> hex) (0b -> binary) (0c  -> Octal)
				// auto& pos = get_current_position();
				token.type = TOKEN_LITERAL;
				token.kind = TOKEN_KIND_INT_LITERAL;
				auto peeked = peek_character(lex);
				if(peeked == 'x' ||  peeked == 'b' || peeked == 'c')  eat_character(lex);
				while (isDigit(peek_character(lex))) eat_character(lex);
				
			} 
			break;
		}
	}
	token.end_position = get_current_position(lex);
	assert(token.type != TOKEN_NONE);
	token.value = CStringToString((lex.input.str_char + temp), lex.input_cursor  - temp);
	token.hash = Hash(token);
	
	// Do we need a mechanesim to bundle the comment with statments ? 
	// or we just ignore them altogather
	if (lex.config.ignore_comments && token.type == TOKEN_COMMENT)
	{
		return process_token(lex);
	}
	
	return token;
}


Token eat_token(LexerState& lex)
{
	return process_token(lex);
}

Token peek_token(LexerState& lex, U64 lookAhead)
{
	LexerState oldLex = lex;
	Token token = {};
	do { token = eat_token(lex);} while(lookAhead--);
	lex = oldLex;
	return token;
}

Token 
eat_until_token_by_type(LexerState& lex, U64 token_type)
{
	Token tok = eat_token(lex);
	while(tok.type != token_type) tok = eat_token(lex);
	return tok;
}

#undef FOR_RANGE