/* ========================================================================
   $File: parser.cpp
   $Date: 2020-10-16
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: parser.cpp
   ========================================================================*/

#include "parser.h"
#include "lex.h"
#include "array.h"
#include <meow_hash.h>


static void
PrintHash(meow_u128 Hash)
{
    printf("%08X-%08X\n",
		   MeowU32From(Hash,1),
		   MeowU32From(Hash,0));

}


Ast_Declaration parse_declaration(LexerState* lexer, Logger* logger) {
	Ast_Declaration result;
	auto token = lexer->eat_token();

	if (token.Type == (ETOKEN)')')
		return {};
	
	if (token.Type != ETOKEN::IDENT)
		logger->print_line(&token, "Error: expected an identifer\n"_s);
	
	result.name = token.name;

	token = lexer->eat_token();
	if (token.Type != ETOKEN::COLON)
		logger->print_line(&token, "Error: expected ':'\n"_s);

	token = lexer->eat_token();
	// this is wrong we need split the keywords to "type keyword" and  "language keyword"
	if (token.Type != ETOKEN::IDENT &&  token.Type != ETOKEN::KEYWORD)
		logger->print_line(&token, "Error: expected a Type\n"_s);
	result.type = token.name;


	token = lexer->peek_token();
	if (token.Type == ETOKEN::ASSIGN){
		lexer->eat_token();
		token = lexer->eat_token(); // this token should be known before assebmleing 
		result.value = token.name;
	}

	return result;
}

Ast_Declaration parse_caller_argument(LexerState* lexer, Logger* logger) {
	Ast_Declaration result;
	auto token = lexer->eat_token();
	
	if (token.Type == ETOKEN::IDENT){
		result.name = token.name;
		if (lexer->peek_token().Type == ETOKEN::ASSIGN){
			lexer->eat_token();
			token = lexer->peek_token();
			if (token.Type == ETOKEN::IDENT ||
				token.Type == ETOKEN::KEYWORD ||
				token.Type == ETOKEN::LITERAL)
			{
				lexer->eat_token();
				result.type = token.name;
				return result;
			}
		}
	}
	
	return {};
}

Ast_ParmeterList parse_caller_arguemnts_list(LexerState* lexer, Logger* logger){

	Ast_ParmeterList list = {};
	auto token = lexer->peek_token();
	if (token.Type == (ETOKEN)')')
		return list;
	
	list.declerations = init_array<Ast_Declaration>();	
	u8 argumentIndex = 0;
	while(true)
	{
		Ast_Declaration dec = parse_caller_argument(lexer,logger);
		if (dec.name.data == nullptr)
			break;
		array_add<Ast_Declaration>(&list.declerations, dec);
		
		token = lexer->eat_token();
		if (token.Type == (ETOKEN)','){	
			continue;
		}
		else if (token.Type == (ETOKEN)')') {
			break;
		}
		else {
			logger->print_line(&token, "Error: unexpected token %s\n"_s ,token.name);
			exit(-1);
		}
				
	}
	return list;
}

Ast_ParmeterList parse_declaration_list(LexerState* lexer, Logger* logger){

	Ast_ParmeterList list = {};
	auto token = lexer->peek_token();
	if (token.Type == (ETOKEN)')')
		return list;
	
	list.declerations = init_array<Ast_Declaration>();

	u8 argumentIndex = 0;
	while(true)
	{
		Ast_Declaration dec = parse_declaration(lexer,logger);
		if (dec.name.data == nullptr)
			break;
		array_add<Ast_Declaration>(&list.declerations, dec);
		
		token = lexer->eat_token();
		if (token.Type == (ETOKEN)','){	
			continue;
		}
		else if (token.Type == (ETOKEN)')') {
			break;
		}
		else {
			logger->print_line(&token, "Error: unexpected token %s\n"_s ,token.name);
			exit(-1);
		}
				
	}
	return list;
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

				  // TODO:@Clean: cleanup String class
				  String sturctStr = "struct"_s;
				  if (isEqual(&next.value, &sturctStr)){
					  logger.print("Struct [%s]\n"_s , token.value);
				  }else if (next.Type == (ETOKEN)'(') {
					  logger.print("Function [%s]\n"_s , token.value);
					  lexer.eat_token();
					  Ast_ParmeterList arguments = parse_declaration_list(&lexer, &logger);
					  for (int a = 0; a < arguments.declerations.occupied; a++){
						  logger.print("IDENT: %s is %s\n"_s,
									   arguments.declerations[a]->name,
									   arguments.declerations[a]->type);
					  }

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
				  }
				  else{
					  next = lexer.eat_token();
					  logger.print("\t\t type[%s]\n"_s , next.value);
				  }
			  }
			  else if (next.Type == (ETOKEN)'('){
				  lexer.eat_token();
				  logger.print("Function Call [%s]\n"_s , token.value);
				  
				  Ast_ParmeterList arguments = parse_caller_arguemnts_list(&lexer, &logger);
				  for (int a = 0; a < arguments.declerations.occupied; a++){
					  logger.print("IDENT: %s is %s\n"_s,
								   arguments.declerations[a]->name,
								   arguments.declerations[a]->type);
				  }
			  }
			  else{
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





