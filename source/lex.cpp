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

const char* Keywords [] = {
	"u32", "u8", "char",  "if", "else", "while", "for",
	"continue", "delete", "new", "InputFile", "OutputDir"
};

String reserved [] = {
	"u32"_s,
	"u8"_s,
	"char"_s,
	"if"_s,
	"else"_s,
	"while"_s,
	"for"_s,
	"continue"_s,
	"delete"_s,
	"new"_s,
	"InputFile"_s,
	"OutputDir"_s,
};
constexpr extern int KeywordsCount = sizeof(Keywords)/sizeof(const char *);

static inline bool isWhitechar(u8 c){
	if (c == '\t' ||
		c ==  '\n' ||
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
	for(int a = 0; a < KeywordsCount; a++)
		if(isEqual(string, reserved[a]))
			return true;
	return false;
}

bool IsReserved(std::string& string)
{
	for(int a = 0; a < KeywordsCount; a++)
		if(string.compare(Keywords[a]) == 0)
			return true;
	return false;
}


// @CleanUp Is this usefull ?? 
inline Position LexerState::get_position_from_cursor(u64 cursor)
{
	return { current_line_number ,  current_char_index };	
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
	// TODO: checks
	while(true){
		auto ch = eat_character();
		if (isWhitechar(ch)) continue;
		return ch;
	}
	return 0;
}

u8 LexerState::eat_character()
{
	
	switch(input[input_cursor])
	{
	  case '\n':
		  previous_token_line_number = current_line_number;
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
LexerState::LexerState(const char * filepath)
{
	printf("initing Lexer\n");
	FILE* file = fopen(filepath, "rb");
	filename = (char *)filepath;
	if(!file)
	{
		printf("Couldn't find file [%s]\n", filepath);
		exit(1);
	}
	
	int length = read_entire_file(file, (void**)&input.data);
	if (length < 0)
	{
		printf("Couldn't read file [%s]\n", filepath);
		exit(1);
	}
	input.count = length;
	input_cursor = 0;
	cursor.string = input;
	cursor.cursor = 0;
	current_line_number = 1;
	current_char_index = 0;
	previous_token_line_number = 0;
	fclose(file);
}

LexerState::~LexerState()
{
	printf("Getting Out\n");
	delete[] input.data;
	input.data = nullptr;
}


// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// TODO: @Important !!!! : String Implementation ASAP
// @(TEMP):
#define ERRSTRING(x) String { sizeof(x), (u8*)x }

// Rename to eat_next_token
Token LexerState::peek_next_token()
{
	// @TODO: init a token and return it.
	
	if(input_cursor >= input.count)
		return { ETOKEN::EOFA , 0 , 0 };
	
	Token token = {};
	int temp = input_cursor;
	u8 ch = eat_character();
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
			  token.value = String {&input[temp], input_cursor  - temp};
			  return token;
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
			  token.value = String {&input[temp], input_cursor  - temp};
			  break;	  
		  } else {
			  // division ?? 
			  token.Type = (ETOKEN) ch;
			  token.value = String{&input[input_cursor], 1,};
			  break;	  
		  }
	  }
	  break;
	  case '+': case '-': case '*': case '{': case '}':
	  case '[': case ']': case '=': case ';': case '.':
	  case '(': case ')': case ',': case '<': case '>':
	  case '~': case '!': case '$': case '%': case '^':
	  case '&': case '?': case '|': case '`': case '\\':
	  case '\'': case '@':
		  token.Type = (ETOKEN) ch;
		  token.value = String{&input[input_cursor], 1};
		  break;
	  case '#':
		  // Compile Time Execution operator
		  // What follows # should be executed at compile time (Just Like Jai)
		  token.Type = ETOKEN::DIRECTIVE;
		  // This is not right .. the behaviour should be to stop on first whitespace
		  // but since i'm getting used to the lexer i wanted to try some stuff - Husam 9/28/2020
		  while(true){
			  auto eaten = peek_character();
			  if(eaten == ' ' || eaten == '\t' || eaten == ';' || eaten == '\n')
				  break;
			  eat_character();
		  };
		  token.value = String { &input[temp], input_cursor  - temp};
		  break;
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
		  token.value = String{&input[temp], input_cursor - temp};
		  break;	  
	  }
	  break;
	  case ':':
	  {
		  
		  auto next = eat_until_character();
		  token.value = String {&input[temp], input_cursor - temp};
		  if (next == ':')  {
			  token.Type = ETOKEN::DOUBLECOLON; // uninitailizaed
			  eat_character();
		  }
		  else if (next == '=')  {
			  token.Type = ETOKEN::COLONEQUAL; // initialized
			  eat_character();
		  }
		  else  {
			  input_cursor--;
			  token.Type = ETOKEN::COLON;
			  token.value = String{&input[temp], 1};
		  } 
		  break;	  
	  }
	  break;
	  case ' ': case '\t': case '\n': case '\r': break;
	  default:
		  if (isAlphabet(ch) || ch == '_'){
			  while(true){
				  auto next = peek_character();
				  if (!isInLireralChar(next))
					  break;
				  eat_character();
			  };
			  token.value = String{&input[temp], input_cursor - temp};
			  if (isKeyword(token.value))
				  token.Type = ETOKEN::KEYWORD;
			  else
				  token.Type = ETOKEN::IDENT;
			  
			  
		  } else {
			  // We assume that this will be only numbers
			  // (0x -> hex) (0b -> binary) (0o  -> Octal)
			  // auto& pos = get_current_position();
			  // std::cout << "Location :(" << pos.x << ',' << pos.y << ") \n" ;
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
			  token.value = String{&input[temp], input_cursor - temp };
		  }
		  break;
	}
	token.end_position = get_current_position();	
	return token;
}


// @Temp::
#define LOCATION 
#ifdef LOCATION 
#define POSI " Location :(" << token.start_position.x << ',' << token.start_position.y << ")"
#else
#define POSI "" 
#endif
