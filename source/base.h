/* date = May 18th 2021 0:58 am */

#ifndef BASE_H
#define BASE_H

#include "defines.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
// TODO: Add Platform macros


#include "platform/platform.h"
#include "string_view.h"
#include "logger.h"
#include "common.h"
#include "stack.h"
#include "queue.h"
#include "arena.h"
#include "vendor/stb_ds.h"
#include <meow_hash\meow_hash.h>


#define Note(...) 
#define Todo(x)
#define Incomplete(x)

#define BYTE 1
#define KB(x) (u64)(x * 1024)
#define MB(x) (u64)(KB(x) * 1024)
#define GB(x) (u64)(MB(x) * 1024)
#endif //BASE_H
