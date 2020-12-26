import os

a  = '/*\n*\n* DO NOT EDIT THIS FILE . This File is generated by "generator.py"\n*\n*/\n\n'

TokenTypes = {
	"IDENT" : [True, 256]  ,
	"HDTYPE": [False, 0],
	"KEYWORD" : [False, 0]  ,
	"LITERAL" : [False, 0]  ,
	"COMMENT" : [False, 0]  ,
	"MULTILINE_COMMENT" : [False, 0]  ,
	"COLON" : [False, 0] ,
	"DOUBLECOLON" : [False, 0] ,
	"COLONEQUAL" : [False, 0] ,
	"OPERATOR" : [False, 0] ,
	"DIRECTIVE" : [False, 0] ,
	"DOUBLEDOT": [False, 0] ,
	
	"STRING_LITERAL": [False, 0] ,
	"INT_LITERAL": [False, 0] ,
	"FLOAT_LITERAL": [False, 0] ,

	
	"ARROW": [False, 0] ,

	"ASSIGN": [False, 0] ,
	"EQL": [False, 0] ,
	"GT": [False, 0] ,
	"LT": [False, 0] ,
	"GT_OR_EQL": [False, 0] ,
	"LT_OR_EQL": [False, 0] ,

	"SHIFT_LEFT": [False, 0] ,
	"SHIFT_RIGHT": [False, 0] ,
#	"FLOAT_LITERAL": [False, 0] ,
#	"FLOAT_LITERAL": [False, 0] ,
#	"FLOAT_LITERAL": [False, 0] ,
	
	
	"NONE" : [False, 0],
	"EOFA" : [False, 0] ,
	"ERROR" : [False, 0]
}

def generateEnum(headerfile, cppfile,  enumValues):
	headerfile.write('\nenum  {\n')
	for token, value in enumValues.items():
		headerfile.write("    ")
		headerfile.write("TOKEN_" +token)
		if value[0] != False:
			headerfile.write(" = " + str(value[1]))
		headerfile.write(",\n")
	headerfile.write('};\n\n')

def generateStringArray(headerfile, cppfile,  enumValues):
	headerfile.write('String ' +  "ETOKENStrings[]" + ';\n')
	cppfile.write('String ' +  "ETOKENStrings[] =  " + ' {\n')
	for token, value in enumValues.items():
		cppfile.write("    ")
		cppfile.write('\"' + token + '\"_s')
		cppfile.write(",\n")
	cppfile.write('};\n\n')

def generateFunction(headerfile, cppfile,  enumValues):
	headerfile.write('String ' + "ETOKENToString(u64 var)" + ';\n')
	cppfile.write('String ' + " ETOKENToString(u64 var)" + ' {\n')
	cppfile.write("    ")
	cppfile.write("if (var < " + "TOKEN_IDENT)" +"\n")
	cppfile.write("    ")
	cppfile.write("    ")
	cppfile.write("return {(u8*) var , 1};\n")
	cppfile.write("    ")
	cppfile.write("return " +  "ETOKENStrings[(u64)var - (u64)TOKEN_IDENT  + 1];\n")
	cppfile.write('};\n\n')
	
def generateFile() :
	cppfile = open("constants.cpp", 'w')
	headerfile = open("constants.h", 'w')
	cppfile.write(a)
	headerfile.write(a)
	headerfile.write("#include \"common.h\"\n")
	headerfile.write("#include \"mystring.h\"\n")
	cppfile.write("#include \"pch.h\"\n")
	cppfile.write("#include \"constants.h\"\n\n")
	
	generateEnum(headerfile, cppfile, TokenTypes)
	generateStringArray(headerfile, cppfile,  TokenTypes)
	generateFunction(headerfile, cppfile,  TokenTypes)
	cppfile.close()
	headerfile.close()
	
generateFile()
