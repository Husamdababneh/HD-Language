/* ========================================================================
$File: arena.h
$Date: December 8th 2020 10:49 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once

#include <typeinfo> // @NOCHECKIN
struct Arena {
	char* data = nullptr;
	char* cursor = nullptr;
	u64 cap = 0;
};




Arena make_arena(u64 size);

inline u64 next_power_of_two(u64 v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	v++;
	
	return v;
}


template<typename T>
T* allocate(Arena* arena){
	assert(arena->cap > ((arena->cursor - arena->data) +  sizeof(T)));
	//printf("sizeof(%s) = %zd : %zd\n", typeid(T).name(), sizeof(T), next_power_of_two(sizeof(T)));
	void * result = arena->cursor;
	arena->cursor += sizeof(T);//72;//next_power_of_two(sizeof(T));
	
	return new(result ) T();
}










