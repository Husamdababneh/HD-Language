/* ========================================================================
   $File: common.cpp
   $Date: 2020-10-13
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: common.cpp
   ========================================================================*/

#include "common.h"

u64 c_str_size(const char* str){

	u64 a = 0;
	while(*str != '\0') {
		str++;
		a++;
	}
	return a;	
}
