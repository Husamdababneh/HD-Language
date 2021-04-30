/* ========================================================================
$File: String.h
$Date: 2020-04-23
$Creator: Husam Dababneh
$Description: Custom String Type
========================================================================*/
#pragma once

#include "common.h"
#include <assert.h>
// @Cleanup: do we need this ?

struct myString
{
	union {
		u8* data;
		u8* str;
	};
	u64 count;
	
	u8& operator[]  (u64 i) {
		assert(i <= count);
		return data[i];
	}
	
	u8& operator[] (u64 i) const  {
		assert(i <= count);
		return data[i];
	}
};


myString operator "" _s(const char* a, size_t s);

typedef myString StringView;

#define cmpsv(x, y) memcmp(x, y, strlen(x))
#define cmp2sv(x, y) memcmp(x.data, y.data, x.count)
#define SV_PRINT(x) (int)x.count, x.data





