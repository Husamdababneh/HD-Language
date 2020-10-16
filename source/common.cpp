/* ========================================================================
   $File: common.cpp
   $Date: 2020-10-13
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: common.cpp
   ========================================================================*/


size_t c_str_size(const char* str){

	size_t a = 0;
	while(*str != '\0') {
		str++;
		a++;
	}
	return a;	
}
