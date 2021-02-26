/* ========================================================================
   $File: lex.cpp
   $Date: February 18th 2021 3:31 pm
   $Creator: Husam Dababneh
   $Description: Defines Lexer functionality & Data Structures
   ========================================================================*/ 

#ifndef STACK_H
#define STACK_H

template<typename T>
struct Stack 
{
	T*  data;
	u64 cap;
	u64 count;
	
	// TODO(Husam) : Add methods here ?? 
	// constructors ?? maybe no
	// destructor ??  maybe yes 
	
	void push(T _data) {
		if(count < cap){
			data[count] = _data;
			count += 1;
			return;
		}
		
		// TODO: Add resize logic ?? 
		assert(false);
		
	}
	
	T pop() {
		if(count == 0) return 0;
		T result = top();
		count -= 1;
		return result;
	}
	
	
	T top() {
		if(count == 0) return 0;
		return data[count - 1];
	}
};

template<typename T>
Stack<T>
make_stack(u64 size)
{
	Stack<T> result;
	result.data = new T[size];
	result.cap = size;
	result.count = 0;
	return result;
}

template<typename T>
void
push(Stack<T>* stack, T data)
{
	stack->push(data);
}


template<typename T>
T
pop(Stack<T>* stack)
{
	return stack->pop();
}

template<typename T>
T
top(Stack<T>* stack)
{
	return stack->top();
}

#endif //STACK_H
