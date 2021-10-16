/* ========================================================================
$File: base.h
$Date: July 31st 2021 4:02 pm
$Creator: Husam Dababneh
$Desc:  
=========================================================================*/
/* NOTEs
		
																						
	*/

/* TODOs

TODO(Husam Dababneh): Create new Printf function to handle our strings :) !!

	TODO(Husam Dababneh): Add Intel Compiler ?? 
TODO(Husam Dababneh): Architectures

*/

#ifndef BASE_H
#define BASE_H


//~
#if !defined(HD_TYPES)
# define HD_TYPES
#endif

//~ 
// NOTE(Husam Dababneh): Some useful Macros

#define stringify_literal( x ) # x
#define stringify_expanded( x ) stringify_literal( x )
#define stringify_with_quotes( x ) stringify_expanded( stringify_expanded( x ) )
#define contact(x, y) #x ": " stringify_with_quotes(y)

#ifdef offsetof
#undef offsetof
#define offsetof(a,b) ((U64)(&(((a*)(0))->b)))
#endif
#define member_size(type, member) sizeof(((type *)0)->member) // this atually works :) 


//~ Compilers

// NOTE(Husam Dababneh): Do I Need more Compilers ?? I dont think so :)
// TODO(Husam Dababneh): Add Intel Compiler ?? 
#if defined(_MSC_VER)
# define COMPILER_CL 1 // MSVC Default Compiler
# define COMPILER_NAME contact(Microsoft Visual C++ Compiler (CL) Version, _MSC_FULL_VER)
# if defined(_M_AMD64) || defined(_M_X64)
#  define ARCH_AMD64 1
#  define ARCH_NAME "AMD 64-bit Architecture"
# elif defined(_M_ARM)
#  define ARCH_ARM   1
#  define ARCH_NAME "ARM Architecture"
# elif defined(_M_I86)
#  define ARCH_INTEL32   1
#  define ARCH_NAME "Intel 16-Bit Architecture"
# elif defined(_M_IX86)
#  define ARCH_INETL64   1
#  define ARCH_NAME "Intel 32-Bit Architecture"
# endif 
#elif defined(__clang__)
# // TODO(Husam Dababneh):  Architectures
# define COMPILER_CLANG 1
# define COMPILER_NAME contact(Clang C++ Compiler (Clang) Version, __clang_version__)
#elif defined(__GNUC__)
# define COMPILER_GCC 1
# // TODO(Husam Dababneh):  Architectures
# if defined(__GNUC_PATCHLEVEL__)
#    define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
# else
#    define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
# endif
# define COMPILER_NAME contact(GCC C++ Compiler (GCC) Version, __GNUC_VERSION__ )
#else
# error Unregistered Compiler
#endif 


// Zero Fill missing context macros
#if   !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif

#if !defined(COMPILER_NAME)
# define COMPILER_NAME "Unknown Compiler"
#endif

//~ Operating System
// TODO(Husam Dababneh): Check if can merge this with the above
#if defined(__linux__)
# define OS_LINUX 1
# define OS_NAME "Linux"
# define OS OS_NAME
#elif defined(_WIN64)
# define OS_WINDOWS 1
# define OS_WINDOWS_64 1
# define OS_NAME "Windows x64"
# define OS OS_NAME
#elif defined(_WIN32)
# define OS_WINDOWS 1
# define OS_WINDOWS_32 1
# define OS_NAME "Windows x32"
# define OS OS_NAME
#elif defined(__unix__)
# define OS_UNIX 1
# define OS_NAME "Unix"
# define OS OS_NAME
#elif defined(__FreeBSD__)
# define OS_FREEBSD 1
# define OS_NAME "FreeBSD"
# define OS OS_NAME
#elif defined(__APPLE__ ) && defined(__MACH__)
# define OS_MAC 1
# define OS_NAME "Apple Macintosh (MAC)"
# define OS OS_NAME
#else 
# error Unregistered Operating System 
#endif 

// Zero Fill missing OS macros
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_WINDOWS_32)
# define OS_WINDOWS_32 0
#endif
#if !defined(OS_WINDOWS_64)
# define OS_WINDOWS_64 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_UNIX)
# define OS_UNIX  0
#endif
#if !defined(OS_FREEBSD)
# define OS_FREEBSD 0
#endif
#if !defined(OS_MAC )
# define OS_MAC 0
#endif
#if !defined(OS_NAME)
# define OS_NAME "Unknown Operating System"
#endif

// NOTE(Husam Dababneh): The whole purpose of this defines are to make sure the user
//                       know his working environment to take action based on his situation.
#if !defined(OS)
#error Could npt detect Operating System (You may diable this error if you know what you are doing)
#endif

//~ Basic Types

#ifdef HD_TYPES

#include <stdint.h>

// Fixed Width singed integers
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef void*    PTR;
//typedef intptr_t PTR;

// Fixed Width unsinged integers
typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;

// This is actually not neccecry 
typedef float     F32;
typedef double    F64;

// Any Value but 0 is true
typedef uint8_t   B8;
typedef uint16_t  B16;
typedef uint32_t  B32;
typedef uint64_t  B64;

// Sizes
typedef size_t Size;

#define _BYTE_ 1
#define KB(x) (U64)(x * 1024)
#define MB(x) (U64)(KB(x) * 1024)
#define GB(x) (U64)(MB(x) * 1024)
#define TB(x) (U64)(GB(x) * 1024)

#endif //HD_TYPES


//~ Strings and String Views 
// This is a simple String/StringView implementation.
// A String Builder will be provided in a separate file 
// NOTE(Husam Dababneh): I dont want to complicate things here

// NOTE(Husam Dababneh): This can be used as a String view!!.. we'll see what happens
struct String {
	// Anonymous Union Because we don't want to deal with str.<union name>.<field name>
	union {U64 length; U64 size;};
	union {S8* str; char* str_char;};
	
	// NOTE(Husam Dababneh): When this is true, the string (in memory) has a size of length + 1
	B8 isNullTerminated;
	
	// TODO: Bounds check here
	S8 operator[](U64 a){return str[a];}
	
	struct Iterator {
		S8* str;
		U64 index;
		
		// NOTE(Husam Dababneh): Do we even support C ?? 
		// This may cause errors with #include
		bool operator !=(Iterator a){ return this->str != a.str; }
		void operator ++(){++str;++index;}
		void operator ++(int){str++;index++;}
		Iterator operator * () const{return *this;}
	};
	// This may cause errors with #include
	typedef const Iterator const_iterator;
	Iterator begin() {Iterator a = {str, 0}; return a;}
	const_iterator begin() const {Iterator a = {str, 0}; return a;}
	Iterator end() {Iterator a = {str + length, length};return a;}
	const_iterator end() const {Iterator a = {str + length, length};return a;}
	
	// TODO(Husam Dababneh): Do we need to specify if the string in null terminated ?? 
	// TODO(Husam Dababneh): i think this will cause issue with hashmap ??  
};

// Forward Declarations  
typedef String StringView;
inline  String operator ""_s(const char* string, U64 length);
inline  String CStringToString(char* string, U64 length);
inline  String CStringToString(char* string);

S8      CompareStrings(String left, String right);
B8      EqualStrings(String left, String right);
#define SV_PRINT(x) (int)x.length, x.str

#if defined(HD_BASE_IMPL)
//#define HD_BASE_IMPL
String operator ""_s(const char* string, U64 length)
{
	String str = {length, (S8*)string, true}; 
	return str;
}


String CStringToString(char* string, U64 length)
{
	String str = {length, (S8*)string, true}; 
	return str;
};

String CStringToString(char* string)
{
	U64 len = 0;
	char* str = string;
	while(*(str++)) len++;
	return CStringToString(string, len); 
}

S8 CompareStrings(String left, String right)
{
	/** 
-     1 -> right is smaller
-     0 -> equal each other
-    -1 -> left is smaller
-    -2 -> same size but not the same string 
*/
	
	if (left.length != right.length) return left.size > right.size ? -1 : 1;
	
	// Check if both have the same pointer 
	if (left.str == right.str) return 0;
	
	for(auto s : left){
		if (*s.str != right[s.index]) return -2;
	}
	return 0;
}

B8 EqualStrings(String left, String right)
{
	return CompareStrings(left, right) == 0;
}

#endif //HD_BASE_IMPL
#endif //BASE_H






