/* ========================================================================
   $File: auxiliary.cpp
   $Date: 2020-04-05
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: see auxiliary.h
   ========================================================================*/
#include "auxiliary.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif

u64 read_entire_file(FILE* file, void** data_return)
{
	assert(file);
	int descriptor = fileno(file);

	struct stat file_stats;
	int result = fstat(descriptor, &file_stats);
	if (result == -1) return -1;

	u64 length = file_stats.st_size;

	unsigned char* data = new unsigned char[length];

	fseek(file, 0, SEEK_SET);
	u64 success = fread((void*)data, length, 1, file);
	if (success < 1) {
		delete[] data;
		return -1;
	}

	*data_return = data;
	return length;
}

u64 read_entire_file(const char* filepath, void** data_return)
{
	FILE* file;
#ifdef WIN32
	fopen_s(&file, filepath, "rb");
#else
	file = fopen(filepath, "rb");
#endif
	if (!file)
	{
		printf("Couldn't find file [%s]\n", filepath);
		return false;
	}
	assert(file);
	int descriptor = fileno(file);

	struct stat file_stats;
	int result = fstat(descriptor, &file_stats);
	if (result == -1) return -1;

	u64  length = file_stats.st_size;

	unsigned char* data = new unsigned char[length];

	fseek(file, 0, SEEK_SET);
	u64  success = fread((void*)data, length, 1, file);
	if (success < 1) {
		delete[] data;
		return -1;
	}

	*data_return = data;
	return length;
}

u64 read_entire_file(const String& filename, void** data_return)
{
	assert(filename.count <= 255);
	char name[256];

	memcpy(name, filename.data, filename.count);
	name[filename.count] = '\0';
	return read_entire_file(name, data_return);

}
