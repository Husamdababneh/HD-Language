/*

    This file should only contain things the complier needs in order
    to successfully compiler user-level code. (For example, things for which
    it needs pointers at various stages in the compilation process).

    Until we implement compilation fences, this file cannot contain any varargs
    procedures, because those would need to use the type of Any, but they might
    compile before Any. We could make dependencies on Any every time there's a
    varargs, but that seems like an unnecessary complication.

                             -- jblow, 15 May 2015.

    Addendum: We also can't compile any code here that does string comparisons,
    due to lack of __strings_are_equal, __strings_are_not_equal! Also, we can't
    use type_info() here! Really it is best if there is no imperative code at all.

*/


Optimization_Level :: enum u8 {
    DEBUG   : 0,
    RELEASE : 1
};

Info_Flags :: enum u32 {
    POLYMORPH_MATCH          : 0x1,
    POLYMORPH_DEDUPLICATE    : 0x2,
    AUTO_VALUE_BAKE          : 0x4
};

Build_Options :: struct {
    optimization_level := Optimization_Level.DEBUG;
    emit_line_directives := true;
    runtime_storageless_type_info := false;
    executable_name : string;
    output_path : string;
    info_flags : Info_Flags.strict;
}

build_options : Build_Options;

Type_Info_Tag :: enum u32 {
    INTEGER   : 0,
    FLOAT     : 1,
    BOOL      : 2,
    STRING    : 3,
    POINTER   : 4,
    PROCEDURE : 5,
    VOID      : 6,
    STRUCT    : 7,
    ARRAY     : 8,
    NULL      : 9,
    ANY       : 10,
    ENUM      : 11,

    HIGHEST_VALUE : 12  // @Cleanup @Hack! This is only necessary because we want to declare arrays of [Type_Info_Tag.highest_value + 1] but we can't handle that +1 at compile time yet!
};

Type_Info :: struct {
    type : Type_Info_Tag.strict;
}

Type_Info_Integer :: struct {
    using info : Type_Info;

    size_in_bits : u32;
    signed : bool;
}

Type_Info_Float :: struct {
    using info : Type_Info;

    size_in_bits : u32;
}

// There is no Type_Info_Bool.
// There is no Type_Info_String.

Type_Info_Pointer :: struct {
    using info : Type_Info;

    pointer_to : *Type_Info;
    soa_packing : s32;  // -1 means no SOA. 0 means no size limit. >0 is AOSOA of that chunk size.
}

Type_Info_Procedure :: struct {
    using info : Type_Info;

    argument_types : [] *Type_Info;
    return_types   : [] *Type_Info;
}

// There is no Type_Info_Void.

Type_Info_Struct :: struct {
    using info : Type_Info;
 
    name : string;
    members : [] Type_Info_Struct_Member;
}

Type_Info_Struct_Member :: struct {
    name : string;
    type : *Type_Info;
    offset_in_bytes : s64;
    
    flags : u32;

    notes: [] string;

    XXX_temporary_offset_to_constant_value : s64 = -1;

    // Flag values: 

    CONSTANT :: 0x1;
    IMPORTED :: 0x2;
}

Type_Info_Array :: struct {
    using info : Type_Info;

    element_type : *Type_Info;
    soa_packing  : s32;  // -1 means no SOA. 0 means no size limit. >0 is AOSOA of that chunk size.

    array_type   : u32;
    array_count  : s64;  // -1 unless this array is FIXED.

    // These are the array types:
    FIXED   :: 0;
    STATIC  :: 1;
    DYNAMIC :: 2;
}

// There is no Type_Info_Null.
// There is no Type_Info_Varargs.
// There is no Type_Info_Any.

Type_Info_Enum :: struct {
    using info : Type_Info;
    
    internal_type : *Type_Info;
    struct_produced : *Type_Info_Struct;
}

_type_table : [] *Type_Info;


Any :: struct {
     type : *Type_Info;
     value_pointer : *void;
}



Allocator :: #type (mode: Allocator_Mode.strict, size: s64, old_size: s64, old_memory_pointer: *void, allocator_data: *void, options: s64) -> *void;
Logger    :: #type (message: string, ident: string, mode: Log_Mode.strict, data: *void) -> void;

Allocator_Mode :: enum {  // @Volatile: If you change these, the bytecode has to change!
    ALLOCATE  : 0,
    RESIZE    : 1,
    FREE      : 2,
    FREE_ALL  : 3
}

Log_Mode :: enum {
    NONE     : 0,
    MINIMAL  : 1,
    EVERYDAY : 2,
    VERBOSE  : 3
}

Context :: struct {
    thread_index   : u32;

    user_index     : u32;
    user_data      : *void;

    allocator      : Allocator;
    allocator_data : *void;

    logger         : Logger;
    logger_data    : *void;
}


__strings_are_equal     :: (a: string, b: string) -> bool #foreign;
__strings_are_not_equal :: (a: string, b: string) -> bool #foreign;


c_malloc :: (size : s64) -> *void #foreign "__malloc";
c_free   :: (memory: *u8) #foreign "__free";
memcpy :: (dest: *void, source: *void, count: s64) -> *void #foreign;

__default_allocator :: (mode: Allocator_Mode.strict, size: s64, old_size: s64, old_memory_pointer: *void, allocator_data: *void, options: s64) -> *void {
    using Allocator_Mode.members;

    if mode == ALLOCATE {
        return c_malloc(size);
    } else if mode == RESIZE {
        // For now, we dont even bother with realloc, just allocate, copy, free.
        result := c_malloc(size);
        if !result return null;

        if old_memory_pointer && (old_size > 0) {
            memcpy(result, old_memory_pointer, old_size);
            c_free(old_memory_pointer);
        }

        return result;
    } else if mode == FREE {
        c_free(old_memory_pointer);
        return null;
    } else if mode == FREE_ALL {
        // @Incomplete: Not supported while we are using C malloc...
        // Use Windows heaps or else an off-the-shelf malloc that gives us control.

        // @Incomplete: Panic.
//        assert(false);
    } else {
        // @Incomplete: Panic.
//        assert(false);   Don't want to pull in assert, as that currently does a printf, ugh!
        return null;
    }
}


__default_logger :: (message: string, ident: string, mode: Log_Mode.strict) {};


// Arrays:


Newstring    :: struct {
    count     : s64;  // Signed so that if we do for 0..count-1 it works...
    data      : *u8;
};

Static_Array :: struct {
    count     : s64;  // Signed so that if we do for 0..count-1 it works...
    data      : *u8;
};

Dynamic_Array :: struct {
    count      : s64;  // Signed so that if we do for 0..count-1 it works...
    allocated  : s64;
    data       : *void;

    allocator      : Allocator;  // @Volatile: If we change functions to always take a data element, size needs to change in general.h
    allocator_data : *void;
};


