/**
 * @file Token.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	enum class TokenType : u8
	{
		// Types
		NUMBER,

		// Keywords
		LET_KEYWORD,    // let
		RETURN_KEYWORD, // return

		// Braces
		LEFT_PARENTHESES,  // (
		RIGHT_PARENTHESES, // )
		LEFT_BRACE,        // {
		RIGHT_BRACE,       // }

		// Operators
		EQUALS,      // =
		PLUS,        // +
		MINUS,       // -
		STAR,        // *
		DOUBLE_STAR, // **
		SLASH,       // /
		PERCENT,     // %

		// Punctuation
		SEMICOLON, // ;

		// Other
		IDENTIFIER,
		END_OF_FILE,
	};

	// Returns the name of the token type. e.g. TokenType::NUMBER -> "NUMBER"
	std::string_view TokenTypeToString(TokenType type);

	// Returns the string representation of the token type. e.g. TokenType::SEMICOLON -> ";"
	std::string_view TokenTypeToStringRepresentation(TokenType type);

	struct Token
	{
		TokenType Type;
		std::optional<std::string> Lexeme = std::nullopt;
		u32 Line;
		u32 Column;
	};

	static const std::unordered_map<std::string_view, TokenType> Keywords = {{"let", TokenType::LET_KEYWORD},
	                                                                         {"return", TokenType::RETURN_KEYWORD}};
} // namespace WandeltCore
