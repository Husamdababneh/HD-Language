/* ========================================================================
$File: arena.cpp
$Date: December 9th 2020 1:21 am 
$Creator: Husam 
$Desc:  
=========================================================================*/


#include "arena.h"

Region *region_create(u64 capacity)
{
	const u64 region_size = sizeof(Region) + capacity;
	Region *region = (Region*)new u8[region_size];
    memset(region, 0, region_size);
    region->capacity = capacity;
    return region;
}

void *arena_alloc(Arena *arena, u64 size)
{
    if (arena->last == NULL) {
        assert(arena->first == NULL);
		
        Region *region = region_create(size > ARENA_DEFAULT_CAPACITY ? size : ARENA_DEFAULT_CAPACITY);
		
        arena->last = region;
        arena->first = region;
    }
	
    while (arena->last->size + size > arena->last->capacity &&
		   arena->last->next) {
        arena->last = arena->last->next;
    }
	
    if (arena->last->size + size > arena->last->capacity) {
        Region *region = region_create(size > ARENA_DEFAULT_CAPACITY ? size : ARENA_DEFAULT_CAPACITY);
		
        arena->last->next = region;
        arena->last = region;
    }
	
    void *result = arena->last->buffer + arena->last->size;
    memset(result, 0, size);
    arena->last->size += size;
    return result;
}

void arena_clean(Arena *arena)
{
    for (Region *iter = arena->first;
		 iter != NULL;
		 iter = iter->next) {
        iter->size = 0;
    }
	
    arena->last = arena->first;
}

void arena_free(Arena *arena)
{
    Region *iter = arena->first;
    while (iter != NULL) {
        Region *next = iter->next;
        delete iter;
        iter = next;
    }
    arena->first = NULL;
    arena->last = NULL;
}

void arena_summary(Arena *arena)
{
    if (arena->first == NULL) {
        printf("[empty]");
    }
	
    for (Region *iter = arena->first;
		 iter != NULL;
		 iter = iter->next) {
        printf("[%zu/%zu] -> ", iter->size, iter->capacity);
    }
    printf("\n");
}