-- premake5.lua

define_table = {}
workspace "HD-Project"
	location "generated"
	architecture "x64"
	configurations { "Debug", "Release", "Tracy" }
	startproject "HDLang"

rule "MetaGeneratorWindows"
	display "Generating Files"
	fileextension ".cpp"

	buildmessage 'Generating MetaData %(Filename) With CL'
	buildcommands 'CL /EP %(FullPath) > ../generated-source/%(Filename).i'
	buildoutputs  '../bin'
	
rule "MetaGeneratorLinux"
	display "Generate Files"
	fileextension ".cpp"

	buildmessage 'Generating MetaData %(Filename) With G++'
	buildcommands 'g++ -E -P %(FullPath) > ../generated-source/%(Filename).i'
	buildoutputs  '../bin'

project "MetaProgram"
	kind "Utility"
	targetdir "bin/"

	files {
		"meta-source/enum.cpp",
		"meta-source/string.cpp"
	}

	includedirs {
		"source"
	}


	if os.host() == "windows" then 
		prebuildcommands {
			"[ -d ..\\generated-source ] || mkdir ..\\generated-source",
		}

		rules { "MetaGeneratorWindows" }

	elseif os.host() == "linux" then 
		prebuildcommands {
			"mkdir -p ..\\generated-source",
		}
		
		buildcommands { 'g++ -E -P %(FullPath) > ../generated-source/%(Filename).i' }
		rules { "MetaGeneratorLinux" }
	end

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
		"generated-source",
		"source"
	}

	links { "MetaProgram" }	

	filter  "system:windows" 
		editandcontinue "on"
	
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
		os.rmdir("./generated-source")
		os.remove("MetaGenerator.props")
		os.remove("MetaGenerator.targets")
		os.remove("MetaGenerator.xml")
		print("done.")
	end
}
