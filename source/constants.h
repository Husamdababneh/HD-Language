/*
*
* DO NOT EDIT THIS FILE . This File is generated by "generator.py"
*
*/

#include "common.h"
#include "String.h"
enum class ETOKEN : u64 {
    IDENT = 256,
    KEYWORD,
    LITERAL,
    COMMENT,
    MULTILINE_COMMENT,
    COLON,
    DOUBLECOLON,
    COLONEQUAL,
    OPERATOR,
    DIRECTIVE,
    DOUBLEDOT,
    STRING_LITIRAL,
    INT_LITIRAL,
    FLOAT_LITIRAL,
    ARROW,
    ASSIGN,
    EQL,
    GT,
    LT,
    GT_OR_EQL,
    LT_OR_EQL,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    NONE,
    EOFA,
    ERROR,
};

String ETOKENStrings[];
String ETOKENToString(ETOKEN var);
