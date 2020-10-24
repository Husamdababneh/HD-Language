/* ========================================================================
   $File: array.h
   $Date: 2020-10-14
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: array
    ========================================================================*/
#pragma once

#include "common.h"

template<typename T, u64 a>
struct Stack_Array  {
    u64 size = a;
    T data[a];
};


template<typename T>
struct Array {
    u64 size;
    T* data;
};

template<typename T>
Array<T> make_heap_array(u64 size);


//Array make_array;
