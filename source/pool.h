/* ========================================================================
   $File: pool.h
   $Date: 2020-10-22
   $Creator: Husam Dababneh
   $Description: pool
    ========================================================================*/
#pragma once

#include "common.h"

constexpr static U64 MAX_POOL_SIZE = 100;

template<typename T>
struct Pool {
	T data[MAX_POOL_SIZE];
};

