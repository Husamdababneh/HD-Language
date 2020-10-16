/*
  The Pool is a memory allocator that you can use when you want to
  quickly allocate memory blocks of many different sizes, but all
  those blocks will have approximately the same lifetime.
  With a Pool, you allocate and allocate but you never deallocate
  until you are done with everything; at that time, you deallocate
  the entire Pool at once.
 */

POOL_BUCKET_SIZE_DEFAULT :: 65536;

pop :: (array: *[..] $T) -> T {
    if array.count == 0 {    // @Cleanup: Should be able to say array.count
        print("*********** ATTEMPT TO POP AN EMPTY ARRAY!\n");  // We don't have runtime assert yet!
        return null;
    }

    result := (<< array)[array.count-1];
    array.count -= 1;

    return result;
}


Pool :: struct {
    memblock_size: s64 = POOL_BUCKET_SIZE_DEFAULT;
    alignment := 8;

    unused_memblocks    : [..] *u8;
    used_memblocks      : [..] *u8;
    obsoleted_memblocks : [..] *u8;

    current_memblock : *u8;
    current_pos      : *u8;
    bytes_left       : s64;

    block_allocator      : Allocator;
    block_allocator_data : *void;
};

set_allocators :: (pool: *Pool, 
                   block_allocator      : Allocator = null,
                   block_allocator_data : *void     = null,
                   array_allocator      : Allocator = null,
                   array_allocator_data : *void     = null) {

    if !block_allocator {
        block_allocator      = context.allocator;
        block_allocator_data = context.allocator_data;
    }

    if !array_allocator {
        array_allocator      = context.allocator;
        array_allocator_data = context.allocator_data;
    }

    pool.block_allocator      = block_allocator;
    pool.block_allocator_data = block_allocator_data;

    pool.   unused_memblocks.allocator = array_allocator;
    pool.     used_memblocks.allocator = array_allocator;
    pool.obsoleted_memblocks.allocator = array_allocator;

    pool.   unused_memblocks.allocator_data = array_allocator_data;
    pool.     used_memblocks.allocator_data = array_allocator_data;
    pool.obsoleted_memblocks.allocator_data = array_allocator_data;
}


// These are the things you may want to call:

get :: (using pool: *Pool, nbytes: s64) -> *void {
    extra := alignment - (nbytes % alignment); 
    nbytes += extra; 

    if bytes_left < nbytes ensure_memory_exists(pool, nbytes); 

    retval := current_pos;
    current_pos += nbytes; 
    bytes_left  -= nbytes; 
 
    return retval; 
}

release :: (using pool: *Pool) {
    reset(pool);

    for pool.unused_memblocks free(it);

    print("Releasing pool.\n");
}


reset :: (using pool: *Pool) {
    if current_memblock {
        array_add(*unused_memblocks, current_memblock);
	    current_memblock = null;
    }

    for used_memblocks array_add(*unused_memblocks, it);
    used_memblocks.count = 0;

    for obsoleted_memblocks free(it);
    obsoleted_memblocks.count = 0;

    cycle_new_block(pool);
}


//
// The stuff below is 'private':
//


ensure_memory_exists :: (using pool: *Pool, nbytes: s64) {
    bs := memblock_size;

    while bs < nbytes bs *= 2;

    if bs > memblock_size resize_blocks(pool, bs);
    cycle_new_block(pool);
}

resize_blocks :: (using pool: *Pool, block_size: s64) {
    memblock_size = block_size;

    if current_memblock array_add(*obsoleted_memblocks, current_memblock);
    
    for used_memblocks array_add(*obsoleted_memblocks, it);
    used_memblocks.count = 0;

    current_memblock = null;
}

cycle_new_block :: (using pool: *Pool) {
    if current_memblock {
        array_add(*used_memblocks, current_memblock);
    }

    new_block : *u8;
    if (unused_memblocks.count) {
        new_block = pop(*unused_memblocks);
    } else {
        assert(pool.block_allocator != null);
        new_block = pool.block_allocator(Allocator_Mode.ALLOCATE, memblock_size, 0, null, pool.block_allocator_data, 0);
    }

    bytes_left = memblock_size;
    current_pos = new_block;
    current_memblock = new_block;
}

