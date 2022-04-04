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

#include "defines.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>


#include "platform/platform.h"
#include "arena.h"
#include "vendor/stb_ds.h"  // Do i need this or i should create my own ?? 

#pragma warning( push )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4324 )
#include <meow_hash\meow_hash.h> 
#pragma warning( pop )

#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#include "hdarena.cpp"
#define introspect(x)
#define padding_check

// TODO: Remove this ?? 

