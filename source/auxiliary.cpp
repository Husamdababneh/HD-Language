/* ========================================================================
   $File: auxiliary.cpp
   $Date: 2020-04-05
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: see auxiliary.h
   ========================================================================*/
#include "auxiliary.h"
#include <stdio.h>




Arguments ParseArguments(int argc, char ** argv)
{
	Arguments args;
	if(argc > MAX_ARG_COUNT)
		return args;
	
    // TODO@TODO(Husam): Report error
	int haha = 3;
	for(int a = 1; a < argc; a++)
	{

		//std::cout << "Proccessing " << argv[a] << "\n";
		if(argv[a][0] != '-')
			return {};

		if(char *sub = (argv[a],INPUT); sub)
		{
			args.inputFile = &sub[strlen(INPUT)];
			haha--;
		}

		if(char *sub = strstr(argv[a],OUTPUT);sub)
		{
			args.outputFile = &sub[strlen(OUTPUT)];
			haha--;
		}
		// we are looking to parse this file
		if(char *sub = strstr(argv[a],BINARY);sub)
		{
			args.structureFile = &sub[strlen(BINARY)];
			haha--;
		}
	}
	if(haha < 3)
		args.isSet = true;
	else
	{
		
		printf("Usage : %s  -inputfile:<inputfilename>  -outputfile:<outputfilename>  -binary:<binarystructurefilename> \n",argv[0]);
	}
	return args;
}

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif

int read_entire_file(FILE* file, void** data_return)
{
	assert(file);
    int descriptor = fileno(file);

    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1) return -1;

    int length = file_stats.st_size;

    unsigned char *data = new unsigned char[length];

    fseek(file, 0, SEEK_SET);
    int success = fread((void *)data, length, 1, file);
    if (success < 1) {
        delete [] data;
        return -1;
    }

    *data_return = data;
    return length;
}

int read_entire_file(const char *filepath , void** data_return)
{
	
	FILE* file = fopen(filepath, "rb");
	if(!file)
	{
		printf("Couldn't find file [%s]\n", filepath);
		return false;
	}
	assert(file);
    int descriptor = fileno(file);

    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1) return -1;

    int length = file_stats.st_size;

    unsigned char *data = new unsigned char[length];

    fseek(file, 0, SEEK_SET);
    int success = fread((void *)data, length, 1, file);
    if (success < 1) {
        delete [] data;
        return -1;
    }

    *data_return = data;
    return length;
}

int read_entire_file(const String& filename  , void** data_return)
{
	assert(filename.count <= 255);
	char name[256];

	memcpy(name, filename.data, filename.count);
	name[filename.count] = '\0';
	return read_entire_file(name, data_return);
	
}

#ifdef WIN32
#undef fileno
#undef fstat 
#undef stat
#endif



Memory alloc_memory(u64 size){
	return {};
}

void free_memory(Memory* memory){
	
}

Memory alloc_memoryslot(Memory*  memory){
	return {};
}

void  free_memoryslot(MemorySlot* slot){
	
}
