/*
*
* DO NOT EDIT THIS FILE . This File is generated by "generator.py"
*
*/

#include "pch.h"
#include "constants.h"

String ETOKENStrings[] =   {
    "IDENT"_s,
    "KEYWORD"_s,
    "LITERAL"_s,
    "COMMENT"_s,
    "MULTILINE_COMMENT"_s,
    "COLON"_s,
    "DOUBLECOLON"_s,
    "COLONEQUAL"_s,
    "OPERATOR"_s,
    "DIRECTIVE"_s,
    "DOUBLEDOT"_s,
    "STRING_LITERAL"_s,
    "INT_LITERAL"_s,
    "FLOAT_LITERAL"_s,
    "ARROW"_s,
    "ASSIGN"_s,
    "EQL"_s,
    "GT"_s,
    "LT"_s,
    "GT_OR_EQL"_s,
    "LT_OR_EQL"_s,
    "SHIFT_LEFT"_s,
    "SHIFT_RIGHT"_s,
    "NONE"_s,
    "EOFA"_s,
    "ERROR"_s,
};

String ETOKENToString(u64 var) {
    if (var < TOKEN_IDENT)
        return {(u8*) var , 1};
    return ETOKENStrings[(u64)var - (u64)TOKEN_IDENT  + 1];
};

