/* ========================================================================
$File: arena.h
$Date: December 8th 2020 10:49 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once


struct Region {
    Region* next;
    size_t  capacity;
    size_t  size;
    u8      buffer[];
};

Region *region_create(size_t capacity);

#define ARENA_DEFAULT_CAPACITY (640 * 1000)

struct Arena {
    Region* first;
    Region* last;
};

void* arena_alloc(Arena *arena, size_t size);
void  arena_clean(Arena *arena);
void  arena_free(Arena *arena);
void  arena_summary(Arena *arena);