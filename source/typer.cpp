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
	// u8 
	Type u8_type = {1, 1, (u8*)"u8", 0, 0};
	shput(types, u8_type.name, u8_type);
	
	// u16 
	Type u16_type = {2, 2, (u8*)"u16", 0, 0 };
	shput(types, u16_type.name, u16_type);
	
	// u32
	Type u32_type = {4, 4, (u8*)"u32", 0,0};
	shput(types, u32_type.name, u32_type);
	
	// u64
	Type u64_type = {8, 8, (u8*)"u64", 0,0};
	shput(types, u64_type.name, u64_type);
	
	// s8 
	Type s8_type = {1, 1, (u8*)"s8", 0, 0};
	shput(types, s8_type.name, s8_type);
	
	// s16 
	Type s16_type = {2, 2, (u8*)"s16", 0, 0 };
	shput(types, s16_type.name, s16_type);
	
	// s32
	Type s32_type = {4, 4, (u8*)"s32", 0,0};
	shput(types, s32_type.name, s32_type);
	
	// s64
	Type s64_type = {8, 8, (u8*)"s64", 0,0};
	shput(types, s64_type.name, s64_type);
	
	
	// float32
	Type float32_type = {4, 4, (u8*)"float32", 0,0};
	shput(types, float32_type.name, float32_type);
	
	
	// float64
	Type float64_type = {8, 8, (u8*)"float64", 0, 0};
	shput(types, float64_type.name, float64_type);
	
	// String
	Type string_type = {16, 16, (u8*)"string", 0,0};
	shput(types, string_type.name, string_type);
	
	Type bool_type = {1, 1, (u8*)"bool", 0,0};
	shput(types, bool_type.name, bool_type);
	
	
	
	/* 
		Type type = shget(types, "u8");
		printf("Size:%2d, Name[%s], Aliases[], children[]\n", 
			   (int)type.size,
			   type.name);
	 */
	
	/* 
	for(u64 i = 0; i < hmlenu(types); i++){
		auto& type = types[i];
		printf("Type[%2Ii]: Size:%2d, Name[%s], Aliases[], children[]\n", 
			   i,
			   (int)type.value.size,
			   type.value.name);
	}
		 */
	
}
