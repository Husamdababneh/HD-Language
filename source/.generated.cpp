const StringView& token_type_to_string(u64 c){
	switch(c){
		case TOKEN_EQUAL: return "TOKEN_EQUAL"_s;  
		case TOKEN_IDENT: return "TOKEN_IDENT"_s;  
		case TOKEN_HDTYPE: return "TOKEN_HDTYPE"_s;  
		case TOKEN_KEYWORD: return "TOKEN_KEYWORD"_s;  
		case TOKEN_LITERAL: return "TOKEN_LITERAL"_s;  
		case TOKEN_COMMENT: return "TOKEN_COMMENT"_s;  
		case TOKEN_MULTILINE_COMMENT: return "TOKEN_MULTILINE_COMMENT"_s;  
		case TOKEN_COLON: return "TOKEN_COLON"_s;  
		case TOKEN_DOUBLE_COLON: return "TOKEN_DOUBLE_COLON"_s;  
		case TOKEN_COLON_EQUAL: return "TOKEN_COLON_EQUAL"_s;  
		case TOKEN_OPERATOR: return "TOKEN_OPERATOR"_s;  
		case TOKEN_DIRECTIVE: return "TOKEN_DIRECTIVE"_s;  
		case TOKEN_DOUBLEDOT: return "TOKEN_DOUBLEDOT"_s;  
		case TOKEN_ARROW: return "TOKEN_ARROW"_s;  
		case TOKEN_ASSIGN: return "TOKEN_ASSIGN"_s;  
		case TOKEN_EQL: return "TOKEN_EQL"_s;  
		case TOKEN_GT: return "TOKEN_GT"_s;  
		case TOKEN_LT: return "TOKEN_LT"_s;  
		case TOKEN_GT_OR_EQL: return "TOKEN_GT_OR_EQL"_s;  
		case TOKEN_LT_OR_EQL: return "TOKEN_LT_OR_EQL"_s;  
		case TOKEN_SHIFT_LEFT: return "TOKEN_SHIFT_LEFT"_s;  
		case TOKEN_SHIFT_RIGHT: return "TOKEN_SHIFT_RIGHT"_s;  
		case TOKEN_NONE: return "TOKEN_NONE"_s;  
		case TOKEN_EOFA: return "TOKEN_EOFA"_s;  
		case TOKEN_ERROR: return "TOKEN_ERROR"_s;  
		default: return c;
	}
}
