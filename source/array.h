/* ========================================================================
   $File: array.h
   $Date: 2020-10-14
   $Creator: Husam Dababneh
   $Description: array
   ========================================================================*/
#pragma once


#include "common.h"
#include <stdio.h>

template<typename T, u64 a>
struct Stack_Array  {
    u64 count = a;
    T data[a];
};


template<typename T>
struct Array {
	u64 count = 0;
	u64 occupied = 0 ;
	bool init = false;
	T* data = nullptr;
	
	T operator[]  (u64 i) {
		//assert(i <= occupied);
		return data[i];
	}
};


extern u8 ARRAY_INIT_SIZE;

template<typename T>
Array<T> init_array() {
	Array<T> array;
	array.count = ARRAY_INIT_SIZE;
	array.occupied = 0;
	array.data = new T[ARRAY_INIT_SIZE];
	array.init = true;
	return array;
}

template<typename T>
Array<T> init_array(u64 count) {
	Array<T> array;
	array.count = count;
	array.occupied = 0;
	array.data = new T[count];
	array.init = true;
	return array;
}


template<typename T>
void array_resize(Array<T>* array ) {
	if (array->init == true) {
		T* newData = new T[array->count * 2];
		array->count = array->count *2;
		memset((void*)newData, 0, array->count * 2);
		memcpy((void *)newData, (const void*)array->data, array->occupied * sizeof(T));
		delete array->data;
		array->data = newData;
		return;
	}
	
	*array = init_array<T>();
}

template<typename T>
void array_add(Array<T>* array, T item ) {
	if(!array->init) array_resize(array);
	if (array->occupied == array->count)
		array_resize(array);
	array->data[array->occupied] = item;
	array->occupied++;
}


template<typename T>
void array_free(Array<T>* array ) {
	delete array->data;
}
