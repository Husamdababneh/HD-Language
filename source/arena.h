/* ========================================================================
$File: arena.h
$Date: December 8th 2020 10:49 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once





struct Arena {
	u8* data = nullptr;
	u64 allocated = 0;
	u64 cap = 0;
};




Arena make_arena(u64 size);

template<typename T>
T* allocate(Arena* arena){
	assert(arena->cap > (arena->allocated + sizeof(T)));
	//if (
	arena->allocated += sizeof(T);
	return new(arena->data + arena->allocated) T();
}
