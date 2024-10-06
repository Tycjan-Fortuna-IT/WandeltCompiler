#include <iostream>
#include <string_view>

#include <Core/Compiler.hpp>

using namespace WandeltCore;

int main(int argc, char* argv[])
{
	ASSERT(argc > 1, "No input file provided. Usage: Wandelt.exe <input file> <output file>");
	ASSERT(argc > 2, "No output file provided. Usage: Wandelt.exe <input file> <output file>");

	bool hasFlags = argc > 3;

	CompilerArguments args;
	args.InputFile  = *(argv + 1);
	args.OutputFile = *(argv + 2);
	args.Flags      = hasFlags ? (strcmp(*(argv + 3), "--v") == 0 ? CompilerFlags::Verbose : CompilerFlags::None)
	                           : CompilerFlags::None;

	Compiler compiler(args);

	compiler.Compile();

	return 0;
}
