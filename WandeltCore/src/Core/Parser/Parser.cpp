#include "Parser.hpp"

namespace WandeltCore
{
	Parser::Parser(const std::vector<Token>& tokens) : m_Tokens(tokens)
	{
	}

	Parser::~Parser()
	{
		for (Expression* expression : m_Expressions) delete expression;
	}

	void Parser::Parse()
	{
		// return

		while (m_Tokens.at(m_Current).Type != TokenType::END_OF_FILE)
		{
			if (m_Tokens.at(m_Current).Type == TokenType::RETURN_KEYWORD)
			{
				m_Expressions.push_back(ParseReturnStatement());
			}

			SYSTEM_DEBUG("LOOP");
		}
	}

	Expression* Parser::ParseLiteral()
	{
		const Token& token = GetCurrentToken();

		if (token.Type == TokenType::NUMBER)
		{
			EatCurrentToken();

			return new NumberLiteral(std::stoi(token.Lexeme.value()));
		}

		SYSTEM_ERROR("Expected expression at line: {} column: {}", token.Line, token.Column);

		return nullptr;
	}

	Expression* Parser::ParsePrefixExpression()
	{
		// for future handling of unary operators

		return ParseLiteral();
	}

	Expression* Parser::ParseExpression()
	{
		Expression* lhs = ParsePrefixExpression();

		if (!lhs)
			return nullptr;

		return ParseExpressionRHS(lhs);
	}

	Expression* Parser::ParseExpressionRHS(Expression*& lhs)
	{
		while (true)
		{
			const Token& token = GetCurrentToken();

			if (!IsOperator(token.Type))
				return lhs;

			EatCurrentToken();

			Expression* rhs = ParseLiteral();

			if (!rhs)
				return nullptr;

			lhs = new BinaryExpression(lhs, rhs, token.Type);
		}

		return lhs;
	}

	Expression* Parser::ParseReturnStatement()
	{
		EatCurrentToken(); // Eat return keyword

		if (GetCurrentToken().Type != TokenType::SEMICOLON)
		{
			Expression* expr = ParseExpression();

			if (GetCurrentToken().Type != TokenType::SEMICOLON)
			{
				SYSTEM_ERROR("Expected ';' after the return statement.");

				return nullptr;
			}

			EatCurrentToken();

			return new ReturnStatement(expr);
		}

		SYSTEM_ERROR("Expected ';' after the return statement.");

		return nullptr;
	}
} // namespace WandeltCore
