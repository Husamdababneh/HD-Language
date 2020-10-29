/* ========================================================================
   $File: lex.cpp
   $Date: 2020-04-04
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Defines Lexer functionality & Data Structures
   ========================================================================*/ 



#include "lex.h"

#include "common.h"
#include "String.h"
#include "auxiliary.h"

String reserved [] = {
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

constexpr int KeywordCount = sizeof(reserved)/sizeof(String);

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
		if(isEqual(string, reserved[a]))
			return true;
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

u8 LexerState::eat_character()
{
	
	switch(input[input_cursor])
	{
	  case '\n':
		  current_line_number++;
		  current_char_index = 0;
		  break;
	  case '\t':
		  current_char_index+=4;
		  break;
	  default:
		  current_char_index++;
		  break;
	}
	input_cursor++;
	return input[input_cursor - 1];
}
char * filename = nullptr;
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
}

LexerState::~LexerState()
{
	printf("Getting Out\n");
	delete[] input.data;
	input.data = nullptr;
}

Token last_token = {};
Token LexerState::eat_token()
{
	// @TODO: init a token and return it.
	
	if(input_cursor >= input.count)
		return { ETOKEN::EOFA , 0 , 0 };
	
	Token token = {};
	
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
			  token.Type = ETOKEN::COMMENT;
			  while(eat_character() != '\n');
		  } else if (next == '*'){
			  token.Type = ETOKEN::MULTILINE_COMMENT;
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
			  token.Type = (ETOKEN) ch;
			  break;	  
		  }
		  break;
	  }
	  break;
	  case '+': case '-': case '*': case '{': case '}':
	  case ';': case '[': case ']': case '`':
	  case '(': case ')': case ',': 
	  case '~': case '!': case '$': case '%': case '^':
	  case '&': case '?': case '|': case '@':
	  case '\'': case '\\':
		  token.Type = (ETOKEN) ch;
		  break;
	  case '#':
		  // Compile Time Execution operator
		  // What follows # should be executed at compile time (Just Like Jai)
		  token.Type = ETOKEN::DIRECTIVE;
		  // This is not right .. the behaviour should be to stop on first whitespace
		  // but since i'm getting used to the lexer i wanted to try some stuff - Husam 9/28/2020
		  while(true){
			  auto eaten = peek_character();
			  if(isWhitechar(eaten))
				  break;
			  eat_character();
		  };
		  break;
	  case '=':
	  {
		  auto next = peek_character();
		  if (next == '='){
			  eat_character();
			  token.Type = ETOKEN::EQL;
		  }
		  else {
			  token.Type = ETOKEN::ASSIGN;
		  }
		  break;
	  }
	  case '<':
	  {
		  auto next = peek_character();
		  if (next == '='){
			  eat_character();
			  token.Type = ETOKEN::LT_OR_EQL;
		  } else if (next == '<'){
			  eat_character();
			  token.Type = ETOKEN::SHIFT_LEFT;
		  } else {
			  token.Type = ETOKEN::LT;
		  }
		  break;
	  }
	  case '>':
	  {
		  auto next = peek_character();
		  if (next == '='){
			  eat_character();
			  token.Type = ETOKEN::GT_OR_EQL;
		  } else if (next == '>'){
			  eat_character();
			  token.Type = ETOKEN::SHIFT_RIGHT;
		  } else {
			  token.Type = ETOKEN::GT;
		  }
		  break;
	  }
	  case '.':
	  {
		  auto next = peek_character();
		  if (next == '.') {
			  eat_character();
			  token.Type = ETOKEN::DOUBLEDOT;
		  }
		  else if (next == '>'){
			  eat_character();
			  token.Type = ETOKEN::ARROW;
		  } else {
			  token.Type = (ETOKEN)ch;
		  }
		  break;
	  }
	  case '"':
	  {
		  // @Cleanup: We could check the prev instead ?? wont it be easer ??
		  u8 escape = 0;
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
		  token.Type = ETOKEN::LITERAL;
		  break;	  
	  }
	  break;
	  case ':':
	  {
		  auto next = peek_character();
		  token.value = String {&input[temp], input_cursor - temp};
		  if (next == ':')  {
			  token.Type = ETOKEN::DOUBLECOLON; // uninitailizaed
			  eat_character();
		  }
		  else if (next == '=')  {
			  token.Type = ETOKEN::COLONEQUAL; // initialized
			  eat_character();
		  }
		  else{
			  token.Type = ETOKEN::COLON;
		  } 
		  break;
	  }
	  break;
	  //case ' ': case '\t': case '\n': case '\r': break;
	  case '\0':
		  return { ETOKEN::EOFA , 0 , 0 };
		  break;
	  default:
		  if (isAlphabet(ch) || ch == '_'){
			  while(true){
				  auto next = peek_character();
				  if (!isInLireralChar(next))
					  break;
				  eat_character();
			  };
			  if (isKeyword(token.value))
				  token.Type = ETOKEN::KEYWORD;
			  else
				  token.Type = ETOKEN::IDENT;
			  
			  
		  } else {
			  // We assume that this will be only numbers
			  // (0x -> hex) (0b -> binary) (0o  -> Octal)
			  // auto& pos = get_current_position();
			  assert(isDigit(ch));
			  token.Type = ETOKEN::LITERAL;
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
	assert(token.Type != ETOKEN::NONE);
	//last_token = token;
	token.value = String { &input[temp], input_cursor  - temp};
	return token;
}


// TODO:@Optimization(Husam): This may be called to many times ?? use cache ??
// 
Token LexerState::peek_token(int lookAhead /* = 0*/ )
{
	u64 input_cursor = this->input_cursor;
	u16 current_line_number = this->current_line_number;
	u16 current_char_index = this->current_char_index;
	auto result =  eat_token();
	for(int counter = 0; counter < lookAhead; counter++)
		result = eat_token();
	this->input_cursor = input_cursor;
	this->current_line_number = current_line_number;
	this->current_char_index = current_char_index;
	return result;
}

// @Temp::
#define LOCATION 
#ifdef LOCATION 
#define POSI " Location :(" << token.start_position.x << ',' << token.start_position.y << ")"
#else
#define POSI "" 
#endif
