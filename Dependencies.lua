include "LLVM.lua"
include "OrderedPairs.lua"

-- Utility function for converting the first character to uppercase
function firstToUpper(str)
	return (str:gsub("^%l", string.upper))
end

--[[
	If you're adding a new dependency all you have to do to get it linking
	and included is define it properly in the table below, here's some example usage:

	MyDepName = {
		LibsToLink = "my_dep_name",
		LibDirs = "some_path_to_dependency_lib_dir",
		IncludeDirs = "my_include_dir",
		Windows = { DebugLibsToLink = "my_dep_name_debug" },
		Configurations = "Debug,Release"
		Defines = { "MY_DEFINE" }
	}

	MyDepName - This is just for organizational purposes, it doesn't actually matter for the build process
	LibsToLink - This is the name of the .lib file that you want to link against (you shouldn't include the .lib extension since Linux uses .a)
	LibDirs - Indicates which directory the lib file is located in, this can include "%{cfg.buildcfg}" if you have a dedicated Debug / Release directory
	IncludeDirs - Pretty self explanatory, the filepath that will be included in externalincludedirs
	Windows - This defines a platform-specific scope for this dependency, anything defined in that scope will only apply for Windows, you can also add one for Linux
	DebugLibsToLink - Use this if the .lib file has a different name in Debug builds (e.g "shaderc_sharedd" vs "shaderc_shared")
	Configurations - Defines a list of configurations that this dependency should be used in, if no Configurations is specified all configs will link and include this dependency (which is what we want in most cases)
	Defines - Defines a list of preprocessor definitions that should be added to the project when this dependency is included
		
]]--

Dependencies = {
    {
		Name = "Global defines",
        Windows = {
            Defines = {
                "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_SECURE_NO_WARNINGS",
                "_CRT_NONSTDC_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_WARNINGS",
                "_SCL_SECURE_NO_DEPRECATE",
                "_SCL_SECURE_NO_WARNINGS",
                "UNICODE",
                "_UNICODE",
                "__STDC_CONSTANT_MACROS",
                "__STDC_FORMAT_MACROS",
                "__STDC_LIMIT_MACROS",
            }
        }
    },
    {
		Name = "LLVM",
        Windows = {
            LibsToLink = llvmLibsTable,
            IncludeDirs = { llvmDir .. "/include" },
            LibDirs = { llvmDir .. "/lib" },
        },
    },
    {
		Name = "ntdll",
        Windows = {
            LibsToLink = { "ntdll.lib" },
        }
    },
	{
		Name = "SW Logger Module",
		LibsToLink = { "Logger" },
		IncludeDirs = { "%{wks.location}/WandeltCore/modules/Logger/src" },
	},
}

function LinkDependency(table, is_debug, target)

	-- Setup library directory
	if table.LibDirs ~= nil then
		libdirs { table.LibDirs }
	end

	-- Try linking
	local libsToLink = nil
	if table.LibsToLink ~= nil then
		libsToLink = table.LibsToLink
	end

	if table.DebugLibsToLink ~= nil and is_debug and target == "Windows" then
		libsToLink = table.DebugLibsToLink
	end

	if libsToLink ~= nil then
		links { libsToLink }
		return true
	end

	return false
end

function AddDependencyIncludes(table)
	if table.IncludeDirs ~= nil then
		externalincludedirs { table.IncludeDirs }
	end
end

function AddDependencyDefines(table)
    if table.Defines ~= nil then
        defines { table.Defines }
    end
end

function ProcessDependencies(config_name)
	local target = firstToUpper(os.target())

	for key, libraryData in orderedPairs(Dependencies) do

		-- Always match config_name if no Configurations list is specified
		local matchesConfiguration = true

		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end

		local isDebug = config_name == "Debug"

		if matchesConfiguration then
			local continueLink = true

			-- Process Platform Scope
			if libraryData[target] ~= nil then
				continueLink = not LinkDependency(libraryData[target], isDebug, target)
				AddDependencyIncludes(libraryData[target])
				AddDependencyDefines(libraryData[target])
			end

			-- Process Global Scope
			if continueLink then
				LinkDependency(libraryData, isDebug, target)
			end

			AddDependencyIncludes(libraryData)
		end

	end
end

function IncludeDependencies(config_name)
	local target = firstToUpper(os.target())

	for key, libraryData in orderedPairs(Dependencies) do

		-- Always match config_name if no Configurations list is specified
		local matchesConfiguration = true

		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end

		if matchesConfiguration then
			-- Process Global Scope
			AddDependencyIncludes(libraryData)

			-- Process Platform Scope
			if libraryData[target] ~= nil then
				AddDependencyIncludes(libraryData[target])
			end
		end

	end
end
