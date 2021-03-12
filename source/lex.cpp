/* ========================================================================
   $File: lex.cpp
   $Date: 2020-04-04
   $Creator: Husam Dababneh
   $Description: Defines Lexer functionality & Data Structures
   ========================================================================*/ 


#include "pch.h"
#include "lex.h"

#include "common.h"
#include "auxiliary.h"
#include "stack.h"


#define FOR_RANGE(x) for(u64 it = 0; it < x; it++)

// TODO : Remove them from reserved[]  
String predefined_types [] = {
	// Types
	"int"_s, "float"_s,
	"u8"_s , "s8"_s,
	"u16"_s, "s16"_s,
	"u32"_s, "s32"_s,
	"u64"_s, "s64"_s,	
	"float32"_s,
	"float64"_s,
	"string"_s,
	"bool"_s,
	"void"_s
};

String reserved [] = {
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
	
	// not used
	"no_inline"_s
};

constexpr int KeywordCount = sizeof(reserved) / sizeof(String);

static inline bool isWhitechar(u8 c){
	if (c == '\t' ||
		c ==  '\n' ||
		c ==  '\r' ||
		c ==  ' ')
		return true;
	return false;
}

static inline bool isAlphabet(u8 ch){
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	return false;
}

static inline bool isDigit(u8 ch){
	if (ch >= '0' && ch <= '9') return true;
	return false;
}
static inline bool isInLireralChar(u8 ch){
	if (isAlphabet(ch)) return true;
	if (ch == '_')  return true;
	if (isDigit(ch)) return true;
	return false;
}



bool isKeyword(String& string)
{
	for(int a = 0; a < KeywordCount; a++)
		if(isEqual(&string, &reserved[a]))
		return true;
	return false;
}

bool isHDType(String& string)
{
	// Make this a hashmap lookup, faster...
	for(int a = 0; a < KeywordCount; a++) if(isEqual(&string, &predefined_types[a])) return true;
	return false;
}
u8& LexerState::peek_next_character()
{
	return input[input_cursor + 1];
}

u8& LexerState::peek_character(u64 lookAhead /* = 0 */)
{
	return input[input_cursor + lookAhead];
}

void LexerState::eat_characters(u64 count)
{
	for(int a = 0; a < count; a++)
		eat_character();
}

u8 LexerState::eat_until_character()
{
	for(u64 a = input_cursor;  a < input.count; a++){
		auto ch = eat_character();
		if (!isWhitechar(ch)) {
			return ch;
		}
	}
	return 0;
}

void LexerState::eat_until_whitespace()
{
	
	for(u64 a = input_cursor;  a < input.count; a++)
	{
		if (isWhitechar(peek_character())) break; 
		eat_character();
	}
	
}


u8 LexerState::eat_character()
{
	
	switch(input[input_cursor])
	{
		case '\n':
		current_line_number++;
		current_char_index = 0;
		break;
		case '\t':
		//current_char_index+=4;
		//break;
		default:
		current_char_index++;
		break;
	}
	input_cursor++;
	return input[input_cursor - 1];
}

LexerState::LexerState(const String& filepath)
{
	printf("initing Lexer\n");
	u64 length = read_entire_file(filepath, (void**)&input.data);
	if (length < 0)
	{
		logger.print("Couldn't read file %s\n"_s, filepath);
		exit(1);
	}
	input.count = length;
	input_cursor = 0;
	
	current_line_number = 1;
	current_char_index = 0;	
	
	
	//// Cache
	token_cache = make_queue<Token>(100);
	// This stack can be very small since i don't think one will write so deep scopes 
	paranthases_stack = make_stack<u8>(26);
	this->preprosses();
}

LexerState::~LexerState()
{
	printf("Getting Out\n");
	free_queue(&token_cache);
	if (input.data){
		delete[] input.data;
	}
}

void LexerState::preprosses() 
{
#if ENABLE_LEX_PREPROSSES == 1
	// TODO: Is there anything usefull we can do here? or it's just a waste of time ?? 
#else
	
#endif
	
}

Token LexerState::process_token()
{
	// @TODO: init a token and return it.
	static int COUTNER = 1;
	if(input_cursor >= input.count)
		return { TOKEN_EOFA , 0, 0 , 0 };
	
	Token token = {};
	
	token.id = COUTNER;
	COUTNER++;
	u8 ch = eat_until_character();
	u64 temp = input_cursor - 1;
	token.start_position = get_current_position();
	switch(ch)
	{
		// @Todo(Husam):Handle nested multiline comments.
		case '/':
		{
			u8 next = peek_character();
			
			if(next  == '/'){
				token.Type = TOKEN_COMMENT;
				while(eat_character() != '\n');
			} else if (next == '*'){
				token.Type = TOKEN_MULTILINE_COMMENT;
				int nested_level = 1;			  
				while(input_cursor < input.count ){
					u8 eaten = eat_character();
					u8 next_to_be_eaten = peek_character();
					if (eaten == '/' && next_to_be_eaten == '*'){
						eat_character();
						nested_level++;
					}
					if(eaten == '*' && next_to_be_eaten == '/'){
						eat_character();
						nested_level--;
						continue;
					}
					if (nested_level == 0) break;	  
				};
				break;	  
			} else {
				// division ?? 
				token.Type = ch;
				break;	  
			}
			break;
		}
		break;
		case '+': case '*': 
		case ';': case '`':
		case ',': 
		case '~': case '!': case '$': case '%': case '^':
		case '&': case '?': case '|': case '\'': case '\\':
		{
			token.Type = ch;
			break;
		}
		// TODO: Put Matching Paranthases Here 
		case '[': 
		case '{': 
		case '(': 
		{
			paranthases_stack.push(ch);
			token.Type = ch;
			break;
		}
		case ']': 
		{
			if (paranthases_stack.top() == '[') paranthases_stack.pop();
			else assert(false);
			token.Type = ch;
			break;
		}
		case '}':
		{
			if (paranthases_stack.top() == '{') paranthases_stack.pop();
			else assert(false);
			token.Type = ch;
			break;
		}
		case ')': 
		{
			if (paranthases_stack.top() == '(') paranthases_stack.pop();
			else assert(false);
			token.Type = ch;
			break;
		}
		case '-':
		{
			auto next = peek_character();
			if (next == '>'){ 
				token.Type = TOKEN_ARROW;
				eat_character();
			}else {
				token.Type = ch;
			}
			break;
			
		}
		// Notes 
		case '@': 
		{
			//TODO : Notes
			token.Type = TOKEN_DIRECTIVE;
			eat_until_whitespace();
			break;
		}
		// Compiler Directives
		case '#':
		{
			token.Type = TOKEN_DIRECTIVE;
			eat_until_whitespace();
			break;
		}
		case '=':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.Type = TOKEN_EQL;
			}
			else {
				token.Type = TOKEN_ASSIGN;
			}
			break;
		}
		case '<':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.Type = TOKEN_LT_OR_EQL;
			} else if (next == '<'){
				eat_character();
				token.Type = TOKEN_SHIFT_LEFT;
			} else {
				token.Type = TOKEN_LT;
			}
			break;
		}
		case '>':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.Type = TOKEN_GT_OR_EQL;
			} else if (next == '>'){
				eat_character();
				token.Type = TOKEN_SHIFT_RIGHT;
			} else {
				token.Type = TOKEN_GT;
			}
			break;
		}
		case '.':
		{
			auto next = peek_character();
			if (next == '.') {
				eat_character();
				token.Type = TOKEN_DOUBLEDOT;
			}
			else {
				token.Type = ch;
			}
			break;
		}
		case '"':
		{
			// @Cleanup: We could check the prev instead ?? wont it be easer ??
			// @TODO: Make sure this works ?? 
			// u8 escape = 0;
			while(true){
				auto peeked = peek_character();
				auto ahead =  peek_character(1);
				if (peeked == '\\' && ahead == '"'){
					eat_characters(2);
				}
				eat_character();
				if(peeked == '"')
					break;
			}	  
			token.Type = TOKEN_LITERAL;
			break;	  
		}
		case ':':
		{
			// @CleanUp: Make this use it's ASCII Value instead.. i think ??
			token.Type = TOKEN_COLON;
			break;
		}
		break;
		case '\0':
		{
			return { TOKEN_EOFA , 0 , 0 };
			break;
		}
		default:
		if (isAlphabet(ch) || ch == '_'){
			while(true){
				auto next = peek_character();
				if (!isInLireralChar(next))
					break;
				eat_character();
			};
			token.value = String { &input[temp], input_cursor  - temp};
			if (isKeyword(token.value))
				token.Type = TOKEN_KEYWORD;
			else if (isHDType(token.value))
				token.Type = TOKEN_HDTYPE;
			else
				token.Type = TOKEN_IDENT;
			
			
		} else {
			// We assume that this will be only numbers
			// (0x -> hex) (0b -> binary) (0o  -> Octal)
			// auto& pos = get_current_position();
			assert(isDigit(ch));
			token.Type = TOKEN_LITERAL;
			auto peeked = peek_character();
			bool doEat  = true;
			if(peeked == 'x' ||  peeked == 'b' ||	 peeked == 'o')  eat_character();
			
			peeked = peek_character();
			
			if (isDigit(peeked)){			  
				while(true){
					auto next = peek_character();
					if (!isDigit(next)){
						break;
					}
					eat_character();
				}
			}
		}
		break;
	}
	token.end_position = get_current_position();
	assert(token.Type != TOKEN_NONE);
	token.value = String { &input[temp], input_cursor  - temp};
	
	//nocheckin 
	if (token.Type == TOKEN_COMMENT ||
		token.Type == TOKEN_MULTILINE_COMMENT)
		return process_token();
	else
		return token;
}

Token LexerState::eat_token()
{
	if (token_cache.count == 0)
		return process_token();
	return *pop(&token_cache);
}


Token LexerState::peek_token(u64 lookAhead /* = 0*/ )
{
	if (token_cache.count == 0) {
		if (lookAhead == 0) return *push(&token_cache, process_token());
		
	}
	
	if (token_cache.count <= lookAhead)
	{
		u64 range = lookAhead - token_cache.count + 1;
		FOR_RANGE(range){
			push(&token_cache, process_token());
		}
	}
	
	return *top_plus(&token_cache, lookAhead);
}

#undef FOR_RANGE