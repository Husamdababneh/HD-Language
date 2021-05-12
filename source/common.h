/* ========================================================================
   $File: common.h
   $Date: 2020-04-17
   $Creator: Husam Dababneh
   $Description: Common Definitions!! 
   ========================================================================*/
#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double  f64;

typedef float  float32;
typedef double float64;



#define STBARRFOR(x,y) for (u64 it = 0; it < arrlenu(x); it++) { auto& it_data = x[it]; y} 