/* ========================================================================
   $File: parser.h
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser
   ========================================================================*/
#pragma once

#include "Ast.h"
#include "logger.h"
#include "arena.h"

// 1 KB of memory for ast nodes 
// NOTE(Husam): Enough?
//extern Arena ast_arena;

struct Parser {
	Logger logger = Logger("Parser"_s);
};


void parse_file(const String& filename);
