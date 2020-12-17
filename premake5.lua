-- premake5.lua
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
	}

	pchheader "pch.h"
	pchsource "source/pch.cpp"

	includedirs {
		"source/meow_hash",
		"source"
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
		files {"source/include_tracy.cpp"}
		defines { "TRACY_ENABLE", "TRACY_NO_EXIT" }
		runtime "Release"
		symbols "off"

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
