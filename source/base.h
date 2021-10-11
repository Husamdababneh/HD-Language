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
//#include "hdbase.h"
#include "platform/platform.h"
#include "logger.h"        // is this really needed?? 
#include "common.h"
#include "stack.h"         // is this really needed?? 
#include "queue.h"         // is this really needed?? 
#include "arena.h"
#include "vendor/stb_ds.h"  // Do i need this or i should create my own ?? 
#include <meow_hash\meow_hash.h> 



#endif //BASE_H
