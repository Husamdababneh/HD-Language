/* ========================================================================
$File: arena.h
$Date: December 8th 2020 10:49 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif

struct Region {
    Region* next;
    size_t  capacity;
    size_t  size;
    u8      buffer[];
};
#ifdef _MSC_VER
#pragma warning(default: 4200)
#endif


Region *region_create(size_t capacity);

#define ARENA_DEFAULT_CAPACITY (500 * 1024) // 0.5 MB

struct Arena {
    Region* first;
    Region* last;
};

void* arena_alloc(Arena *arena, size_t size);
void  arena_clean(Arena *arena);
void  arena_free(Arena *arena);
void  arena_summary(Arena *arena);