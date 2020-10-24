/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "parser.h"
#include "lex.h"
#include <meow_hash.h>


static void
PrintHash(meow_u128 Hash)
{
    printf("%08X-%08X\n",
		   MeowU32From(Hash,1),
		   MeowU32From(Hash,0));

}

void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);

	int a = 0;
	auto token = lexer.eat_token();
	while(token.Type != ETOKEN::EOFA){
		switch(token.Type)
		{
		  case ETOKEN::DIRECTIVE:
			  //logger.print("Token Type Name = [%s]\n"_s, ETOKENToString(token.Type));
			  // TODO
			  break;
		  case ETOKEN::COMMENT:
		  case ETOKEN::MULTILINE_COMMENT:
			  // TODO
			  break;
		  case ETOKEN::IDENT:
		  {
			  // meow_u128 Hash = MeowHash(MeowDefaultSeed, token.value.count, token.value.data);
			  // logger.print("%s hash is :"_s , token.value);
			  // PrintHash(Hash);
			  auto next = lexer.peek_token();
			  if (next.Type == ETOKEN::DOUBLECOLON)
			  {
				  // Functions are considered constants since thet will nerver change
				  lexer.eat_token();
				  
				  //logger.print("Function or Constant Or Struct [%s]\n"_s , token.value);
				  next = lexer.peek_token();

				  if (isEqual(next.value, "struct"_s)){
					  logger.print("Struct [%s]\n"_s , token.value);
				  }else if (next.Type == (ETOKEN)'(') {
					  lexer.eat_token();
					  //while(
					  //parse_arguemnt_list();
				  }				  
			  }
			  else if (next.Type == ETOKEN::COLONEQUAL){
				  lexer.eat_token();
				  logger.print("Declaration With inferred type ident[%s]\n"_s , token.value);
				  
			  }
			  else if (next.Type == ETOKEN::COLON){
				  lexer.eat_token();
				  logger.print("Possible With type Declaration ident[%s]\n"_s , token.value);
				  //parse_possible_type();
				  if(lexer.peek_token().Type == (ETOKEN)'['){
					  lexer.eat_token();
					  next = lexer.eat_token();
					  logger.print("\t\t type[  [%s]  ]\n"_s , next.value);
					  next = lexer.eat_token();
				  }else{
					  next = lexer.eat_token();
					  logger.print("\t\t type[%s]\n"_s , next.value);
				  }
			  }else{
				  // TODO: report errors
			  }
		  }
		  break;
		  case ETOKEN::KEYWORD :
			  break;
		  case ETOKEN::LITERAL :
			  break;
		  case ETOKEN::COLON :
			  break;
		  case ETOKEN::DOUBLECOLON :
			  break;
		  case ETOKEN::COLONEQUAL :
			  break;
		  case ETOKEN::OPERATOR :
			  break;
		  case ETOKEN::DOUBLEDOT :
			  break;
		  case ETOKEN::NONE :
			  break;
		  case ETOKEN::EOFA :
			  break;
		  case ETOKEN::ERROR :
			  break;
		  default:
			  //logger.print("Error: Unexpected Token of type  [%s]\n"_s, token.value);
			  break;
		}
		token = lexer.eat_token();
	}
//	logger.print("Directives count = %d\n"_s , a);
	
}





