-- premake5.lua
workspace "HD-Project"
   location "Generated"
   architecture "x64"
   configurations { "Debug", "Release", "Tracy" }
   startproject "HDLang"

project "HDLang"
   kind "ConsoleApp"

   targetname "hd"
   language "C++"
   cppdialect "C++17"
   staticruntime "On"
      
   targetdir "bin/"
   objdir "bin-int/%{cfg.buildcfg}/"

   files {
	  "source/**.h",
	  "source/**.c",
	  "source/**.cpp"
   }


   	includedirs
	{
		"source/meow_hash"
	}

	filter  "system:windows" 
	  editandcontinue "Off"

	filter "system:linux"
	  links { "pthread", "dl" }
	  buildoptions {
	  	"-msse2",
		"-msse",
		"-march=native",
		"-maes"
	}
	
	filter "configurations:Debug"
	  defines { "DEBUG" }
	  runtime "Debug"
      symbols "On"

	filter "configurations:Tracy"
	  files {"submodules/tracy/TracyClient.cpp"}
	  
	  defines { "DEBUG", "TRACY_ENABLE", "TRACY_NO_EXIT" }
	  runtime "Release"
      symbols "On"
	  
	filter "configurations:Release"
      defines { "NDEBUG" }
	  runtime "Release"
      optimize "off"
	  

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
