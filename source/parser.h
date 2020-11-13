/* ========================================================================
   $File: parser.h
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser
   ========================================================================*/
#pragma once

#include "String.h"
#include "Ast.h"
#include "logger.h"


struct Parser {
	Logger logger = Logger("Parser"_s);
};


void parse_file(const String& filename);
