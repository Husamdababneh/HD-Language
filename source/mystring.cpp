/* ========================================================================
   $File: String.cpp
   $Date: 2020-04-23
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Custom String Type
   ========================================================================*/

#include "pch.h"
#include "string.h"

typedef myString String;
typedef myStringView StringView;

bool isEqual(String* first, String* second)  {
	if (first->count != second->count) return false;
	
	for(u32 a = 0; a < first->count; a++){
		if((*first)[a] != (*second)[a])
			return false;
	}
	return true;
}


myStringView make_view(String& str)
{
	return { &str, 0 };
}

myString operator "" _s(const char* a, size_t s){
    return { (u8*)a, s};
}
