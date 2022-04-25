/* ========================================================================
   $File: main.h
   $Date: 2020-04-03
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


#define HD_BASE_IMPL
#define HD_APPLICATION_LAYER

#include "hd/hdbase.h"
#include "hd/hdarena.cpp"
#include "hd/hd_hashtable.h"

#include "hd/platform.cpp"

// @TEMP: I will create my own
#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#pragma warning( push )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
//#include <vendor\meow_hash.h> 
#pragma warning( pop )





