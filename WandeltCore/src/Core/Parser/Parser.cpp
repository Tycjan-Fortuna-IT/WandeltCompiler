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
		while (!IsAtEnd())
		{
			if (m_Tokens.at(m_Current).Type == TokenType::RETURN_KEYWORD)
			{
				Expression* expr = ParseReturnStatement();

				if (!expr)
				{
					SynchronizeAfterError();
					continue;
				}

				m_Expressions.push_back(expr);
			}

			SYSTEM_DEBUG("LOOP");
		}
	}

	void Parser::SynchronizeAfterError()
	{
		EatCurrentToken();

		while (!IsAtEnd())
		{
			if (GetPreviousToken().Type == TokenType::SEMICOLON)
				return;

			const Token& token = GetNextToken();

			switch (token.Type)
			{
			case TokenType::RETURN_KEYWORD:
				return;
			default:
				EatCurrentToken();
			}
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

			if (!IsExpressionOperator(token.Type))
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
