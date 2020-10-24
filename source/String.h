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

#include <string.h>

// @Cleanup: do we need this ?

struct myString
{
	static int counter;

	// -1??
	// myString() : data(nullptr), count(0) {	
    // }

    // myString(u8* d, u64 size): data(d), count(size) {
    // }
	
	u8* data;
	u64 count;

	u8& operator[]  (u64 i) {
		assert(i <= count);
		return data[i];
	}

};



struct myStringView
{
	myString string;
	u64 cursor;

	operator u64() const
	{
		return cursor;
	}
	
	u8& operator[]  (u64 i) {
		return string[i];
	}
	

};

myString operator "" _s(const char* a, size_t s);

typedef myString String;
typedef myStringView StringView;

bool isEqual(String& first, String& second);
myStringView make_view(const myString& str);
