#include "Error.hpp"

#include "Core/Lexer/Token.hpp"

namespace WandeltCore::Error
{
	std::nullptr_t ReportError(ParserErrorCode code, const Token& lastToken)
	{
		const SourceLocation& location = lastToken.Location;

		const std::string_view& stringifiedToken =
		    lastToken.HasLexeme() ? lastToken.Lexeme.value() : TokenTypeToStringRepresentation(lastToken.Type);

		const u32 indicatorLocation = stringifiedToken.size() + location.Column;

		SYSTEM_ERROR("");
		SYSTEM_ERROR("[Parser] - Syntax error!");

		if (code == ParserErrorCode::MISSING_SEMICOLON)
		{
			SYSTEM_ERROR("Expected ';' after the '{}'. At line: {} column: {}.", stringifiedToken, location.Line,
			             location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_EXPRESSION)
		{
			SYSTEM_ERROR("Expected expression received '{}'. At line: {} column: {}.", stringifiedToken, location.Line,
			             location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 2, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_LEFT_PARENTHESIS)
		{
			SYSTEM_ERROR("Expected '(' at line: {} column: {}.", location.Line, location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_RIGHT_PARENTHESIS)
		{
			SYSTEM_ERROR("Expected ')' at line: {} column: {}.", location.Line, location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_LEFT_BRACE)
		{
			SYSTEM_ERROR("Expected '{{' at line: {} column: {}.", location.Line, location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_RIGHT_BRACE)
		{
			SYSTEM_ERROR("Expected '}}' at line: {} column: {}.", location.Line, location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::MISSING_SCOPE_CLOSING)
		{
			SYSTEM_ERROR("Scope not closed! Expected '}}' at line: {} column: {}.", location.Line, location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 1, 1) + "^");
		}
		else if (code == ParserErrorCode::UNEXPECTED_TOKEN)
		{
			SYSTEM_ERROR("Unexpected token '{}'. At line: {} column: {}.", stringifiedToken, location.Line,
			             location.Column);
			SYSTEM_TRACE("{}", location.CodeLine);
			SYSTEM_INFO(getIndent(indicatorLocation - 2, 1) + "^");
		}

		SYSTEM_ERROR("__________________________________________________________");
		SYSTEM_ERROR("");

		return nullptr;
	}
} // namespace WandeltCore::Error
