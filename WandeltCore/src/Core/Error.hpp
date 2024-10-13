/**
 * @file Error.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	struct Token;

	enum class ErrorType : u8
	{
		LEXER_ERROR,    // Error that occurred during lexing
		PARSER_ERROR,   // Error that occurred during parsing
		SEMANTIC_ERROR, // Error that occurred during semantic analysis
		CODEGEN_ERROR,  // Error that occurred during code generation
		RUNTIME_ERROR,  // Error that occurred during runtime
	};

	enum class LexerErrorCode : u8
	{
	};

	enum class ParserErrorCode : u8
	{
		MISSING_SEMICOLON,         // Missing semicolon ';' at the end of a statement
		MISSING_EXPRESSION,        // Missing expression in a statement that requires one
		MISSING_LEFT_PARENTHESIS,  // Missing left parentheses '('
		MISSING_RIGHT_PARENTHESIS, // Missing right parentheses ')'
		MISSING_LEFT_BRACE,        // Missing left brace '{'
		MISSING_RIGHT_BRACE,       // Missing right brace '}'
		MISSING_SCOPE_CLOSING,     // Missing closing scope
		UNEXPECTED_TOKEN,          // Unexpected token
	};

	enum class SemanticErrorCode : u8
	{
	};

	enum class CodegenErrorCode : u8
	{
	};

	enum class RuntimeErrorCode : u8
	{
	};

	namespace Error
	{
		std::nullptr_t ReportError(ParserErrorCode code, const Token& lastToken);
	}
} // namespace WandeltCore
