#define generate_enum(name, semicolon, ...) enum /* name */ { __VA_ARGS__ } semicolon
#define generate_function_decls(name, x, semicolon) enum /* name */ { x } semicolon
#define _enum(name, semicolon, x) generate_enum(name,semicolon,x) \
extern String name##_Strings[] semicolon \
extern String name##_toString(u64 var) semicolon 
#define entry_name(x) TOKEN_##x,
#define entry_name_value(x, value) TOKEN_##x = value,
#define string_function(x)
#include "data.cpp"
#undef _enum
#undef entry_name
#undef string_function
#undef entry_name_value

