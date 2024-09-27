#include <iostream>

#include <Core/FileSystem/FileSystem.hpp>

using namespace WandeltCore;

BEGIN_CAST_FORMATTER(std::filesystem::path);
FORMATTER_CAST(std::string, value.string());
END_CAST_FORMATTER;

int main(int argc, char* argv[])
{
	const SW::LogSystemSpecification spec = {
	    .LogFileName              = "logs/wandelt.log",
	    .ConsoleSinkLoggerPattern = "%^[%T] [%n] [%l]: %v%$",
	    .FileSinkLoggerPattern    = "[%T] [%l] [%n] [%l]: %v",
	    .SystemLoggerName         = "COMPILER",
	    .AppLoggerName            = "APP",
	};

	SW::LogSystem::Initialize(spec);

	ASSERT(argc > 1, "No input file provided.");

	std::filesystem::path filepath = std::filesystem::path(*(argv + 1));

	ASSERT(FileSystem::Exists(filepath), "Input file does not exist.");

	SYSTEM_DEBUG("Input file: {}", filepath.filename());

	std::string fileContents = FileSystem::ReadFile(filepath);

	SW::LogSystem::Shutdown();

	return 0;
}
