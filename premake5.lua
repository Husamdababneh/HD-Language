-- premake5.lua

define_table = {}
workspace "HD-Project"
	location "generated"
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

	excludes {
		"source/extra/graph.cpp",
		"source/extra/graph.h",
		"source/generator.cpp"
	}

	pchheader "pch.h"
	pchsource "source/pch.cpp"

	includedirs {
		"source/meow_hash",
		"source"
	}


	filter  "system:windows" 
		editandcontinue "on"
		--buildoptions {
		--	"/GF"
		--}
	

	filter "system:linux"
		links { "pthread", "dl" }
		buildoptions {
		"-msse2",
		"-msse",
		"-march=native",
		"-maes"
	}

	table.insert(define_table, "NOMINMAX")

	filter "configurations:Debug"
		table.insert(define_table, "DEBUG")
		runtime "Debug"
		symbols "On"

	filter "configurations:Tracy"
		table.insert(define_table, { "TRACY_ENABLE","TRACY_NO_EXIT"} )
		files {"submodules/tracy/TracyClient.cpp" }
		runtime "Release"
		symbols "off"

	filter "configurations:Release"
		table.insert(define_table, { "NDEBUG"} )
		runtime "Release"
		optimize "off"

	filter "files:**TracyClient.cpp"
	    flags {"NoPCH"}
	
	filter "files:**stb_*.cpp"
	    flags {"NoPCH"}

	defines { define_table } 
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
