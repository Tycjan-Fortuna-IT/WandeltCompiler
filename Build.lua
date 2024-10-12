include "SolutionItems.lua"
include "Dependencies.lua"

workspace "WandeltLanguage"
    configurations { "Debug", "Release", "Dist" }
    startproject "Wandelt"
    conformancemode "On"
    language "C++"
	cppdialect "C++20"
	staticruntime "Off"

    architecture "x86_64"

    externalanglebrackets "On"
    externalwarnings "Off"
    warnings "Off"

    -- using all warnings with those exceptions:
    -- they are disabled because they are not relevant for the project and are too noisy
    disablewarnings {

    }

    defines {

    }

    -- Include common solution items
    solution_items { ".clang-format", ".clang-tidy" }

    flags { "MultiProcessorCompile" }

    filter "action:vs*"
        buildoptions { "/wd4710", "/wd4711" }

    filter "system:windows"
        defines { "SW_PLATFORM_WINDOWS" }
        systemversion "latest"

    filter "configurations:Debug"
        defines { "SW_DEBUG_BUILD" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "SW_RELEASE_BUILD" }
        runtime "Release"
        optimize "Off"
        symbols "On"

    filter "configurations:Dist"
        defines { "SW_DIST_BUILD" }
        runtime "Release"
        optimize "On"
        symbols "Off"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Wandelt"
	include "WandeltCore/Build-WandeltCore.lua"

group "Runtime"
    include "Wandelt/Build-Wandelt.lua"
