-- premake5.lua
workspace "HD-Project"
   location "Generated"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "HDLang"

project "HDLang"
   kind "ConsoleApp"

   targetname "hd"
   language "C++"
   cppdialect "C++17"
   staticruntime "On"
      
   targetdir "bin/"
   objdir "bin-int/"

   files { "**.h", "**.c", "**.cpp" }


   	includedirs
	{
		"source/meow_hash"
	}

	filter  "system:windows" 
	  editandcontinue "Off"
     	
	
	filter "configurations:Debug"
	  defines { "DEBUG" }
	  runtime "Debug"
      symbols "On"

	filter "configurations:Release"
      defines { "NDEBUG" }
	  runtime "Release"
      optimize "on"
	  

-- Clean Function --
newaction {
   trigger     = "clean",
   description = "Removing project files",
   execute     = function ()
      print("clean the build...")
      os.rmdir("./bin")
	  os.rmdir("./bin-int")
	  os.rmdir("./.vs")
	  os.rmdir("./generated")
      print("done.")
   end
}
