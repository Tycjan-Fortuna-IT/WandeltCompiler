project "Wandelt"
    kind "ConsoleApp"

    warnings "Everything"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir ("%{wks.location}/bin/" .. outputdir .. "/Wandelt")

    files {
        "src/**.hpp",
        "src/**.cpp"
    }

    includedirs {
        "src",
        "../WandeltCore/src",
        "../WandeltCore/modules",
        "../WandeltCore/vendor",
    }

    dependson { "WandeltCore" }
    links { "WandeltCore" }

    filter "configurations:Debug"
        IncludeDependencies("Debug")
        ProcessDependencies("Debug")

    filter "configurations:Release"
        IncludeDependencies("Release")
        ProcessDependencies("Release")

    filter "configurations:Dist"
        IncludeDependencies("Dist")
        ProcessDependencies("Dist")
