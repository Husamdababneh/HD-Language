/* ========================================================================
   $File: parser.cpp
   $Date: April 23th 2021 2:13 am
   $Creator: Husam Dababneh
   $Description: typer.cpp
   ========================================================================*/

#include "typer.h"

//Type * types;


Type_Map* types = NULL;

void register_predefined_types() {
	// U8 
	Type U8_type = {1, 1, (U8*)"U8", 0, 0};
	shput(types, U8_type.name, U8_type);
	
	// u16 
	Type u16_type = {2, 2, (U8*)"u16", 0, 0 };
	shput(types, u16_type.name, u16_type);
	
	// U32
	Type U32_type = {4, 4, (U8*)"U32", 0,0};
	shput(types, U32_type.name, U32_type);
	
	// U64
	Type U64_type = {8, 8, (U8*)"U64", 0,0};
	shput(types, U64_type.name, U64_type);
	
	// S8 
	Type S8_type = {1, 1, (U8*)"S8", 0, 0};
	shput(types, S8_type.name, S8_type);
	
	// s16 
	Type s16_type = {2, 2, (U8*)"s16", 0, 0 };
	shput(types, s16_type.name, s16_type);
	
	// s32
	Type s32_type = {4, 4, (U8*)"s32", 0,0};
	shput(types, s32_type.name, s32_type);
	
	// S64
	Type S64_type = {8, 8, (U8*)"S64", 0,0};
	shput(types, S64_type.name, S64_type);
	
	
	// float32
	Type float32_type = {4, 4, (U8*)"float32", 0,0};
	shput(types, float32_type.name, float32_type);
	
	
	// float64
	Type float64_type = {8, 8, (U8*)"float64", 0, 0};
	shput(types, float64_type.name, float64_type);
	
	// String
	Type string_type = {16, 16, (U8*)"string", 0,0};
	shput(types, string_type.name, string_type);
	
	Type bool_type = {1, 1, (U8*)"bool", 0,0};
	shput(types, bool_type.name, bool_type);
	
	
	
	/* 
		Type type = shget(types, "U8");
		printf("Size:%2d, Name[%s], Aliases[], children[]\n", 
			   (int)type.size,
			   type.name);
	 */
	
	/* 
	for(U64 i = 0; i < hmlenu(types); i++){
		auto& type = types[i];
		printf("Type[%2Ii]: Size:%2d, Name[%s], Aliases[], children[]\n", 
			   i,
			   (int)type.value.size,
			   type.value.name);
	}
		 */
	
}
