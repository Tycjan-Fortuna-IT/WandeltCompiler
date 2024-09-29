#include <iostream>
#include <string_view>

#include <Core/Codegen/Codegen.hpp>
#include <Core/FileSystem/FileSystem.hpp>
#include <Core/Lexer/Lexer.hpp>
#include <Core/Parser/Parser.hpp>
#include <Core/ScopedTimer.hpp>

using namespace WandeltCore;

BEGIN_CAST_FORMATTER(std::filesystem::path);
FORMATTER_CAST(std::string, value.string());
END_CAST_FORMATTER;

BEGIN_CAST_FORMATTER(std::string_view);
FORMATTER_CAST(std::string, std::string(value));
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

	Lexer lexer(fileContents);

	{
		ScopedTimer timer("Lexing took: {} ms, {} ns");
		lexer.Lex();
	}

	std::vector<Token> tokens = lexer.GetTokens();

	SYSTEM_DEBUG("Tokens: ");

	for (const Token& token : tokens)
	{
		SYSTEM_DEBUG("Token [{}]: {} at line: {} column: {}", TokenTypeToString(token.Type),
		             token.Lexeme.has_value() ? token.Lexeme.value() : TokenTypeToStringRepresentation(token.Type),
		             token.Line, token.Column);
	}

	Parser parser(tokens);

	{
		ScopedTimer timer("Parsing took: {} ms, {} ns");
		parser.Parse();
	}

	std::vector<Expression*> expressions = parser.GetExpressions();

	SYSTEM_DEBUG("Expressions: ");

	for (Expression* expression : expressions)
	{
		if (expression == nullptr)
		{
			SYSTEM_DEBUG("Expression is nullptr.");
			continue;
		}

		SYSTEM_DEBUG("Expression: {}", expression->ToString());
	}

	Codegen codegen;
	{
		ScopedTimer timer("Generating IR took: {} ms, {} ns");
		codegen.GenerateIR(expressions);
	}

	SYSTEM_DEBUG("Generated IR: ");

	codegen.GetModuleWithGeneratedIR().print(llvm::outs(), nullptr);

	int ret = 0;

	{
		ScopedTimer timer("Compiling took: {} ms, {} ns");
		ret = std::system("clang output.ll -o output.exe");
	}

	SW::LogSystem::Shutdown();

	return ret;
}
