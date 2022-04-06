/* ========================================================================
$File: base.cpp
$Date: 2020-10-16
$Creator: Husam Dababneh
$Description: TODO
========================================================================*/



// TODO (Husam Dababneh): Remove all "Compiler Specific files"
// TODO (Husam Dababneh): Add all base-layer components here

#define HD_BASE_IMPL
#define HD_APPLICATION_LAYER

#include "hdbase.h"


#include <stdio.h>
#include "hdarena.cpp"

#include "platform/platform.cpp"

// @TEMP: I will create my own
#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#pragma warning( push )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#include <meow_hash\meow_hash.h> 
#pragma warning( pop )


