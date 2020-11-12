/* ========================================================================
   $File: String.h
   $Date: 2020-04-23
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Custom String Type
   ========================================================================*/
#pragma once

#include "common.h"
#include <assert.h>
// @Cleanup: do we need this ?

struct myString
{
	u8* data;
	u64 count;

	u8& operator[]  (u64 i) {
		assert(i <= count);
		return data[i];
	}
	
	u8& operator[] (u64 i) const  {
		assert(i <= count);
		return data[i];
	}

	// @Incomplete:
	bool compare(const myString& str){
		if (this->count != str.count) return false;

		for(u32 a = 0; a < this->count; a++){
			if(str[a] != data[a])
				return false;
		}
		
		return true;
	}

	bool isEqual(const myString& str){
		if (this->count != str.count) return false;

		for(u32 a = 0; a < this->count; a++){
			if(str[a] != data[a])
				return false;
		}
		
		return true;
	}
};



struct myStringView
{
	myString* string;
	u64 cursor;

	operator u64() const
	{
		return cursor;
	}
	
	u8 operator[]  (u64 i) {
		return (*string)[i];
	}
	

};

myString operator "" _s(const char* a, size_t s);

typedef myString String;
typedef myStringView StringView;

bool isEqual(String* first, String* second);
myStringView make_view(const myString& str);
