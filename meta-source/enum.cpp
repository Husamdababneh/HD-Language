#define generate_enum(name, x, semicolon) enum /* name */ { x } semicolon
#define generate_function_decls(name, x, semicolon) enum /* name */ { x } semicolon
#define _enum(name, x, semicolon) generate_enum(name,x,semicolon) \
String name##_Strings[] semicolon \
String name##_toString[] semicolon 
#define entry_name(x) TOKEN_##x,
#define entry_name_value(x, value) TOKEN_##x = value,
#define string_function(x)
#include "data.cpp"
#undef _enum
#undef entry_name
#undef string_function
#undef entry_name_value

