#include "Compiler.hpp"

#include "Codegen/Codegen.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "ScopedTimer.hpp"

namespace WandeltCore
{
	Compiler::Compiler(const CompilerArguments& args) : m_Args(args)
	{
		const SW::LogSystemSpecification spec = {
		    .LogFileName              = "logs/wandelt.log",
		    .ConsoleSinkLoggerPattern = "%^[%T] [%n] [%l]: %v%$",
		    .FileSinkLoggerPattern    = "[%T] [%l] [%n] [%l]: %v",
		    .SystemLoggerName         = "COMPILER",
		    .AppLoggerName            = "APP",
		};

		SW::LogSystem::Initialize(spec);
	}

	Compiler::~Compiler()
	{
		SW::LogSystem::Shutdown();
	}

	void Compiler::Compile()
	{
		Lexer lexer(m_Args.InputFile);

		{
			ScopedTimer timer("Lexing took: {} ms, {} ns");
			lexer.Lex();
		}

		const std::vector<Token>& tokens = lexer.GetTokens();

		if (m_Args.Flags & CompilerFlags::VerboseLexer)
		{
			SYSTEM_DEBUG("Tokens: ");

			for (const Token& token : tokens)
			{
				SYSTEM_DEBUG("Token [{}]: {} at line: {} column: {}", TokenTypeToString(token.Type),
				             token.Lexeme.has_value() ? token.Lexeme.value()
				                                      : TokenTypeToStringRepresentation(token.Type),
				             token.Location.Line, token.Location.Column);
			}
		}

		if (!lexer.IsValid())
		{
			SYSTEM_ERROR("Lexing failed. Syntax errors occurred. Exiting.");

			return;
		}

		Parser parser(tokens);

		{
			ScopedTimer timer("Parsing took: {} ms, {} ns");
			parser.Parse();
		}

		std::vector<Statement*> statements = parser.GetStatements();

		if (m_Args.Flags & CompilerFlags::VerboseParser)
		{
			SYSTEM_DEBUG("Statements: ");

			for (Statement* statement : statements)
			{
				if (statement == nullptr)
				{
					SYSTEM_DEBUG("Statements is nullptr.");

					continue;
				}

				statement->Dump();
			}
		}

		if (!parser.IsValid())
		{
			SYSTEM_ERROR("Parsing failed. Syntax errors occurred. Exiting.");

			return;
		}

		Codegen codegen;
		{
			ScopedTimer timer("Generating IR took: {} ms, {} ns");
			codegen.GenerateIR(statements);
		}

		if (m_Args.Flags & CompilerFlags::VerboseCodegen)
		{
			SYSTEM_DEBUG("Generated IR: ");

			codegen.GetModuleWithGeneratedIR().print(llvm::outs(), nullptr);
		}

		int ret = 0;

		{
			ScopedTimer timer("Compiling took: {} ms, {} ns");

			ret = std::system(
			    std::vformat("clang output.ll -o {} -Wno-override-module", std::make_format_args(m_Args.OutputFile))
			        .c_str());
		}

		SYSTEM_INFO("Saved output to: {}", m_Args.OutputFile);

		if (ret != 0)
		{
			SYSTEM_ERROR("Failed to compile the generated IR. Exiting.");

			return;
		}
	}
} // namespace WandeltCore
