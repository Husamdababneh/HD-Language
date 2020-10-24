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
	u64 occupied;
    T* data;

	T* operator[]  (u64 i) {
		assert(i <= occupied);
		return &data[i];
	}
};


extern constexpr int ARRAY_INIT_SIZE = 10;

// template<typename T>
// Array<T> init_array();


// template<typename T>
// void array_resize(const Array<T>* array, const T item );

// template<typename T>
// void array_add(const Array<T>* array, const T item );

template<typename T>
Array<T> init_array() {
	Array<T> array;
	array.size = ARRAY_INIT_SIZE;
	array.occupied = 0;
	array.data = new T[ARRAY_INIT_SIZE];
	return array;
}


template<typename T>
void array_resize(Array<T>* array ) {
	T* newData = new T[array->size * 2];
	memcpy((u8*)newData, (u8*)array->data, array->occupied);
	delete array->data;
	array->data = newData;
}

template<typename T>
void array_add(Array<T>* array, T item ) {
	if (array->occupied == array->size)
		array_resize(array);
	array->data[array->occupied] = item;
	array->occupied++;
}
