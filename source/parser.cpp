/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "parser.h"
#include "lex.h"



void parse_file(const String& filename){
	Logger logger = Logger(filename);
	LexerState lexer(filename);
	//auto token = lexer.eat_token();

	int a = 0;
	auto token = lexer.eat_token();
	while(token.Type != ETOKEN::EOFA){
		switch(token.Type)
		{
		  case ETOKEN::DIRECTIVE:
			  // TODO
			  break;
		  case ETOKEN::COMMENT:
		  case ETOKEN::MULTILINE_COMMENT:
			  // TODO
			  break;
		  case ETOKEN::IDENT:
		  {
			  auto next = lexer.peek_token();
			  if (next.Type == ETOKEN::DOUBLECOLON)
			  {
				  // Functions are considered constants since thet will nerver change
				  lexer.eat_token();
				  //logger.print("Function or Constant[%s]\n"_s , token.value);
				  
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
		}
		token = lexer.eat_token();
	}
	logger.print("Directives count = %d\n"_s , a);
	
}





