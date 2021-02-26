/* ========================================================================
$File: arena.h
$Date: December 8th 2020 10:49 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once

struct Arena {
	u8* data = nullptr;
	u8* cursor = nullptr;
	u64 cap = 0;
};




Arena make_arena(u64 size);


template<typename T>
T* allocate(Arena* arena){
	assert(arena->cap > ((arena->cursor - arena->data) +  sizeof(T)));
	arena->cursor += sizeof(T);
	return ::new(arena->cursor - sizeof(T)) T();
}










