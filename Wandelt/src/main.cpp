#include <string_view>

#include <filesystem>
#include <string>

#include <Wandelt.hpp>

using namespace WandeltCore;

int main(int argc, char* argv[])
{
	const SW::LogSystemSpecification spec = {
	    .LogFileName              = "logs/wandelt.log",
	    .ConsoleSinkLoggerPattern = "%^%v%$",
	    // .ConsoleSinkLoggerPattern = "%^[%T] [%n] [%l]: %v%$",
	    .FileSinkLoggerPattern = "[%T] [%l] [%n] [%l]: %v",
	    .SystemLoggerName      = "COMPILER",
	    .AppLoggerName         = "APP",
	};

	SW::LogSystem::Initialize(spec);

	ASSERT(argc > 1, "No input file provided. Usage: Wandelt.exe <input file> <output file>");
	ASSERT(argc > 2, "No output file provided. Usage: Wandelt.exe <input file> <output file>");

	bool hasFlags = argc > 3;

	CompilerArguments args;
	args.InputFile  = *(argv + 1);
	args.OutputFile = *(argv + 2);
	args.Flags      = hasFlags ? (strcmp(*(argv + 3), "--v") == 0 ? CompilerFlags::Verbose : CompilerFlags::None)
	                           : CompilerFlags::None;

	// args.InputFile  = "../WandeltExamples/simple.wdt";
	// args.OutputFile = "output.exe";
	// args.Flags      = CompilerFlags::Verbose;

	Compiler compiler(args);

	compiler.Compile();

	SW::LogSystem::Shutdown();

	return 0;
}
