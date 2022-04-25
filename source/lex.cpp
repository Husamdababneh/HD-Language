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


// TODO : Remove them from reserved[]  
//constexpr
String predefined_types [] = {
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

constexpr
static String keywords[] = {
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
	"proc"_s,
	"enum"_s,
	"flag"_s,
	"struct"_s,
	"template"_s,

	// Libs
	"import"_s,
	"load"_s,
	"as"_s,

	// Directives
	"run"_s,
	"cook"_s,

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

struct KeywordsMap
{
	S64 hash[KEYWORDS_COUNT];
};


constexpr
auto computeKeywordsHashMap()
{
	KeywordsMap map{}; //[KEYWORDS_COUNT] = {};
	for(U64 i = 0; i < KEYWORDS_COUNT; i++)
		map.hash[i] = -1;
	
	for(U64 i = 0; i < KEYWORDS_COUNT; i++)
	{
		auto index = djb2(keywords[i]) % KEYWORDS_COUNT;
		while(map.hash[index] != -1)
		{
			index = (index + 1) % KEYWORDS_COUNT;
		}
		map.hash[index] = i; 
	}
	return map;
}

constexpr static KeywordsMap keywordsHashes = computeKeywordsHashMap();

static inline 
HDTokenType isKeyword(String& string)
{
	// TODO: Generate Hashmap for the keywords
	auto hasher = [](String str) { return keywordsHashes.hash[djb2(str) % KEYWORDS_COUNT]; } ;
	auto hashOfStr = djb2(string) % KEYWORDS_COUNT;
	auto index = keywordsHashes.hash[hashOfStr];
	auto it = index;
	do {
		auto word = keywords[it];
	 	it = (it + 1) % KEYWORDS_COUNT;
		if (EqualStrings(string, word)) return static_cast<HDTokenType>(it + HDTokenType::TOKEN_KEYWORDS_START);
		if (index == it) return TOKEN_NONE;
	} while(index != it);
		
	return TOKEN_NONE;
}

static inline 
S8 peek_next_character(LexerState& lex)
{
	// bounds check
	return peek_character(lex);
}

static inline 
S8 peek_character(LexerState& lex, U64 lookAhead /* = 0 */)
{
	if (lex.input_cursor + lookAhead >= lex.input.size) return -1;
	return lex.input[lex.input_cursor + lookAhead];
}

static inline 
void eat_characters(LexerState& lex, U64 count)
{
	for(int a = 0; a < count; a++)
		if (eat_character(lex) == -1) break;
}

static inline 
S8 eat_until_character(LexerState& lex)
{
	while(true)
	{
		// eat_character is EOF safe
		auto ch = eat_character(lex);
		if (isWhiteSpace(ch)) continue;
		return ch;
	}
}

static inline 
void eat_until_whitespace(LexerState& lex)
{
	while(!isWhiteSpace(peek_character(lex))) eat_character(lex);
}

static inline 
void eat_ident(LexerState& lex)
{
	while(isLiteralChar(peek_character(lex)))
		eat_character(lex);
}

static inline
S8 eat_character(LexerState& lex)
{
	// TODO: Check for end of file
	if (lex.input_cursor >= lex.input.size) return -1;
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
	if (lex.input_cursor >= lex.input.length) return -1;
	
	return lex.input[lex.input_cursor - 1];
}

static inline
void handleComments(LexerState& lex, Token& token)
{
	auto ch = eat_character(lex);
	if (ch == '/')
	{
		token.type = HDTokenType::TOKEN_COMMENT;
		while(peek_character(lex) != '\r' && peek_character(lex) != '\n')
			eat_character(lex);
		return;
	}
	
	assert(ch == '*');
	token.type = HDTokenType::TOKEN_MULTILINE_COMMENT;
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
}

static 
Token process_token(LexerState& lex)
{
	//static int COUTNER = 1;
	//COUTNER++;
	Token token = {};
	S8 ch = eat_until_character(lex);
	if(ch == -1) 
	{
		token.type = HDTokenType::TOKEN_EOFA;
		return token;
	}
	
	//HDTokenType type  = (HDTokenType)ch;
	U64 temp = lex.input_cursor - 1;
	token.start_position = get_current_position(lex);

	auto next = peek_character(lex);
	switch(ch)
	{
	  case '/':
	  {
		  if (next == '/' || next == '*') handleComments(lex, token);
		  else                            token.type = (HDTokenType)ch;
		  return process_token(lex);
	  }
	  break;
	  case '+': case '*': case ';': 
	  case '`': case '|': case '$':
	  case ',': case '%': case '^':
	  case '~': case '!': case '{': 
	  case '&': case '?': case '[': 
	  case '(': case ']': case '}':
	  case ')': case '\\':
	  {
		  token.type = (HDTokenType)ch;
		  break;
	  }
	  case '-':
	  {
		  if (next != '>') token.type = (HDTokenType)ch;
		  else
		  {
			  token.type = HDTokenType::TOKEN_ARROW;
			  eat_character(lex);
		  } 
		  break;	
	  }
	  // Notes 
	  case '@': 
	  {
		  //TODO : Notes
		  token.type = HDTokenType::TOKEN_NOTE;
		  eat_until_whitespace(lex);
		  break;
	  }
	  // Compiler Directives
	  case '#':
	  {
		  token.type = HDTokenType::TOKEN_DIRECTIVE;
		  eat_until_whitespace(lex);
		  break;
	  }
	  case '=':
	  {
		  if (next == '='){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_EQL;
		  }
		  else {
			  token.type = (HDTokenType)ch;
		  }
		  break;
	  }
	  case '<':
	  {
		  if (next == '='){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_LT_OR_EQL;
		  } else if (next == '<'){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_SHIFT_LEFT;
		  } else {
			  token.type = HDTokenType::TOKEN_LT;
		  }
		  break;
	  }
	  case '>':
	  {
		  if (next == '='){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_GT_OR_EQL;
		  } else if (next == '>'){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_SHIFT_RIGHT;
		  } else {
			  token.type = HDTokenType::TOKEN_GT;
		  }
		  break;
	  }
	  case '.':
	  {
		  if (next == '.') {
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_DOUBLEDOT;
		  }
		  else {
			  token.type = (HDTokenType)ch;
		  }
		  break;
	  }
	  case '\'':
	  {
		  printf("['] is unused lexeme\n");
		  exit(-1);
		  break;
	  }
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
		  token.type = HDTokenType::TOKEN_LITERAL;
		  token.kind = TOKEN_KIND_STRING_LITERAL;
		  break;	  
	  }
	  case ':':
	  {
		  if (next == ':')
		  {
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_DOUBLE_COLON;
		  }
		  else if (next == '='){
			  eat_character(lex);
			  token.type = HDTokenType::TOKEN_COLON_EQUAL;
		  }
		  else {
			  token.type = HDTokenType::TOKEN_COLON;
		  }
		  break;
	  }
	  case '\0':
	  {
		  token.type = HDTokenType::TOKEN_EOFA;
		  return token;
	  }
	  default:
	  {
		  if (isAlphabet(ch) || ch == '_'){
			  eat_ident(lex);
			  token.name = CStringToString((lex.input.str_char+ temp), lex.input_cursor  - temp);
			  token.type = HDTokenType::TOKEN_IDENT;

			  auto keyWordTokenType = isKeyword(token.name);
			  if (keyWordTokenType != TOKEN_NONE)
				  token.type = keyWordTokenType;
		  
		  }
		  else if (isDigit(ch))
		  {
			  // TODO: Handle floating points ... 
			  // TODO: handle the real values instead of the string
			  // We assume that this will be only numbers
			  // (0x -> hex) (0b -> binary) (0c  -> Octal)
			  // auto& pos = get_current_position();
			  token.type = HDTokenType::TOKEN_LITERAL;
			  token.kind = TOKEN_KIND_INT_LITERAL;
			  //auto peeked = peek_character(lex);
			  // if(peeked == 'x' ||  peeked == 'b' || peeked == 'c')  eat_character(lex);
			  //tokenize_number(lex)
			  while (isDigit(peek_character(lex))) eat_character(lex);
				
		  }
		  else {
			  assert(false && "What happend here?");
		  }
			
		  break;
	  }
	}
	
	assert(token.type != HDTokenType::TOKEN_NONE);
	//token.name = {lex.input_cursor  - temp, (S8*)lex.input.str_char + temp};
	
	if (token.type == HDTokenType::TOKEN_COMMENT || 
		token.type == HDTokenType::TOKEN_MULTILINE_COMMENT
		)
	{
		return process_token(lex);
	}
	
	return token;
}


B8 process_file(LexerState* lex)
{
	return false;
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
	while((U16)tok.type != (U16)token_type) tok = eat_token(lex);
	return tok;
}
