/* ========================================================================
$File: queue.h
$Date: December 11th 2020 4:39 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once

/*

TODO(Husam) : safty checks here would be nice i think !!.. -_-.

*/


#include "common.h"

template<typename T>
struct Queue {
	T* data;
	u64 cap;
	u64 start;
	u64 end;
	u64 count;
};


template<typename T> 
Queue<T> 
make_queue(u64 size)
{
	Queue<T> result;
	result.data = new T[size];
	result.cap = size;
	result.start = result.end = result.count = 0;
	return result; 
}

template<typename T> 
void
free_queue(Queue<T>* queue)
{
	delete queue->data;
	
}

template<typename T> T*
top_plus(Queue<T>* queue, u64 plus)
{
	//assert(queue->start + plus < queue->end);
	return &queue->data[queue->start + plus];
}


template<typename T> T*
top(Queue<T>* queue)
{
	return &queue->data[queue->start];
}


template<typename T> T*
pop(Queue<T>* queue)
{
	T* result = &queue->data[queue->start];
	queue->start = (queue->start + 1) % queue->cap;
	queue->count--;
	return result;
}

template<typename T> T*
push(Queue<T>* queue, T data)
{
	queue->data[queue->end] = data;
	queue->end = (queue->end + 1) % queue->cap;
	queue->count++;
	return top(queue);
}

