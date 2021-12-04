/* ========================================================================
   $File: auxiliary.cpp
   $Date: 2020-04-05
   $Creator: Husam Dababneh
   $Description: see auxiliary.h
   ========================================================================*/

#include "platform/platform.h"


// NOTE(Husam): HD-Specific read_entire_file, 
//              usually when reading a file we should return a pointer to the data not an object 
// NOTE(Husam): In this case "String" is a light wieght object "Length and Pointer"

String read_entire_file(FILE* file)
{
	// assert(file);
	int descriptor = fileno(file);
	
	struct stat file_stats;
	int result = fstat(descriptor, &file_stats);
	if (result == -1) return {0};
	
	U64 length = file_stats.st_size;
	
	unsigned char* data = new unsigned char[length];
	
	fseek(file, 0, SEEK_SET);
	U64 success = fread((void*)data, length, 1, file);
	if (success < 1) {
		delete[] data;
		return {0};
	}
	
	
	return CStringToString((char*)data, length);
}

String read_entire_file(const char* filepath)
{
	open_file(file, filepath, "rb");
	
	if (!file)
	{
		printf("Couldn't find file [%s]\n", filepath);
		return {};
	}
	
	String data = read_entire_file(file);
	fclose(file);
	return data;
}

String read_entire_file(const StringView& filename)
{
	// assert(filename.count <= 255);
	char name[256];
	
	memcpy((void*)name, (void*)filename.str, filename.length);
	name[filename.length] = '\0';
	return read_entire_file(name);
	
}

StringView readEntireFileToStringView(const StringView& filename)
{
	return read_entire_file(filename);
}

//StringView readEntireFile()