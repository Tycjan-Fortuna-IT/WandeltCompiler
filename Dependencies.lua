include "LLVM.lua"

FBS.Configurations = FBS.Enum { "Debug", "Release", "Dist" }

FBS.Dependencies = {
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
		Name = "ntdll",
		Windows = {
			LibsToLink = { "ntdll.lib" },
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
		Name = "SW Logger Module",
		LibsToLink = { "Logger" },
		IncludeDirs = { "%{wks.location}/WandeltCore/modules/Logger/src" },
	},
}
