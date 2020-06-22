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
		assert(i < count);
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

typedef myString String;
typedef myStringView StringView;
