/*

  WARNING! Creating threads is not currently threadsafe!
  Only do it from the main thread for now. This is easily
  fixed, but we don't have mutexes yet...

*/

Thread_Index :: s64;
Thread_Proc  :: #type (*Thread) -> s64;

Thread_Os_Specific :: struct {
    windows_thread:    HANDLE;
    windows_thread_id: s32;
}

Thread :: struct {
    index : Thread_Index;
    proc  : Thread_Proc;
    data  : *void;

    starting_context: Context;
    use_this_starting_context := false;

    using specific : Thread_Os_Specific;
}

global_thread_table : [..] *Thread;

thread_assert_all_are_stopped :: () {
    live_count := 0;

    for global_thread_table {
        if it != null  live_count += 1;
    }

//    print("live_count is %\n", live_count);
    assert(live_count == 0);
}

thread_create :: (proc: Thread_Proc) -> *Thread {
    windows_thread_id : s32;

    windows_thread_proc := cast(*void) __jai_windows_thread_entry_proc;
    if running_at_compile_time() {
        windows_thread_proc = bytecode_get_foreign_function_pointer(windows_thread_proc);
        if !windows_thread_proc return null;
    }

    thread := new Thread;
    windows_thread := CreateThread(null, 0, windows_thread_proc, thread, CREATE_SUSPENDED, *windows_thread_id);

    if !windows_thread {
        free(thread);
        return null;
    }

    thread.proc = proc;

    thread.windows_thread_id = windows_thread_id;
    thread.windows_thread    = windows_thread;


    // Put the thread into an empty slot in the table.

    index := find_empty_slot(global_thread_table);
    if index == -1 {
        array_add(*global_thread_table, thread);
        index = global_thread_table.count - 1;
    } else {
        global_thread_table[index] = thread;
    }

    thread.index = index+1;

    return thread;
}

thread_start :: (thread: *Thread) {
    ResumeThread(thread.windows_thread);
}

thread_is_done :: (thread: *Thread) -> bool {
    result := WaitForSingleObject(thread.windows_thread, 0);

    return result != WAIT_TIMEOUT;
}

thread_destroy :: (thread: *Thread) {
    CloseHandle(thread.windows_thread);

    assert(thread.index > 0);
    table_index := thread.index - 1;

    // print("thread index %, table entry = %, i am %\n", thread.index, global_thread_table[table_index], thread);

    assert(global_thread_table[table_index] == thread);
    global_thread_table[table_index] = null;
    free(thread);
}



//
// Windows API stuff:
//

CREATE_SUSPENDED :: 0x4;
WAIT_TIMEOUT     :: 0x102;

CreateThread :: (threadAttributes: *void, stackSize: s64, startAddress: *void, parameter: *void,
                 creation_flags: s32, threadIdReturn: *s32) -> HANDLE #foreign;

ResumeThread :: (thread: HANDLE) -> s32 #foreign;

WaitForSingleObject :: (handle: HANDLE, milliseconds: s32) -> s32 #foreign;

__jai_windows_thread_entry_proc :: (parameter: *void) -> s32 #foreign;
