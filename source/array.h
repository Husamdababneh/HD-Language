/* ========================================================================
$File: array.h
$Date: 2020-12-6
$Creator: Husam Dababneh
$Description: Array
========================================================================*/

#ifndef ARRAY_H
#define ARRAY_H


template <typename Type> 
struct Array
{
	Type* data;
	U64   size;
	U64   capacity;
};



template<typename Type> Array<Type> make_array(MemoryArena* arena);





#endif //ARRAY_H
