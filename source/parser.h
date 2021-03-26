/* ========================================================================
   $File: parser.h
   $Date: 2020-10-16
   $Creator: Husam Dababneh
   $Description: parser
   ========================================================================*/
#pragma once



struct Parser {
	Logger logger = Logger("Parser"_s);
};


void parse_file(const String& filename);
void parse_str(const String& str);
