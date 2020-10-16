CreateFileA   :: (filename: *u8, desired_access: s32, share_mode: s32 = FILE_SHARE_READ, security: *void = null, creation: s32 = FILE_OPEN_EXISTING, flags_and_attributes: s32 = 0, hTemplateFile: HANDLE = 0) -> HANDLE #foreign;

GetFileSizeEx :: (file: HANDLE, size: *LARGE_INTEGER) -> s32 #foreign;

ReadFile :: (file: HANDLE, buffer: *void, to_read: s32, did_read: *s32, overlapped: *void = null) -> s32 #foreign;

LARGE_INTEGER :: struct {
    QuadPart: s64;
}

FILE_GENERIC_ALL     :: 0x10000000;
FILE_GENERIC_READ    :: 0x80000000;
FILE_GENERIC_WRITE   :: 0x40000000;
FILE_GENERIC_EXECUTE :: 0x20000000;

File_Os_Specific :: struct {
    // Windows version.
    handle: HANDLE;
};

File :: struct {
    using specific : File_Os_Specific;
};


file_open :: (name: string, for_writing := false) -> File {
    c_name := to_c_string(name);

    handle: HANDLE;
    if for_writing {
        handle = CreateFileA(c_name, FILE_GENERIC_WRITE, FILE_SHARE_READ,
                            creation = FILE_CREATE_ALWAYS);
    } else {
        handle = CreateFileA(c_name, FILE_GENERIC_READ);
    }

    file: File;
    file.handle = handle;

    return file;
}

file_close :: (file: *File) {
    CloseHandle(file.handle);
}


read_entire_file :: (name: string) -> string, bool {
    c_string := to_c_string(name);
    defer free(c_string);

    s: string;

    f := file_open(name);
    if f.handle == INVALID_HANDLE_VALUE return s, false;

    size_struct: LARGE_INTEGER; // Kind of pointless, just pass *s64?
    success := GetFileSizeEx(f.handle, *size_struct);
    if !success return s, false;

    length : s64 = size_struct.QuadPart;

    data : *u8 = alloc(length);
    if data == null return s, false;
    
    single_read_length: s32;
    total_read:  s64;

    while total_read < length {
        remaining := length - total_read;
        to_read : s32;
        if remaining <= 0x7fffffff {
            to_read = cast(s32) remaining;
        } else {
            to_read = 0x7fffffff;
        }

        ReadFile(f.handle, data + total_read, to_read, *single_read_length);
        if single_read_length <= 0 {  // @Robustness: Probably we are supposed to check the return code.
            free(data);
            return s, false;
        }

        total_read += single_read_length;
    }

    s.count = length;
    s.data = data;

    file_close(*f);

    return s, true;
}
