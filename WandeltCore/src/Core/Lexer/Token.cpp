#include "Token.hpp"

namespace WandeltCore
{
	std::string_view TokenTypeToString(TokenType type)
	{
		switch (type)
		{
		case TokenType::NUMBER:
			return "NUMBER";
		case TokenType::LET_KEYWORD:
			return "LET_KEYWORD";
		case TokenType::RETURN_KEYWORD:
			return "RETURN_KEYWORD";
		case TokenType::EQUALS:
			return "EQUALS";
		case TokenType::SEMICOLON:
			return "SEMICOLON";
		case TokenType::IDENTIFIER:
			return "IDENTIFIER";
		case TokenType::END_OF_FILE:
			return "END_OF_FILE";
		default:
			ASSERT(false, "Unknown token type.");
			return "UNKNOWN";
		}
	}

	std::string_view TokenTypeToStringRepresentation(TokenType type)
	{
		switch (type)
		{
		case TokenType::LET_KEYWORD:
			return "let";
		case TokenType::RETURN_KEYWORD:
			return "return";
		case TokenType::EQUALS:
			return "=";
		case TokenType::SEMICOLON:
			return ";";
		case TokenType::END_OF_FILE:
			return "EOF";
		default:
			ASSERT(false, "No representation for token type.");
			return "UNKNOWN";
		}
	}
} // namespace WandeltCore
