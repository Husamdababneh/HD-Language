/*

  Debug bad things that happen by writing through context pointer.

*/

#import "Basic";
#import "Pool";
#import "File";
#import "Thread";
#import "Print";

newline :: () {
    print("\n");
}

fprint :: (file: *File, format_string: string, args: ..) {
    buffer: String_Buffer;

    print_to_buffer(*buffer, format_string, ..args);

    total: s64;
    while total < buffer.occupied {
        to_write : s32 = xx (buffer.occupied - total);
        written  : s32;
        status := WriteFile(file.handle, buffer.data.data + written, xx to_write, *written, null);  // @Cleanup the xx: 32 vs 64-bit. fprint won't work on big buffers yet!

        if !status return;  // Make an error code of some kind!

        total += written;
    }
}


pool_allocator :: (mode: Allocator_Mode.strict, size: s64, 
                   old_size: s64, old_memory_pointer: *void, 
                   allocator_data: *void, options: s64) -> *void {

    using Allocator_Mode.members;

    pool := cast(*Pool) allocator_data;
    assert(pool != null);

    // print("In pool_allocator, mode %, size %, data %\n", mode, size, allocator_data);

    if mode == ALLOCATE {
        return get(pool, size);
    } else if mode == RESIZE {
        // Pools don't expand allocated things. Just get new memory and copy.
        // If we want to get sophisticated we can let the Pool expand the last
        // entry in any bucket, but it is unclear how useful this is.

        result := get(pool, size);
        if !result return null;

        if old_memory_pointer && (old_size > 0) {
            memcpy(result, old_memory_pointer, old_size);
        }

        return result;
    } else if mode == FREE {
        // Pools don't free individual elements!
        return null;
    } else if mode == FREE_ALL {
        reset(pool);
        return null;
    } else {
        assert(false);  // We should not get here!
        return null;
    }
}

debug_clear :: (pool: *Pool) {
    memset(pool.current_memblock, 0xdd, pool.memblock_size);
}

array_allocators :: () {

    a: [..] u8;
    b: [..] u8;

    // a will still use the global heap.

    pool: Pool;
    set_allocators(*pool);
    defer release(*pool);

    b.allocator = pool_allocator;
    b.allocator_data = *pool;

    N :: 20;
    for 1..N {
        array_add(*a, cast(u8) it);
        array_add(*b, cast(u8) it);
    }

    print("pool.current_pos is % bytes into its memblock.\n", pool.current_pos - pool.current_memblock);
    print("pool.bytes_left = %\n", pool.bytes_left);


    print_byte_array :: (lead: string, array : [] u8) {
        print("% [%] ", lead, array.count);

        for array print("% ", formatInt(it, base = 16, minimum_digits = 2));

        newline();
    }

    newline();

    print_byte_array("a is: ", a);
    print_byte_array("b is: ", b);

    newline();

    debug_clear(*pool);

    print_byte_array("a is: ", a);
    print_byte_array("b is: ", b);
}

map_test :: () {

    newline();


    /*

     An experiment in a more Lisp-y style.

     This time, we don't have to pass any allocators around.
     It's all carried by the context pointer.

    */


    N :: 5;

    a: [N] int;
    for 0..N-1 a[it] = it+1;

    for a print("a[%] = %\n", it_index, it);

    newline();


    // map: Apply f to each element of array, returning the resulting array.

    map :: (array: [] $T, f: (T) -> $R) -> [] R {

        result : [] R;
        result.data = alloc(array.count * size_of(R));
        result.count = array.count;

        for array result[it_index] = f(it);

        return result;
    }

    square :: (n: int) -> int { return n*n; }

    b := map(a, square);
    for b print("b[%] = %\n", it_index, it);


    newline();

    stringify :: (n: int) -> string { return sprint("The Number %", n); }

    c := map(b, stringify);
    for c print("c[%] = \"%\"\n", it_index, it);

    newline();

    reverse :: (s: string) -> string {   // Dumb casts, sorry!
        count := s.count;

        t: string;
        t.count = count;
        t.data  = alloc(count);

        for 0..count-1 t[count-1-it] = s[it];

        return t;
    }

    d := map(c, reverse);
    for d print("d[%] = \"%\"\n", it_index, it);

/*
    // Before, we worried about memory leaks.
    // Now, we'll just let the caller toss out the entire allocator.

    free(b.data);

    for c free(it);
    free(c.data);

    for d free(it);
    free(d.data);
*/
}


concat :: (a: string, b: string) -> string {
    result : string;

    result.count = a.count + b.count;
    result.data  = alloc(result.count);

    memcpy(result.data, a.data, a.count);
    memcpy(result.data + a.count, b.data, b.count);

    return result;
}


Debug_Hook :: #type (value: u32) -> void;

debug_log :: (value: u32) {
    print("I am logging: %!\n", value);
}



do_some_other_work :: () {
    print("I am doing some other work.\n");
    
    // You might have shoved this into global variables, but that
    // doesn't work particularly well when threading... ... ...

    if context.user_data {
        hook := cast(Debug_Hook) context.user_data;
        hook(context.user_index);
    }
}

do_some_work :: () {
    print("I am doing some work.\n");
    do_some_other_work();
}

context_allocator :: () {
    pool: Pool;
    pool.alignment = 1;

    set_allocators(*pool);
    defer release(*pool);

    a := "Hello";
    b := "Sailor!";

    c1 := concat(a, b);

    newline();
    print("c1 is %\n", c1);
    print("Pool has used % bytes.\n", pool.current_pos - pool.current_memblock);

    new_context := context;
    new_context.allocator      = pool_allocator;
    new_context.allocator_data = *pool;

    c2 : string;
    push_context new_context {
        c2 = concat(a, b);
    }

    newline();
    print("c2 is %\n", c2);
    print("Pool has used % bytes.\n", pool.current_pos - pool.current_memblock);

    newline();


    new_context.user_index = 13539;
    new_context.user_data  = cast(*void) debug_log;

    push_context new_context {
        do_some_work();
    }
}

pool_map :: () {
    pool: Pool;
    set_allocators(*pool);
    defer release(*pool);

    new_context := context;
    new_context.allocator      = pool_allocator;
    new_context.allocator_data = *pool;

    push_context new_context {
        map_test();
    }

    newline();

    print("Pool has used % bytes.\n", pool.current_pos - pool.current_memblock);

    newline();

    // When we exit, the Pool is gone.
    // No fragmentation, no muss, no fuss.
}


MAX_THREADS :: 6;
prefix_table : [MAX_THREADS] string;


do_one_iteration :: (iteration: int) {
    thread_index := context.thread_index;

    prefix: string;
    if thread_index >= MAX_THREADS {
        prefix = "UNKNOWN";
    } else {
        prefix = prefix_table[thread_index];
    }

    print("%: iteration %\n", prefix, iteration);
    sleep_milliseconds(1);
}

worker_proc :: (thread: *Thread) -> s64 {
    logprint(MyIdent, Log_Mode.MINIMAL, "thread % entered worker_proc.", thread.index);

    for 1..10 {
        logprint(MyIdent, Log_Mode.VERBOSE, "thread % is on iteration %.", thread.index, it);
        do_one_iteration(it);
    }

    return 0;
}


MyIdent :: "threadsystem";


thread_action :: () {
    prefix_table[0] = "Main Thread";
    prefix_table[1] = "West";
    prefix_table[2] = "North";
    prefix_table[3] = "East";
    prefix_table[4] = "South";
    prefix_table[5] = "Purple";

    threads : [..] *Thread;

    log("Ready to start threads.", MyIdent, Log_Mode.EVERYDAY);

    for 1..MAX_THREADS-1 {
        thread := thread_create(worker_proc);
        if thread {
            thread.starting_context = context;
            thread.use_this_starting_context = true;

            array_add(*threads, thread);
            thread_start(thread);
        }
    }

    // Wait on all threads to complete.

    while threads.count {
        for threads {
            if thread_is_done(it) {
                logprint(MyIdent, Log_Mode.EVERYDAY, "Thread % is done; removing.", it.index);

                remove it;
                thread_destroy(it);
            }
        }

        sleep_milliseconds(5);
    }

    thread_assert_all_are_stopped();

    print("Done!\n");
}


/*

  Here is preexisting code from demo_type_info.
  It serializes and unserializes stuff.

*/

// 'serialize' is a little like our 'print_any' from earlier,
// but now we are printing the value of each field in a struct,
// using the offsets in the type info.

// We could straightforwardly make this recursive, printing sub-structs,
// arrays, etc, but for demo purposes I am keeping it simple.

serialize :: (a: Any) -> string {
    using Type_Info_Tag.members;

    if (a.type.type != STRUCT) {
        print("This routine only serializes structs.\n");
        result: string;
        return result;
    }

    info_struct := cast(*Type_Info_Struct) a.type;
    
    result : string;

    for info_struct.members {
        item_pointer := cast(*u8) a.value_pointer + it.offset_in_bytes;

        // if has_note(it, "NoSerialize") continue;

        s : string;

        if it.type.type == INTEGER       s = sprint("%1: %2\n",      it.name, << cast(*int)   item_pointer);
        else if it.type.type == FLOAT    s = sprint("%1: %2\n",      it.name, << cast(*float) item_pointer);
        else if it.type.type == BOOL     s = sprint("%1: %2\n",      it.name, << cast(*bool)  item_pointer);
        else if it.type.type == STRING {
            // Also write out the number of bytes, useful when reading the string back.
            // If a null string pointer , just write out a .

            value := << cast(*string) item_pointer;
            if value s = sprint("%1: %2 %3\n", it.name, value.count, value);
            else s = sprint("%1: .\n", it.name);
        }

        if !s continue;  // Unknown field or one we don't want to serialize.

        if result {
            // Concatenate to result. Very lazy, very slow!
            new_result := sprint("%1%2", result, s);

            free(result.data);
            result = new_result;
        } else {
            // Just put the memory in result.
            result = s;
        }
    }

    return result;
}


    

make_substring :: (input: string, cursor: s64) -> string {
    substring := input;
    substring.data += cursor;
    substring.count -= cursor;

    return substring;
}

unserialize :: (input : string, a: Any) {
    find_character :: (s: string, cursor: int, target: int) -> int {
        while cursor < s.count {
            if s[cursor] == target return cursor;
            cursor += 1;
        }

        return -1;
    }



    using Type_Info_Tag.members;

    if (a.type.type != STRUCT) {
        print("This routine only unserializes structs.\n");
        return;
    }

    info_struct := cast(*Type_Info_Struct) a.type;


    cursor := 0;
    while cursor < input.count {
        // Isolate the next field name.
        colon_position := find_character(input, cursor, #char ":");
        if colon_position == -1 break;

        name : string;
        name.data  = input.data + cursor;
        name.count = colon_position - cursor;

        // print("NAME is %\n", name);
        cursor = colon_position + 2;

        // Find the appropriate field.
        for info_struct.members {
            if it.name != name continue;

            // We matched the name, now parse out the value depending on type.

            pointer := (cast(*u8) a.value_pointer) + it.offset_in_bytes;

            if it.type.type == INTEGER  {
                post, value := parse_number(input, cursor);
                cursor = post;
                (<< cast(*int) pointer) = value;
            } else if it.type.type == FLOAT {
                format := to_c_string("%f");
                defer free(format);

                scan_point := make_substring(input, cursor);
                scan_c     := to_c_string(scan_point);
                defer free(scan_c);

                sscanf(scan_c, format, pointer);
            } else if it.type.type == BOOL {
                value: bool = false;

                if input[cursor] == #char "t" {
                    value = true;
                } else {
                    assert(input[cursor] == #char "f");
                }

                << cast(*bool) pointer = value;
            } else if it.type.type == STRING {
                // Very bad code that is not thinking about security, file corruption, etc:

                // Skip leading space.
                // while (cursor < input.count) && (input[cursor] == #char " ")  cursor += 1;
                
                // Get the string length.
                post, length := parse_number(input, cursor);
                cursor = post;
                cursor += 1;  // We know there is one space here because we printed it. (Do not do this in production code!)

                s : string;
                s.count = length;
                s.data = alloc(length);
                memcpy(s.data, input.data + cursor, length);

                << (cast(*string) pointer) = s;
            }

            break;
        }

        // Advance past the next newline.
        next_line_position := find_character(input, cursor, #char "\n");
        if next_line_position == -1 {
            break;
        } else {
            cursor = next_line_position + 1;
        }
    }
}

serialize_simple :: () {
    Font_Settings :: struct {
        font_name     : string = "Inconsolata";
        point_size    : int = 20;
        weight        : string = "bold";
        is_italic     := false;
        is_underlined := false;
    }

    font_settings_a : Font_Settings;
    font_settings_b : Font_Settings;

    font_settings_b.font_name = "Anonymous Pro";
    font_settings_b.is_italic = true;
    font_settings_b.point_size *= 2;

    print("\n-- Serialize --\n\n");

    s : string;
    s = serialize(font_settings_a);

    print("Font a:\n\n%1\n\n", s);

    free(s.data);

    s = serialize(font_settings_b);

    print("Font b:\n\n%1\n\n", s);


    font_settings_c : Font_Settings;

    print("Before:\n");
    print("Font c's name is %1\n", font_settings_c.font_name);
    print("Font c's point size is %1\n", font_settings_c.point_size);

    unserialize(s, font_settings_c);
    free(s.data);

    print("\n");

    print("After:\n");
    print("Font c's name is %1\n", font_settings_c.font_name);
    print("Font c's point size is %1\n", font_settings_c.point_size);

    // Philosophical issue: Note that we have the equivalent of a writable reference
    // in unserialize(s, font_settings_c). Not sure I like that, but it is not nearly
    // as free-fire as & in C++.


    // Let's use all that code again, on a completely unrelated type!

    Physics_Tweaks :: struct {
        gravity := -9.8;
        friction := 0.29;
        timestep := 1.0 / 60.0;
    }

    tweaks : Physics_Tweaks;
    s2 := serialize(tweaks);
    defer free(s2.data);

    print("\n\n");
    print("Also, a different struct using the same serialize function:\n%1\n", s2);

    // Let's change all the 9s in the string to 5s.
    for 0..s2.count-1 {
        if s2[it] == #char "9" then s2[it] = #char "5";
    }

    unserialize(s2, tweaks);

    print("Now, gravity is: %1\n", tweaks.gravity);
    print("Now, friction is: %1\n", tweaks.friction);
}


/*

  End preexisting code.

*/

use_preexisting_code :: (thread: *Thread = null) -> s64 {
    // 
    // The procedure serialize_simple() is from demo_type_info.
    // We are going to call it now, but replace its allocator.
    // The author of this serialize code by definition never thought
    // about allocators, because swappable allocators weren't available!
    //


    // Set up the Pool.

    pool: Pool;
    set_allocators(*pool);
    defer release(*pool);

    new_context := context;
    new_context.allocator      = pool_allocator;
    new_context.allocator_data = *pool;

    push_context new_context {
        // Do the work.
        serialize_simple();
    }

    print("Pool has used % bytes.\n", pool.current_pos - pool.current_memblock);

    return 0;
}

use_preexisting_code_2 :: () {
    threads : [..] *Thread;

    for 1..100 {
        thread := thread_create(use_preexisting_code);
        if thread {
            array_add(*threads, thread);
            thread_start(thread);
        }
    }

    // Wait on all threads to complete.

    while threads.count {
        for threads {
            if thread_is_done(it) {
                remove it;
                thread_destroy(it);
            }
        }

        sleep_milliseconds(5);
    }

    thread_assert_all_are_stopped();

    print("Done!\n");
}


logging :: () {
    toupper :: inline (c: u8) -> u8 {
        if c < #char "a" return c;
        if c > #char "z" return c;
        
        return c + (#char "A" - #char "a");
    }

    my_logger :: (message: string, ident: string, mode: Log_Mode.strict, logger_data: *void) {

        // if mode == Log_Mode.VERBOSE return;  // We don't want verbose messages right now.

        s := copy_string(message);

        for i : 0..s.count-1 {
            s[i] = toupper(s[i]);
        }


        print(">>>>>>>> %\n", s);

        file := cast(*File) logger_data;
        fprint(file, ">>>>>>>> %\n", s);
    }

    log_file := file_open("log.txt", true);
    defer file_close(*log_file);

    new_context := context;
    new_context.logger      = my_logger;
    new_context.logger_data = *log_file;

    push_context new_context {
        thread_action();
    }

    // Rather than just printing it out or logging to a file,
    // in a graphical program you can put logs into a pull-down
    // console, etc.

}


main :: () {
    // Note on new syntax!

    array_allocators();

    context_allocator();

    // Note that arrays use the context allocator!
    // (Take a look at array_add).
 

    pool_map();

    thread_action();

    use_preexisting_code();

    use_preexisting_code_2();

    // Discussion of AI Strategy Prototype use case.

    // Discussion of exiting a program that has made many allocations.
    //     -- RAII causes slowness in these cases!


    logging();
}

/*

  Other likely context functions: assert?


  Possible slightly-more-convenient notation for push_context.


  By the way ... we can do all this inside the compiler...

*/


