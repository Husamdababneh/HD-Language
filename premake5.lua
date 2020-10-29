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
      print("done.")
   end
}
