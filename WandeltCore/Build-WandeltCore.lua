project "WandeltCore"
    kind "StaticLib"

    warnings "Everything"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.hpp",
        "src/**.cpp",
    }

    pchheader "pch.hpp"
    pchsource "src/pch.cpp"

    forceincludes { "pch.hpp" }

    includedirs {
        "src"
    }

    filter "configurations:Debug"
        IncludeDependencies("Debug")

    filter "configurations:Release"
        IncludeDependencies("Release")

    filter "configurations:Dist"
        IncludeDependencies("Dist")

    group "Wandelt/Dependencies"
        include "modules/Logger/Build-Logger.lua"
    group ""
