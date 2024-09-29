#include <iostream>
#include <string_view>

#include <Core/FileSystem/FileSystem.hpp>
#include <Core/Lexer/Lexer.hpp>
#include <Core/Parser/Parser.hpp>

// #include "llvm/ADT/APFloat.h"
// #include "llvm/ADT/STLExtras.h"
// #include "llvm/IR/BasicBlock.h"
// #include "llvm/IR/Constants.h"
// #include "llvm/IR/DerivedTypes.h"
// #include "llvm/IR/Function.h"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/LLVMContext.h"
// #include "llvm/IR/Module.h"
// #include "llvm/IR/Type.h"
// #include "llvm/IR/Verifier.h"

using namespace WandeltCore;

BEGIN_CAST_FORMATTER(std::filesystem::path);
FORMATTER_CAST(std::string, value.string());
END_CAST_FORMATTER;

BEGIN_CAST_FORMATTER(std::string_view);
FORMATTER_CAST(std::string, std::string(value));
END_CAST_FORMATTER;

int main(int argc, char* argv[])
{
	// // Create LLVM context, module, and builder
	// llvm::LLVMContext context;
	// llvm::Module module("wandelt", context);
	// llvm::IRBuilder<> builder(context);

	// // Define the return type (int32) and function type (int32 main())
	// llvm::Type* int32Type            = llvm::Type::getInt32Ty(context);
	// llvm::FunctionType* functionType = llvm::FunctionType::get(int32Type, false);

	// // Create the 'main' function and its entry basic block
	// llvm::Function* mainFunction =
	//     llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, "main", module);
	// llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", mainFunction);
	// builder.SetInsertPoint(entryBlock);

	// // Return 42
	// builder.CreateRet(llvm::ConstantInt::get(int32Type, 42));

	// // Validate the generated code, checking for consistency
	// llvm::verifyFunction(*mainFunction);

	// // Output the generated LLVM IR
	// module.print(llvm::outs(), nullptr);

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
	lexer.Lex();

	std::vector<Token> tokens = lexer.GetTokens();

	SYSTEM_DEBUG("Tokens: ");

	for (const Token& token : tokens)
	{
		SYSTEM_DEBUG("Token [{}]: {} at line: {} column: {}", TokenTypeToString(token.Type),
		             token.Lexeme.has_value() ? token.Lexeme.value() : TokenTypeToStringRepresentation(token.Type),
		             token.Line, token.Column);
	}

	Parser parser(tokens);
	parser.Parse();

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

	SW::LogSystem::Shutdown();

	return 0;
}
