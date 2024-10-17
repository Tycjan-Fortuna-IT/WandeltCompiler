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
        FBS.ProcessDependencies(FBS.Configurations.Debug)

    filter "configurations:Release"
        FBS.ProcessDependencies(FBS.Configurations.Release)

    filter "configurations:Dist"
        FBS.ProcessDependencies(FBS.Configurations.Dist)
