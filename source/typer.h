/* ========================================================================
$File: typer.h
$Date: April 17th 2021 2:17 am
$Creator: Husam Dababneh
$Description: typer.h
========================================================================*/

#ifndef TYPER_H
#define TYPER_H

struct Alias;

struct Type {
	u32 size;
	u32 alignment;
	u8* name;
	Alias* aliases;
	Type* children;
};

struct Type_Map {
	u8* key;
	Type value;
};

extern Type_Map* types;

void register_predefined_types();

/*
struct Alias {
	Type* realType;
	StringView name;
	Alias* aliases;
};


String type {
		 
size = 8 + 8 // sizeof string, + sizeof pointer to string
name = string
aliases = NULL
children {
u64 size;
stringPtr  { data, string, str}  
}

}


*/


#endif //TYPER_H
