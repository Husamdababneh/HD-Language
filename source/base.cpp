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

#define introspect(x)
#define padding_check
#include "hdbase.h"
#include "base.h"

//#include "arena.cpp"
#include "logger.cpp"
#include "auxiliary.cpp"
//#include "Ast.cpp"

// TODO: Remove this ?? 
#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"


#include "hdarena.cpp"

//#include "array.cpp"