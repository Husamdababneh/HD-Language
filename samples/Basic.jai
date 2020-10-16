#import "Print"

SOA_Pointer :: struct {
    data: * u8;
    index: u32;
    count: u32;
};


__array_unordered_remove :: (array: *void, item: *void, size: s64) -> s32 #foreign;

array_unordered_remove :: inline (array: *[..] $T, item: T) {
    __array_unordered_remove(array, *item, size_of(T));
}

array_find :: (array: [] $T, item: T) -> s64 {
    for array if it == item return it_index; 
    return -1;  // Not found.
}

array_add :: (array: *[..] $T, item: T) { 
    if array.count >= array.allocated {
        reserve := 2 * array.allocated;
        if reserve < 8  reserve = 8;

        array_reserve(array, reserve);
    }

    array.data[array.count] = item;
    array.count += 1;
}

array_reserve :: (array: *[..] $T, reserve: s64) {
    if reserve <= array.allocated return;

    size := size_of(T);
    num_bytes := reserve * size;

    if !array.allocator  array.allocator = __default_allocator;  // @Incomplete: Use context.allocator. @Context

    allocator := array.allocator;

    new_memory := allocator(Allocator_Mode.RESIZE, num_bytes, array.count * size, array.data, array.allocator_data, 0);
    assert(new_memory != null);
    if !new_memory return;  // @Incomplete: array_add should know this failed.

    array.data = new_memory;
    array.allocated = reserve;
}

find_empty_slot :: (array: [] * $T) -> s64 {
    for array 
        if it == null return it_index;

    return -1;
}


add_build_file :: (filename: string) #foreign;
add_build_string :: (data: string) #foreign;
set_build_file_path :: (filename: string) #foreign;
update_build_options :: () #foreign;

// For some reason in Windows sinf and cosf don't have linkage, probably because
// they always get turned into intrinsics? So I made my own..
cosf :: (theta: float) -> float #foreign "__cosf"; 
sinf :: (theta: float) -> float #foreign "__sinf";
sqrtf :: (theta: float) -> float #foreign "__sqrtf";

TAU : float : 6.283147;
TAU64 : float64 : 6.28318530717958647;

//printf :: (s: *u8, args: ..) -> s32 #foreign;
sprintf :: (dest: *u8, s: *u8, args: ..) -> s32 #foreign;
strchr :: (s: *u8, c: u32) -> *u8 #foreign;
sscanf :: (s: *u8, format: *u8, args: ..) -> s32 #foreign;
atoi   :: (s: *u8) -> s32 #foreign;
sqrt   :: (f: float64) -> float64 #foreign "__sqrt_float64";

memcmp :: (a: *void, b: *void, count: s64) -> s32 #foreign;

assert :: (arg: bool) {  // This is a sorry-assed assert.
    if arg return;

    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    print("!!!!!    Assertion failed    !!!!!\n");
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    ExitProcess(0);  // Excuse my win32-ism.
}


alloc :: (size: s64) -> *void {
    return context.allocator(Allocator_Mode.ALLOCATE, size, 0, null, context.allocator_data, 0);
}

free :: (memory: *void) {
    context.allocator(Allocator_Mode.FREE, 0, 0, memory, context.allocator_data, 0);
}

memset :: (dest: *void, value: int, count: s64)    -> *void #foreign;


log :: (message: string, ident: string, mode: Log_Mode.strict = Log_Mode.EVERYDAY) {
    context.logger(message, ident, mode, context.logger_data);
}

get_time :: () -> float64 #foreign;


get_field :: (info: *Type_Info_Struct, name: string) -> *Type_Info_Struct_Member {
    for * info.members {
        if name == it.name return it;
    }

    return null;
}

to_string :: (s: *u8) -> string {
    strlen :: (x: *u8) -> s64 {
        count := 0;
        while << x  { x += 1; count += 1; }

        return count;
    }
    
    // @Temporary

    length := strlen(s);

    result: string;
    result.count = length;
    result.data = cast(*u8) s;
    return result;
}

bit_or  :: (a: u64, b: u64) -> u64 #foreign;
bit_and :: (a: u64, b: u64) -> u64 #foreign;
bit_xor :: (a: u64, b: u64) -> u64 #foreign;
bit_not :: (a: u64)         -> u64 #foreign;

bit_shift_left             :: (a: u64, count: s32) -> u64 #foreign;
bit_shift_right            :: (a: u64, count: s32) -> u64 #foreign;
bit_shift_right_arithmetic :: (a: s64, count: s32) -> s64 #foreign;


XXX_temporary_get_start_of_data_segment :: () -> *u8 #foreign;
running_at_compile_time :: () -> bool #foreign;

bytecode_get_foreign_function_pointer :: (lambda: *void) -> *void #foreign;

to_c_string :: (s: string) -> *u8 {

    result : *u8 = alloc(s.count + 1);
    memcpy(result, s.data, s.count);
    result[s.count] = 0;

    return result;
}

copy_string :: (s: string) -> string {
    t : string;

    if !s.count return t;

    t.count = s.count;
    t.data = alloc(s.count);
    memcpy(t.data, s.data, s.count);

    return t;
}

//
// Windows-specific stuff.
//
// @Incomplete: We need a way to cordon off platform-specific code.
//

STD_INPUT_HANDLE  :: -10;
STD_OUTPUT_HANDLE :: -11;
STD_ERROR_HANDLE  :: -12;

HANDLE :: #type s64;   // This actually needs to be pointer-sized, so s32 in 32-bit, but as you see above, lots of people use integers too, arrrrgh!!

// @Cleanup: We needed these to be explicitly s32 for CreateFileA, but that is a bug...
FILE_SHARE_READ   : s32 : 0x1;
FILE_SHARE_WRITE  : s32 : 0x2;
FILE_SHARE_DELETE : s32 : 0x4;

FILE_CREATE_NEW        : s32 : 1;
FILE_CREATE_ALWAYS     : s32 : 2;
FILE_OPEN_EXISTING     : s32 : 3;
FILE_OPEN_ALWAYS       : s32 : 4;
FILE_TRUNCATE_EXISTING : s32 : 5;

INVALID_HANDLE_VALUE :: -1;

GetStdHandle :: (handle: s32) -> HANDLE #foreign;  // Here because Print needs it.
CloseHandle  :: (handle: HANDLE) -> s32 #foreign;
WriteFile    :: (handle: HANDLE, buffer: *void, buffer_length: s32, written_result: *s32, overlapped: *void) -> s32 #foreign;  // This is here because Print needs it.
GetLastError :: () -> s32 #foreign;
ExitProcess :: (exit_code: u32) -> void #foreign;


sleep_milliseconds :: (milliseconds: s32) {
    Sleep :: (milliseconds: s32) -> void #foreign;

    Sleep(milliseconds);
}

setcwd :: (s: string) -> bool {
    _chdir :: (s: *u8) -> s32 #foreign;

    // Allocate a C string. Yuck!
    
    c_string := to_c_string(s);
    defer free(c_string);

    chdir_result := _chdir(c_string);

    return chdir_result != 0;
}

