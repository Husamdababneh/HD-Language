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
	U8* name;
	Alias* aliases;
	Type* children;
	U16 size;
	U16 alignment;
};

struct Type_Map {
	U8* key;
	Type value;
};

extern Type_Map* types;



#endif //TYPER_H
