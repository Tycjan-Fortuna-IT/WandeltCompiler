/**
 * @file Compiler.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	enum CompilerFlags : u32
	{
		None           = 0,                                             // No flags
		VerboseLexer   = 1 << 0,                                        // Display lexer output
		VerboseParser  = 1 << 1,                                        // Display parser output
		VerboseCodegen = 1 << 2,                                        // Display codegen output
		Verbose        = VerboseLexer | VerboseParser | VerboseCodegen, // Display all output
	};

	struct CompilerArguments
	{
		std::filesystem::path InputFile;
		std::filesystem::path OutputFile;
		CompilerFlags Flags = CompilerFlags::None;
	};

	class Compiler
	{
	public:
		Compiler(const CompilerArguments& args);
		~Compiler();

		void Compile();

	private:
		CompilerArguments m_Args;
	};
} // namespace WandeltCore
