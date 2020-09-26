/* ========================================================================
   $File: String.h
   $Date: 2020-04-23
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Custom String Type
   ========================================================================*/
#pragma once

#include "common.h"

#include <cassert>

struct myString
{
	u64 count;
	u8* data;
	
	u8& operator[]  (u64 i) {
		assert(i <= count);
		return data[i];
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
	
	u8& operator[]  (u64 i) {
		return (*string)[i];
	}		
};

// @Temp(Husam):.
// @IMPORTANT(Husam): Memory ??.
template<typename T>
struct Array
{
	Array(size_t _count): count(_count) {
		if(count != 0)
			data = new T[count];
	}
	size_t count = 0;
	T* data = nullptr;
};

typedef myString String;
typedef myStringView StringView;
