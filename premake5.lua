-- premake5.lua
workspace "HD-Project"
   configurations { "Debug", "Release" }

project "HDLang"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   architecture "x86_64"
   
   targetdir "bin/"
   objdir "bin-int/"

   files { "**.h", "**.c", "**.cpp" }


   	includedirs
	{
		"source/meow_hash"
	}
   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
