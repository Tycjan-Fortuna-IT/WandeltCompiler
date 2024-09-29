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
		case TokenType::LEFT_PARENTHESES:
			return "LEFT_PARENTHESES";
		case TokenType::RIGHT_PARENTHESES:
			return "RIGHT_PARENTHESES";
		case TokenType::LEFT_BRACE:
			return "LEFT_BRACE";
		case TokenType::RIGHT_BRACE:
			return "RIGHT_BRACE";
		case TokenType::EQUALS:
			return "EQUALS";
		case TokenType::PLUS:
			return "PLUS";
		case TokenType::MINUS:
			return "MINUS";
		case TokenType::STAR:
			return "STAR";
		case TokenType::SLASH:
			return "SLASH";
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
		case TokenType::LEFT_PARENTHESES:
			return "(";
		case TokenType::RIGHT_PARENTHESES:
			return ")";
		case TokenType::LEFT_BRACE:
			return "{";
		case TokenType::RIGHT_BRACE:
			return "}";
		case TokenType::EQUALS:
			return "=";
		case TokenType::PLUS:
			return "+";
		case TokenType::MINUS:
			return "-";
		case TokenType::STAR:
			return "*";
		case TokenType::SLASH:
			return "/";
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
