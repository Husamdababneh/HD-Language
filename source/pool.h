/* ========================================================================
   $File: pool.h
   $Date: 2020-10-22
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: pool
    ========================================================================*/
#pragma once



constexpr static MAX_POOL_SIZE = 100;

template<typename T>
struct Pool {
	T data[MAX_POOL_SIZE];
	
	
	
};




int func () {
	Pool<int> myPool;

	myPool.allocate
}
