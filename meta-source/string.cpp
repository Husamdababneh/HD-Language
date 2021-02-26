#define _enum(name, x, semicolon) String name##_Strings[] =  { x } semicolon
#define entry_name(x) #x##_s,
#define entry_name_value(x, value) entry_name(x)
#define string_function(x) String x##_toString(u64 var) { \
if(var < Index ) return  {(u8*) var , 1}; \
return x##_Strings[(u64)var - Index  + 1]; \
}

#include "data.cpp"
#undef _enum
#undef entry_name
#undef string_function
#undef entry_name_value