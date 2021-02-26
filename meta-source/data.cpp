
#define Index 256
#define EnumName(x) Tokens
_enum(EnumName(x),
	  entry_name_value(IDENT, Index)
	  //entry_name(IDENT = Index)
	  entry_name(HDTYPE)
	  entry_name(KEYWORD)
	  entry_name(LITERAL)
	  entry_name(COMMENT)
	  entry_name(MULTILINE_COMMENT)
	  entry_name(COLON)
	  entry_name(DOUBLECOLON)
	  entry_name(COLONEQUAL)
	  entry_name(OPERATOR)
	  entry_name(DIRECTIVE)
	  entry_name(DOUBLEDOT)
	  entry_name(STRING_LITERAL)
	  entry_name(INT_LITERAL)
	  entry_name(FLOAT_LITERAL)
	  entry_name(ARROW)
	  entry_name(ASSIGN)
	  entry_name(EQL)
	  entry_name(GT)
	  entry_name(LT)
	  entry_name(GT_OR_EQL)
	  entry_name(LT_OR_EQL)
	  entry_name(SHIFT_LEFT)
	  entry_name(SHIFT_RIGHT)
	  entry_name(NONE)
	  entry_name(EOFA)
	  entry_name(ERROR),
	  ;
	  )

string_function(EnumName(x))
