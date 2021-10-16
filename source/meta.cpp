/* ========================================================================
   $File: meta.cpp
   $Date: 2021-05-29
   $Creator: Husam Dababneh
   $Description: Meta-Program to generate some data
   ========================================================================*/

#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#pragma warning( disable : 4456 )


#include "base.cpp"

#define LEXER_ENABLE_C_CHAR_TOKEN 1
#include "lex.cpp"
#define FOR(x) for (U64 it = 0; it < x; it++)
#define ArraySize(x) sizeof(x)/sizeof(x[0])

FILE* file ;

inline 
void _expect_and_eat_2(LexerState& lexer , U64 type, U64 line, const char* _file)
{
	Token token = peek_token(lexer);
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", _file ,line);
		printf("Expected [%c, %d] got %.*s \n", (char)type, (int)type, SV_PRINT(token.name));
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.line, token.start_position.index);
		exit(-1);
	}
	eat_token(lexer);
	
}

//_CrtDbgBreak();
#define expect_and_eat(x) _expect_and_eat_2(lex, x, __LINE__, __FILE__)


StringView NoteTypes[] {
	"{"_s,
	"Note"_s,
	"Todo"_s,
};


S64 NoteToInt(const StringView& str) 
{
	FOR(ArraySize(NoteTypes)){
		if (EqualStrings(str, NoteTypes[it])){
			return it;
		}
	}
	return -1;
}

struct GenerateStringsCommand {
	String name;
	String* array;
};

static String current_filename = {0};

void enums_handler(LexerState& lex)
{
	
	Token token = eat_token(lex);
	S64 type = NoteToInt(token.name);
	
	GenerateStringsCommand command;
	command.array = NULL;
	switch(type)
	{
		// '{'
		case 0:
		{
			return;
		}
		
		//  NOTE:
		case 1:
		{
			//printf("Filename %.*s\n", SV_PRINT(current_filename));
			//printf("Token is [%.*s]\n", SV_PRINT(token.name));
			expect_and_eat('(');
			if (!EqualStrings(peek_token(lex).name, "GenerateStrings"_s)){
				// TODO : Better error message
				assert(false);
			}
			
			eat_token(lex).name; // Eat Argument (GenerateStrings")
			expect_and_eat(',');
			command.name = eat_token(lex).name;
			expect_and_eat(')'); // Eat (
			
			
			expect_and_eat('{'); // Eat {
			
			token = eat_token(lex);
			while(token.type == TOKEN_IDENT) {
				//printf("%s \n", *token.name);
				
				arrput(command.array, token.name);
				
				if (peek_token(lex).type == '=')
				{
					auto equal = eat_token(lex); // eat '='
					
					// TODO: Handle Shift operations ? 
					auto value = eat_token(lex); // eat value 
				}
				
				expect_and_eat(',');
				token = eat_token(lex);
				if (token.type == '}') break;
			}
			
			// Generate Strings for the enum
			//expect_and_eat('}');
			expect_and_eat(';');
			
			
			break;
		}
		// TODO: 
		case 2:
		{
			return;
		}
		default: 
		{
			return;
			//assert(false);
			//exit(-1);
		}
		
	}
	
	int size = MB(0.5);
	char* buffer = new char[size];
	int writen= 0;
	
	writen += sprintf_s(buffer+ writen, size - writen, "#include \"%.*s\"\n", SV_PRINT(current_filename));
	writen += sprintf_s(buffer+writen, size - writen, "const StringView %.*s_to_string(U64 c){\n", SV_PRINT(command.name));
	writen += sprintf_s(buffer+ writen, size - writen, "\tswitch(c){\n");
	FOR(arrlenu(command.array)){
		writen += sprintf_s(buffer+ writen, size- writen, "\t\tcase %.*s: return \"%.*s\"_s;  \n", SV_PRINT(command.array[it]), SV_PRINT(command.array[it]));
	}
	
	//writen += sprintf_s(buffer+ writen, size - writen, "\t\tdefault: return \"UNKNOWN_STRING\"_s;\n\t}\n}\n");
	
	writen += sprintf_s(buffer+ writen, size - writen, "\t\tdefault: return \"\"_s;\n\t}\n}\n");
	
	//printf("%s", buffer);
	//open_file(file, "./generated.cpp", "w");
	fprintf(file, buffer);
	
	delete[] buffer;
}


void parse_struct_members(LexerState& lex, StringView struct_name)
{
	while(peek_token(lex).type != '}'){
		Token type = eat_token(lex);
		Token name = eat_token(lex);
		eat_until_token_by_type(lex, TOKEN_SEMI_COLON);
		printf("\t{ offsetof(%.*s, %.*s), member_size(%.*s, %.*s) },\n", 
			   SV_PRINT(struct_name), SV_PRINT(name.name),
			   SV_PRINT(struct_name), SV_PRINT(name.name));
	}
	eat_token(lex);// eat '}'
}

[[nodiscard]]
bool parse_file (const StringView& filename) {
	current_filename = filename;
	
	LexerState lex = {};//(filename); 
	lex.input = readEntireFileToStringView(filename);
	lex.config.ignore_comments = true;
	Token token = eat_token(lex);
	while(token.type != TOKEN_EOFA)
	{
		if (EqualStrings(token.name, "enum"_s))
		{
			enums_handler(lex);
		}
		
		if (EqualStrings(token.name, "introspect"_s))
		{
			// TODO(Husam Dababneh): Unimplemented yet ?? 
			//printf("--- Kawabanga\n");
		}
		
		
		
		/* 		
				if (EqualStrings(token.name, "padding_check"_s))
				{
					eat_token(lex);// eat 'struct'
					Token struct_name = eat_token(lex);
					printf("Name: %.*s\n", SV_PRINT(struct_name.name)); 
					expect_and_eat('{');
					parse_struct_members(lex, struct_name.name);
					
				}
				
				 */
		
		
		token = eat_token(lex);
		
	}
	
	return true;
}

int main (int argc, char** argv){
	
	// C-Style .. turn off warning C4100 :) 
	(void)argv;
	(void)argc;
	
	StringView filenames[] = {
		"lex.h"_s,
		"constants.h"_s,
		"ast.h"_s,
	};
	
	open_file(outfile, "./generated.cpp", "w");
	file = outfile;
	FOR(ArraySize(filenames)) {
		printf("[Meta]: Parsing %.*s Started\n", SV_PRINT(filenames[it]));
		bool success = parse_file(filenames[it]);
		
		if (!success) {
			printf("[Meta]: Failed to parse %.*s\n", SV_PRINT(filenames[it]));
		}
		//printf("\n\n\n\n\n\n\n");
	}
	
	fclose(file);
}
