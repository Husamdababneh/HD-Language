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
StringView predefined_types [] = {
	// types
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

StringView reserved [] = {
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
	"no_inline"_s,
};

constexpr int KeywordCount = sizeof(reserved) / sizeof(StringView);

static inline bool isWhiteSpace(u8 c){
	if (c == '\t' ||c ==  '\n' ||c ==  '\r' ||c ==  ' ') return true;
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

static inline bool isLiteralChar(u8 ch){
	if (isAlphabet(ch)) return true;
	if (ch == '_')  return true;
	if (isDigit(ch)) return true;
	return false;
}

bool isKeyword(StringView& string)
{
	// TODO: Generate Hashmap for the keywords
	for(int a = 0; a < KeywordCount; a++){
		if(cmp2sv(string, &reserved[a]) == 0) {
			return true;
		}
	}
	return false;
}

bool isHDtype(StringView& string)
{
	// TODO: Generate Hashmap for the keywords
	for(int a = 0; a < KeywordCount; a++){
		if(cmp2sv(string, &predefined_types[a]) == 0) {
			return true;
		}
	}
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
		if (!isWhiteSpace(ch)) {
			return ch;
		}
	}
	return 0;
}

void LexerState::eat_until_whitespace()
{
	
	for(u64 a = input_cursor; a < input.count; a++)
	{
		if (isWhiteSpace(peek_character())) break; 
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
		default:
		current_char_index++;
		break;
	}
	input_cursor++;
	return input[input_cursor - 1];
}

LexerState::LexerState(const StringView& str, bool isfile /* = true*/)
{
	if (isfile){
		u64 length = read_entire_file(str, (void**)&input.data);
		if (length < 0)
		{
			logger.print("Couldn't read file %s\n"_s, str);
			exit(1);
		}
		input.count = length;
		input_cursor = 0;
		data = input.data;
	}else {
		data = nullptr;
		input.data = str.data;
		input.count = str.count;
		input_cursor = 0;
	}
	
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
	//string_free(input);
	if (data){
		delete[] data;
	}
}

void LexerState::preprosses() 
{
#if ENABLE_LEX_PREPROSSES == 1
	// TODO: Is there anything usefull we can do here? or it's just a waste of time ?? 
#else
	
#endif
	
}
#define Hash(token) MeowHash(MeowDefaultSeed, sizeof(Token),(void*)&token); 
Token LexerState::process_token()
{
	// @TODO: init a token and return it.
	static int COUTNER = 1;
	if(input_cursor >= input.count)
		return { TOKEN_EOFA , 0, 0 , 0 };
	
	Token token = {0};
	
	//token.id = COUTNER;
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
				token.type = TOKEN_COMMENT;
				while(eat_character() != '\n');
			}
			else if (next == '*'){
				// TODO: 
				token.type = TOKEN_MULTILINE_COMMENT;
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
		case '+': case '*': 
		case ';': case '`':
		case ',': 
		case '~': case '!': case '$': case '%': case '^':
		case '&': case '?': case '|': case '\'': case '\\':
		{
			token.type = ch;
			break;
		}
		// TODO: Remove the stack,, a 3Xu16 variables are enough to do the work 
		case '[': 
		case '{': 
		case '(': 
		{
			paranthases_stack.push(ch);
			token.type = ch;
			break;
		}
		case ']': 
		{
			if (paranthases_stack.top() == '[') paranthases_stack.pop();
			else {
				logger.print_with_location(&token, "Unbalanced '['\n"_s);
				exit(-1);
			}
			token.type = ch;
			break;
		}
		case '}':
		{
			if (paranthases_stack.top() == '{') paranthases_stack.pop();
			else {
				logger.print_with_location(&token, "Unbalanced '{'\n"_s);
				exit(-1);
			}
			
			token.type = ch;
			break;
		}
		case ')': 
		{
			if (paranthases_stack.top() == '(') paranthases_stack.pop();
			else {
				logger.print_with_location(&token, "Unbalanced '('\n"_s);
				exit(-1);
			}
			
			token.type = ch;
			break;
		}
		case '-':
		{
			auto next = peek_character();
			if (next == '>'){ 
				token.type = TOKEN_ARROW;
				eat_character();
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
			eat_until_whitespace();
			break;
		}
		// Compiler Directives
		case '#':
		{
			token.type = TOKEN_DIRECTIVE;
			eat_until_whitespace();
			break;
		}
		case '=':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.type = TOKEN_EQL;
			}
			else {
				token.type = ch;
			}
			break;
		}
		case '<':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.type = TOKEN_LT_OR_EQL;
			} else if (next == '<'){
				eat_character();
				token.type = TOKEN_SHIFT_LEFT;
			} else {
				token.type = TOKEN_LT;
			}
			break;
		}
		case '>':
		{
			auto next = peek_character();
			if (next == '='){
				eat_character();
				token.type = TOKEN_GT_OR_EQL;
			} else if (next == '>'){
				eat_character();
				token.type = TOKEN_SHIFT_RIGHT;
			} else {
				token.type = TOKEN_GT;
			}
			break;
		}
		case '.':
		{
			auto next = peek_character();
			if (next == '.') {
				eat_character();
				token.type = TOKEN_DOUBLEDOT;
			}
			else {
				token.type = ch;
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
			token.type = TOKEN_LITERAL;
			token.kind = TOKEN_KIND_STRING_LITERAL;
			break;	  
		}
		case ':':
		{
			if (peek_character() == ':')
			{
				eat_character();
				token.type = TOKEN_DOUBLE_COLON;
			}
			else if (peek_character() == '='){
				token.type = TOKEN_COLON_EQUAL;
				eat_character();
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
				while(true){
					if (!isLiteralChar(peek_character()))
						break;
					eat_character();
				};
				token.value = StringView{ &input[temp], input_cursor  - temp};
				token.type = TOKEN_IDENT;
				if (isKeyword(token.value))
					token.type = TOKEN_KEYWORD;
				else if (isHDtype(token.value))
					token.type = TOKEN_HDTYPE;
				else
					token.type = TOKEN_IDENT;
			}
			else 
			{
				// TODO: Handle floating points ... 
				// TODO: handle the real values instead of the string
				// We assume that this will be only numbers
				// (0x -> hex) (0b -> binary) (0c  -> Octal)
				// auto& pos = get_current_position();
				assert(isDigit(ch));
				token.type = TOKEN_LITERAL;
				token.kind = TOKEN_KIND_INT_LITERAL;
				auto peeked = peek_character();
				if(peeked == 'x' ||  peeked == 'b' || peeked == 'c')  eat_character();
				while (isDigit(peek_character()))eat_character();
				
			}
			break;
		}
	}
	token.end_position = get_current_position();
	assert(token.type != TOKEN_NONE);
	token.value = StringView{ &input[temp], input_cursor  - temp};
	token.hash = Hash(token);
	
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