/* ========================================================================
$File: typer.h
$Date: April 17th 2021 2:17 am
$Creator: Husam Dababneh
$Description: typer.h
========================================================================*/

#ifndef TYPER_H
#define TYPER_H

struct Alias;

struct DataType {
	String name;
	//Alias* aliases;
	DataType* children;
	U16 size;
	U16 alignment;
};

struct Type_Map {
	U8* key;
	Ast_Type* value;
};

//extern Type_Map* types;

/*
*/

#endif //TYPER_H
