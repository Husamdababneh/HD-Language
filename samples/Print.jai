#import "Basic"

Printer :: #type (buffer: *String_Buffer, any: Any, expansion: *Formatter) -> void;

Formatter :: struct {
    value   : Any;
    printer : Printer;

    silently_set_new_default := false;
}

FormatInt :: struct {
    using formatter : Formatter;

    base := 10;
    minimum_digits := -1;
}

FormatFloat :: struct {
    using formatter : Formatter;

    trailing_width := -1;
}

formatInt :: (value : Any, base := 10, printer := null, silently_set_new_default := false, minimum_digits := -1) -> FormatInt {
    result : FormatInt;

    result.value = value;
    result.base = base;
    result.printer = printer;
    result.silently_set_new_default = silently_set_new_default;
    result.minimum_digits = minimum_digits;

    return result;
}

formatFloat :: (value : Any, trailing_width := -1, printer := null) -> FormatFloat {
    result : FormatFloat;

    result.value = value;
    result.trailing_width = trailing_width;
    result.printer = printer;

    return result;
}


// @Incomplete: Silent failure if buffer fills.
String_Buffer :: struct {
    SIZE :: 1024;

    data: [SIZE] u8;
    occupied: s64;
}

standard_output : HANDLE;


append_u8_and_length :: (buffer: *String_Buffer, s: *u8, length: s64) {
    length_max := String_Buffer.SIZE - buffer.occupied;
    assert(length_max >= 0);

    if length <= 0 return;    // @Robustness: Silently failed.
    if length > length_max  length = length_max;

    memcpy(buffer.data.data + buffer.occupied, cast (*u8) s, length);
    buffer.occupied += length;
}

append_string :: (buffer: *String_Buffer, s: string) {
    append_u8_and_length(buffer, s.data, s.count);
}

buffer_to_string :: (buffer: *String_Buffer) -> string {
    length := buffer.occupied;

    result: string;

    data : *u8 = alloc(length);
    if !data return result;

    memcpy(data, buffer.data.data, length);

    result.data  = data;
    result.count = length;
    return result;
}

write_string :: (s: string) {
    if !standard_output  standard_output = GetStdHandle(STD_OUTPUT_HANDLE);

    written: s32;
    status := WriteFile(standard_output, s.data, xx s.count, *written, null);
}

write_buffer :: (b: *String_Buffer) {
    if !standard_output  standard_output = GetStdHandle(STD_OUTPUT_HANDLE);

    written: s32;
    status := WriteFile(standard_output, b.data.data, xx b.occupied, *written, null); 
}

get_s64_from_void_pointer :: (pointer: *void, size_in_bits: u32) -> s64 {
    if size_in_bits == 64 return <<cast(*s64) pointer;
    if size_in_bits == 32 return <<cast(*s32) pointer;
    if size_in_bits == 16 return <<cast(*s16) pointer;
    if size_in_bits ==  8 return <<cast(*s8)  pointer;

    assert(false);
}

get_u64_from_void_pointer :: (pointer: *void, size_in_bits: u32) -> u64 {
    if size_in_bits == 64 return <<cast(*u64) pointer;
    if size_in_bits == 32 return <<cast(*u32) pointer;
    if size_in_bits == 16 return <<cast(*u16) pointer;
    if size_in_bits ==  8 return <<cast(*u8)  pointer;

    assert(false);
}


base_64_characters :: "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_#";

print_u64_base_n :: (buffer: *String_Buffer, v: u64, format: FormatInt) {
    base := cast(u64) format.base;
    assert(base >=  2);
    assert(base <= 64);

    MAX_OUTPUT_LENGTH :: 64;

    digits := format.minimum_digits;

    if digits > MAX_OUTPUT_LENGTH digits = MAX_OUTPUT_LENGTH;
    if digits == 0 return;  // Do not print anything!


    output : [MAX_OUTPUT_LENGTH] u8;
    
    end := *output[MAX_OUTPUT_LENGTH];
    p   := end;

    while v || (digits > 0) {
        place := v % base;
        v     =  v / base;

        p -= 1;
        << p  = base_64_characters[place];

        digits -= 1;
    }

    if (p == end) && ((digits > 0) || (digits == -1)) {
        p -= 1;
        << p = #char "0";
    }

    assert(p >= output.data);

    append_u8_and_length(buffer, p, end - p);
}

print_float_from_any :: (buffer : *String_Buffer, format : FormatFloat) {
    info := cast(*Type_Info_Float) format.value.type;
    assert(info.type == Type_Info_Tag.FLOAT);

    // Printing floats is kind of hard, so initially we are going to punt
    // and call out to printf.
    

    // Build the format string.

    format_string: [32] u8;
    p := format_string.data;

    << p = #char "%";
    p += 1;
 
    if format.trailing_width != -1 {
        c_string := to_c_string(".%d");
        count := sprintf(p, c_string, format.trailing_width);
        p += count;
    }

    << p = #char "f";
    p += 1;
    << p = 0;


//    printf("format_string is %s\n", format_string);

    output: [1024] u8;  // Overkill.
    if info.size_in_bits == 64 {
        f := << cast(*float64) format.value.value_pointer;
        sprintf(output.data, xx format_string.data, f);
    } else {
        f := << cast(*float32) format.value.value_pointer;
        sprintf(output.data, xx format_string.data, f);
    }

    append_string(buffer, to_string(xx output.data));
}


print_integer_from_any :: (buffer : *String_Buffer, format : FormatInt) {
    any := format.value;
    assert(any.type.type == Type_Info_Tag.INTEGER);

    info := cast(*Type_Info_Integer) any.type;

    v : u64 = ---;

    if format.base == 10 {
        if info.signed {
            v_signed := get_s64_from_void_pointer(any.value_pointer, info.size_in_bits);

            if v_signed < 0 {
                append_string(buffer, "-");
                v = cast(u64) -v_signed;
            } else {
                v = cast(u64)  v_signed;
            }
        } else {
            v = get_u64_from_void_pointer(any.value_pointer, info.size_in_bits);
        }
    } else {
        v = get_u64_from_void_pointer(any.value_pointer, info.size_in_bits);
    }

    print_u64_base_n(buffer, v, format);
}

print_sequence :: (args: ..) { 
    buffer: String_Buffer;

    print_sequence_to_buffer(*buffer, ..args);

    write_buffer(*buffer);
}

sprint_sequence :: (args: ..) -> string {
    buffer: String_Buffer;

    print_sequence_to_buffer(*buffer, ..args);

    return buffer_to_string(*buffer);
}


print :: (format_string: string, args: ..) { 
    buffer: String_Buffer;

    print_to_buffer(*buffer, format_string, ..args);

    write_buffer(*buffer);
}

sprint :: (format_string: string, args: ..) -> string {
    buffer: String_Buffer;

    print_to_buffer(*buffer, format_string, ..args);

    return buffer_to_string(*buffer);
}

logprint :: (ident: string, mode: Log_Mode.strict, format_string: string, args: ..) {
    buffer: String_Buffer;

    print_to_buffer(*buffer, format_string, ..args);

    s : string;
    s.data  = buffer.data.data;
    s.count = buffer.occupied;

    if !s.count return;

    context.logger(s, ident, mode, context.logger_data);
}

append_struct_info :: (buffer: *String_Buffer, info: *Type_Info_Struct, base_pointer: *void) {
    if info.name then {
        append_string(buffer, "struct ");
        append_string(buffer, info.name);
        append_string(buffer, " :: ");
    } else {
        append_string(buffer, "struct :: ");
    }

    using Type_Info_Tag.members;

    for info.members {
        append_string(buffer, it.name);
        append_string(buffer, ": ");

        address := (cast(*u8) base_pointer) + it.offset_in_bytes;

        any : Any;
        any.type = it.type;
        any.value_pointer = address;

        if it.type.type == FLOAT {
            string_heads_match :: (a: string, b: string) -> bool {
                if b.count < a.count return false;

                for i : 0..a.count-1 {
                    if a[i] != b[i] then return false;
                }

                return true;
            }

            precision := -1;
            for note : it.notes {
                head := "print_precision";

                if string_heads_match(head, note) {
                    post, value := parse_number(note, head.count);

                    precision = value;
                }
            }

            // @Cleanup: We might want a more-convenient way to 
            // do these notes without overriding print_item_to_buffer
            // completely.

            format : FormatFloat;
            format.trailing_width = precision;
            format.value = any;

            print_float_from_any(buffer, format);
        } else {
            print_item_to_buffer(buffer, any);
        }

        append_string(buffer, "; ");
    }
}

print_item_to_buffer :: (buffer: *String_Buffer, it: Any, default_formatters: *[] *Formatter = null) {
    using Type_Info_Tag.members;

    t := it.type.type;

    default_formatter : *Formatter;
    if default_formatters {
        default_formatter = (<< default_formatters)[t];
    }

    if t == STRING {
        s := << cast(*string) it.value_pointer;
        if s {
            append_string(buffer, s);
        } else {
            append_string(buffer, "(null)");
        }

        return;
    }

    if t == BOOL {
        b := << cast(*bool) it.value_pointer;
        if b {
            append_string(buffer, "true");
        } else {
            append_string(buffer, "false");
        }

        return;
    }

    if t == INTEGER {
        if default_formatter {
            default_formatter.value = it;
            print_integer_from_any(buffer, << cast(*FormatInt) default_formatter);
        } else {
            default : FormatInt;
            default.value = it;
            print_integer_from_any(buffer, default);
        }

        return;
    }

    if t == FLOAT {
        if default_formatter {
            default_formatter.value = it;
            print_float_from_any(buffer, << cast(*FormatFloat) default_formatter);
        } else {
            default : FormatFloat;
            default.value = it;
            print_float_from_any(buffer, default);
        }

        return;
    }

    if t == POINTER {
        u64_val := << cast(*u64) it.value_pointer;  // Assumes we are running in 64-bit. @WordSize
        
        default : FormatInt;
        default.value = u64_val;
        default.base = 16;

        print_integer_from_any(buffer, default);

        return;
    }

    if t == STRUCT {
        info_struct := cast(*Type_Info_Struct) it.type;
        expansion := isFormatter(info_struct);

        if expansion {
            expander := cast(*Formatter) it.value_pointer;

            // If it's a silent default-setter, set the new default pointer, then bail.

            if expander.silently_set_new_default {
                if default_formatters {
                    index := expander.value.type.type;
                    (<< default_formatters)[index] = expander;
                    return;
                }
            }

            if it.type == type_info(FormatInt) {
                format := << cast(*FormatInt) it.value_pointer;
                print_integer_from_any(buffer, format);
                return;
            } else if it.type == type_info(FormatFloat) {
                format := << cast(*FormatFloat) it.value_pointer;
                print_float_from_any(buffer, format);
                return;
            }
        }


        printer : Printer;

        if expansion {
            expander := cast(*Formatter) it.value_pointer;

            // Look for a printer and then recurse, printing our value.

            printer = expander.printer;
            if !printer  printer = get_printer(expander.value.type);

            if printer printer(buffer, expander.value, expander);
        } else {
            printer = get_printer(it.type);
            if printer printer(buffer, it, null);
        }

        if !printer {
            append_string(buffer, "{ ");
            
            append_struct_info(buffer, info_struct, it.value_pointer);

            append_string(buffer, " }");
        }

        return;
    }

    if t == ARRAY {
        info_array := cast(*Type_Info_Array) it.type;
        
        // Incomplete: Print array type.
        element_type := "(some element type)";
        if info_array.array_type == Type_Info_Array.FIXED {
            append_string(buffer, "[");
            print_item_to_buffer(buffer, info_array.array_count);
            append_string(buffer, "] ");
        } else if info_array.array_type == Type_Info_Array.STATIC {
            append_string(buffer, "[] ");
        } else if info_array.array_type == Type_Info_Array.DYNAMIC {
            append_string(buffer, "[..] ");
        } else {
            assert(false);
        }

        if info_array.soa_packing != -1
            append_string(buffer, "SOA ");

        append_string(buffer, element_type);

        if info_array.array_type == Type_Info_Array.STATIC {
            array := cast(*Static_Array) it.value_pointer;
            append_string(buffer, " { count = ");
            print_item_to_buffer(buffer, array.count);
            append_string(buffer, ", data = ");
            print_item_to_buffer(buffer, array.data);
            append_string(buffer, "}");
        } else if info_array.array_type == Type_Info_Array.DYNAMIC {
            array := cast(*Dynamic_Array) it.value_pointer;
            append_string(buffer, " { count = ");
            print_item_to_buffer(buffer, array.count);
            append_string(buffer, ", data = ");
            print_item_to_buffer(buffer, array.data);
            append_string(buffer, ", allocated = ");
            print_item_to_buffer(buffer, array.allocated);
            append_string(buffer, ", allocator = ");
            print_item_to_buffer(buffer, array.allocator);
            append_string(buffer, "}");
        }

        return;
    }

    if t == PROCEDURE {
        append_string(buffer, "(procedure 0x");

        pv := << cast(**void) it.value_pointer;  // @Hack
        print_item_to_buffer(buffer, pv);
        append_string(buffer, ")");
        return;
    }

    if t == ENUM {
        // Kind of ugly, but let's go for it.

        info_enum := cast(*Type_Info_Enum) it.type;

        info_struct := info_enum.struct_produced;

        names_field := get_field(info_struct, "names");
        values_field := get_field(info_struct, "values");
        assert(names_field != null);
        assert(values_field != null);

        if !names_field || !values_field {
            append_string(buffer, "(enum)");  // @Incomplete: Return error code.
            return; 
        }

        array_type := cast(*Type_Info_Array) names_field.type;
        assert(array_type.type == ARRAY);

        count := array_type.array_count;
        assert(count >= 0);

        data_segment := XXX_temporary_get_start_of_data_segment();

        names_offset := names_field.XXX_temporary_offset_to_constant_value;
        values_offset := values_field.XXX_temporary_offset_to_constant_value;

        names_pointer  := cast(**u8) (data_segment + names_offset);
        values_pointer := cast(*s64)    (data_segment + values_offset);

        // Now we have the names array, but we have to index it by the enum value,
        // which could be of various sizes...

        // However, the element stored in values_pointer are currently always
        // s64, apparently..

        info_int := cast(*Type_Info_Integer) info_enum.internal_type;
        assert(info_int.type == INTEGER);


        target_value : s64;
        size := info_int.size_in_bits;

        if size == 64 {
            target_value = << cast(*s64) it.value_pointer;
        } else if size == 32 {
            target_value = << cast(*s32) it.value_pointer;
        } else if size == 16 {
            target_value = << cast(*s16) it.value_pointer;
        } else if size ==  8 {
            target_value = << cast(*s8) it.value_pointer;
        }

//        assert(target_value >= 0);
//        assert(target_value < count);

        // Get values_count.
        values_array_type := cast(*Type_Info_Array) values_field.type;
        assert(values_array_type.type == Type_Info_Tag.ARRAY);
        assert(values_array_type.array_type == Type_Info_Array.FIXED);
        values_count := values_array_type.array_count;

        target_index := -1;
        for 0..values_count-1 {
            if values_pointer[it] == target_value {
                target_index = it;
                break;
            }
        }

        if target_index == -1 {
            append_string(buffer, "(enum out of range: ");
            print_item_to_buffer(buffer, target_value);
            append_string(buffer, ")");
        } else {
            name := names_pointer[target_index];
            append_string(buffer, to_string(name));
        }

        return;
    }

    // This is a type we don't know or don't yet handle.

    append_string(buffer, "(unknown type)");
}


is_digit :: (c: s64) -> bool {
    relative := c - #char "0";
    return (relative >= 0) && (relative <= 9);
}

parse_number :: (s: string, cursor: s64) -> end_cursor: s64, sum: s64 {
    sum := 0;

    while true {
        c := s[cursor];
        if !is_digit(c) break;

        sum *= 10;
        sum += c - #char "0";

        cursor += 1;
    }

    return cursor, sum;
}
            

print_to_buffer :: (buffer : *String_Buffer, format_string : string, args: ..) -> bool {
    default_formatters : [Type_Info_Tag.HIGHEST_VALUE] *Formatter;

    implicit_index_cursor := 0;

    cursor := 0;
    printed := 0;
    while cursor < format_string.count {
        c := format_string.data[cursor];

        if c != #char "%" {
            cursor += 1;
            continue;
        }

        append_u8_and_length(buffer, format_string.data + printed, cursor - printed);
        cursor += 1;  // Skip the %.

        next := format_string.data[cursor];
        if next == #char "%" {  // Double-percent means to actually output a percent.
            append_string(buffer, "%");
            cursor += 1;
            printed = cursor;
            continue;
        }

        value : s64;

        // If it's not a digit, we take this as a cue to read our cursor and bump it forward.
        if !is_digit(next) {
            value = implicit_index_cursor;  // Will get bumped below.
        } else {
            // It is a digit, so we parse a number.

            // @Robustness: We probably want to test that parse_number does not overflow!

            cursor, value = parse_number(format_string, cursor);
            value -= 1;
        }


        if value < 0 {
            return false;  // @Incomplete: Give a reason. Maybe continue printing?
        }

        if value >= args.count {
            return false;  // @Incomplete: Give a reason. Maybe continue printing?
        }

        item := args[value];

        untyped : [] *Formatter = default_formatters; // Get around a casting problem when taking pointer.
        print_item_to_buffer(buffer, item, *untyped);
        implicit_index_cursor = value + 1;

        printed = cursor;   // Next time we append, start here.
    }

    append_u8_and_length(buffer, format_string.data + printed, cursor - printed);
}

print_sequence_to_buffer :: (buffer : *String_Buffer, args: ..) { 
    for args {
        print_item_to_buffer(buffer, it);
    }
}

get_printer :: (info : *Type_Info) -> Printer {
    if info.type != Type_Info_Tag.STRUCT return null;

    info_struct := cast(*Type_Info_Struct) info;

    field := get_field(info_struct, "print_me");
    if !field return null;

    offset := field.XXX_temporary_offset_to_constant_value;
    if offset == -1 return null;

    data_segment := XXX_temporary_get_start_of_data_segment();
    function_pointer := cast(*Printer) (data_segment + offset);

    return <<function_pointer;
}



isFormatter :: (info: *Type_Info_Struct) -> *Type_Info_Struct_Member {
    for * info.members {
        if it.type == type_info(Formatter) return it;
    }

    return null;
}

