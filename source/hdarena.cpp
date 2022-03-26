/* ========================================================================
$File   : hdarena.h
$Date   : October 16th 2021 0:00 am
$Creator: Husam Dababneh
$Desc   : TODO(Husam):
=========================================================================*/


#include "hdarena.h"

static inline void 
InitializeMemoryArena(MemoryArena* arena, Size size, PTR base)
{
	arena->size = size;
	arena->base = base;
	arena->used = 0;
}

static inline MemoryArena
InitializeMemoryArena(Size size, PTR base)
{
	MemoryArena arena = {};
	arena.size = size;
	arena.base = base;
	arena.used = 0;
	return arena;
}


static inline PTR 
PushSize(MemoryArena* arena, Size size)
{
	// TODO(Husam) Create custom assert
	assert(arena->used + size <= arena->size);
	
	// Stupid C/C++ -_-
	// (void*)((char*)
	PTR result = (PTR)((char*)arena->base + arena->used);
	arena->used +=size;
	
	return result;
}
