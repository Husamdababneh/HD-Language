/* ========================================================================
   $File: meta.cpp
   $Date: 2021-05-29
   $Creator: Husam Dababneh
   $Description: Meta-Program to generate some data
   ========================================================================*/

#include "base.h"
#include "lex.cpp"
#define FOR(x) for (u64 it = 0; it < x; it++)
#define ArraySize(x) sizeof(x)/sizeof(x[0])

FILE* file ;

inline 
void _expect_and_eat(LexerState& lexer , u64 type, u64 line, const char* file)
{
	Token token = lexer.peek_token();
	if(token.type != type)
	{
		printf("expect_and_eat: %s %zd\n", file ,line);
		printf("Expected [%c, %d] got %.*s \n", (char)type, (int)type, SV_PRINT(token.name));
		printf("Token[%.*s] Located in Line: %d, Col: %d \n", SV_PRINT(token.name), token.start_position.line, token.start_position.index);
		exit(-1);
	}
	lexer.eat_token();
	
}

//_CrtDbgBreak();
#define expect_and_eat(x) _expect_and_eat(lex, x, __LINE__, __FILE__)


StringView NoteTypes[] {
	"{"_s,
	"Note"_s,
	"Todo"_s,
};


s64 NoteToInt(const StringView& str) 
{
	FOR(ArraySize(NoteTypes)){
		if (cmp2sv(str, NoteTypes[it]) == 0){
			return it;
		}
	}
	return -1;
}

struct GenerateStringsCommand {
	StringView  name;
	StringView* array;
};

static StringView current_filename = {0};

void enums_handler(LexerState& lex)
{
	
	Token token = lex.eat_token();
	s64 type = NoteToInt(token.name);
	
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
			expect_and_eat('(');
			if (cmp2sv(lex.peek_token().name, "GenerateStrings"_s) != 0){
				// TODO : Better error message
				assert(false);
			}
			
			lex.eat_token().name; // Eat Argument (GenerateStrings")
			expect_and_eat(',');
			command.name = lex.eat_token().name;
			expect_and_eat(')'); // Eat (
			
			
			expect_and_eat('{'); // Eat {
			
			token = lex.eat_token();
			while(token.type == TOKEN_IDENT) {
				//printf("%s \n", *token.name);
				
				arrput(command.array, token.name);
				
				if (lex.peek_token().type == '='){
					while (lex.peek_token().type != ','){
						auto a = lex.eat_token(); // eat = 
						if (a.type == '\'') {
							lex.eat_until('\'');
						}
						
					}
				}
				
				expect_and_eat(',');
				token = lex.eat_token();
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
			assert(false);
			exit(-1);
		}
		
	}
	
	char* buffer = new char[MB(0.5)];
	int size = MB(0.5);
	int writen= 0;
	printf("%zd ......... \n", arrlenu(command.array));
	
	writen += sprintf_s(buffer+ writen, size - writen, "#include \"%s\"\n", *current_filename);
	writen += sprintf_s(buffer+writen, size - writen, "const StringView %s_to_string(u64 c){\n", *command.name);
	writen += sprintf_s(buffer+ writen, size - writen, "\tswitch(c){\n");
	FOR(arrlenu(command.array)){
		writen += sprintf_s(buffer+ writen, size- writen, "\t\tcase %s: return \"%s\"_s;  \n", *command.array[it], *command.array[it]);
	}
	
	writen += sprintf_s(buffer+ writen, size - writen, "\t\tdefault: return \"\"_s;\n\t}\n}\n");
	
	printf("%s", buffer);
	//open_file(file, "./generated.cpp", "w");
	fprintf(file, buffer);
	
	delete[] buffer;
}


[[nodiscard]]
bool parse_file (const StringView& filename) {
	current_filename = filename;
	LexerState lex(filename); 
	lex.config.ignore_comments = true;
	Token token = lex.eat_token();
	while(token.type != TOKEN_EOFA)
	{
		if (cmp2sv(token.name, "enum"_s) == 0)
		{
			enums_handler(lex);
		}
		
		
		token = lex.eat_token();
	}
	
	return true;
}

int main (int argc, char** argv){
	
	StringView filenames[] = {
		"lex.h"_s,
		"constants.h"_s,
		"ast.h"_s,
	};
	
	open_file(outfile, "./generated.cpp", "w");
	file = outfile;
	FOR(ArraySize(filenames)) {
		printf("[Meta]: Parsing %s Started\n", *filenames[it]);
		bool success = parse_file(filenames[it]);
		
		if (!success) {
			printf("[Meta]: Failed to parse %s\n", *filenames[it]);
		}
		
	}
	
	fclose(file);
	
}