/* ========================================================================
   $File: hd_hashtable.h
   $Date: 2022-04-01
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Hash Table
    ========================================================================*/
#pragma once

#include "hdbase.h"


constexpr int REMOVED_HASH = 1;
constexpr int FIRST_VALID_HASH = 2;

template<typename T>
using Hasher = int (*)(const T&);

template<typename T>
using Allocator = T* (*)(U32 count);

template<typename T>
T* simple_allocate(U32 count)
{
	return new T[count];
}

template<typename T>
int simple_hash(const T& t)
{
	return 0;
}

template<typename T,
		 Hasher<T> hasher = simple_hash<T>,
		 Allocator<T*> atom_allocator = simple_allocate<T*>,
		 Allocator<U32> hash_allocator = simple_allocate<U32> >
struct HashTable
{
	void init(U32 _table_size);
	void allocate();

	void add(T* atom, U32 hash);
	T* find(T* key_item, U32 hash);
	
	void resize();
	
	U32 table_size;
	U32 items;
	U32 mask;
	U32 resize_threshold;
	
	T** atom_array;  // List of pointers to items
	U32* hash_array;
};

template <typename T>
inline int desired_bucket(const HashTable<T> *table, U32 hash)
{
	return hash & table->mask; 
}

template<typename T, Hasher<T> hasher, Allocator<T*> atom_allocator, Allocator<U32> hash_allocator>
void HashTable<T, hasher, atom_allocator, hash_allocator>:: add(T* atom, U32 hash)
{
	if (items >= resize_thresold) resize();

	if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;

	int bucket = desired_bucket(this, hash);

	items++;

	while(1){
		if (hash_array[bucket] == nullptr){
			hash_array[bucket] = hash;
			atom_array[bucket] = atom;
			return
		}
		bucket = (bucket + 1) & mask;
	}
}


template<typename T, Hasher<T> hasher, Allocator<T*> atom_allocator, Allocator<U32> hash_allocator>
void HashTable<T, hasher, atom_allocator, hash_allocator>::init(U32 _table_size)
{
	table_size = _table_size;

	U32 atom_size = table_size * sizeof(T);
	U32 hash_size = table_size * sizeof(U32);

	atom_array = atom_allocator(atom_size);
	hash_array = hash_allocator(hash_size);

	items = 0;

	mask = table_size - 1;
	assert((table_size & mask) == 0);

	constexpr U32 LOAD_FACTOR_PERCENT = 80;
	resize_threshold = (table_size * LOAD_FACTOR_PERCENT) / 100;
	
}












