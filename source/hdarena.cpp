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
#if 0
	printf("Arena Size = %lld\nRequested Size = %lld\nUsed = %lld\nFlag = %s\n",
		   arena->size,
		   size,
		   arena->used,
		   (arena->size - size >= arena->used) ? "true" : "false");
#endif	
	assert(arena->size - size >= arena->used);
	
	// Stupid C/C++ -_-
	// (void*)((char*)
	PTR result = (PTR)((char*)arena->base + arena->used);
	arena->used +=size;
	
	return result;
}

