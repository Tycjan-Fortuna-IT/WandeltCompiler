#include "Parser.hpp"

#include <iostream>

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

				continue;
			}

			const Token& token = GetAndEatCurrentToken();
			if (token.Type == TokenType::END_OF_FILE)
				break;

			SYSTEM_ERROR("Unexpected token: {} at line: {} column: {}",
			             token.Lexeme.has_value() ? token.Lexeme.value() : TokenTypeToStringRepresentation(token.Type),
			             token.Location.Line, token.Location.Column);

			m_IsValid = false;
		}
	}

	void Parser::SynchronizeAfterError()
	{
		// SYSTEM_ERROR("Synchronizing parser after error.");
		m_IsValid = false;

		while (!IsAtEnd())
		{
			if (GetNextToken().Type == TokenType::SEMICOLON)
				return;

			const Token& token = GetCurrentToken();

			switch (token.Type)
			{
			case TokenType::RETURN_KEYWORD:
				return;
			default:
				EatCurrentToken();
				break;
			}
		}
	}

	i32 Parser::GetTokenPrecedence(TokenType type) const
	{
		switch (type)
		{
		case TokenType::DOUBLE_STAR:
		case TokenType::PERCENT:
			return 3;
		case TokenType::STAR:
		case TokenType::SLASH:
			return 2;

		case TokenType::PLUS:
		case TokenType::MINUS:
			return 1;
		default:
			return -1;
		}
	}

	Expression* Parser::ParseLiteral()
	{
		const Token& token = GetCurrentToken();

		if (token.Type == TokenType::LEFT_PARENTHESES)
		{
			EatCurrentToken(); // eat the left parentheses

			Expression* expr = ParseExpression();

			if (GetCurrentToken().Type != TokenType::RIGHT_PARENTHESES)
			{
				SYSTEM_ERROR("Expected ')' at line: {} column: {}.", token.Location.Line, token.Location.Column);

				return nullptr;
			}

			EatCurrentToken(); // eat the right parentheses

			return new GroupingExpression(expr);
		}

		if (token.Type == TokenType::NUMBER)
		{
			EatCurrentToken();

			return new NumberLiteral(std::stoi(token.Lexeme.value()));
		}

		SYSTEM_ERROR("Expected expression at line: {} column: {}. Received: {}.", token.Location.Line,
		             token.Location.Column, TokenTypeToString(token.Type));

		return nullptr;
	}

	Expression* Parser::ParsePrefixExpression()
	{
		const Token& token = GetCurrentToken();

		if (token.Type == TokenType::MINUS)
		{
			EatCurrentToken();

			Expression* expr = valueOrReturnNullptr(ParseLiteral());

			return new UnaryExpression(expr, token.Type);
		}

		return ParseLiteral();
	}

	Expression* Parser::ParseExpression()
	{
		Expression* lhs = valueOrReturnNullptr(ParsePrefixExpression());

		return ParseExpressionRHS(lhs, 0);
	}

	Expression* Parser::ParseExpressionRHS(Expression*& lhs, i32 precedence)
	{
		while (true)
		{
			const Token& token  = GetCurrentToken(); // should be an operator
			i32 tokenPrecedence = GetTokenPrecedence(token.Type);

			if (!IsExpressionOperator(token.Type))
				return lhs;

			EatCurrentToken();

			Expression* rhs = valueOrReturnNullptr(ParseLiteral());

			if (tokenPrecedence < GetTokenPrecedence(GetCurrentToken().Type))
			{
				rhs = valueOrReturnNullptr(ParseExpressionRHS(rhs, tokenPrecedence + 1));
			}

			if (token.Type == TokenType::DOUBLE_STAR)
				lhs = new PowerExpression(lhs, rhs);
			else
				lhs = new BinaryExpression(lhs, rhs, token.Type);
		}

		return lhs;
	}

	Expression* Parser::ParseReturnStatement()
	{
		EatCurrentToken(); // eat the return keyword
		const Token& token = GetCurrentToken();

		if (token.Type != TokenType::SEMICOLON)
		{
			Expression* expr = valueOrReturnNullptr(ParseExpression());

			const Token& afterToken = GetCurrentToken(); // should be a semicolon

			if (afterToken.Type != TokenType::SEMICOLON)
			{
				SYSTEM_ERROR("Expected ';' after the return keyword. At line: {} column: {}.", afterToken.Location.Line,
				             afterToken.Location.Column);

				return nullptr;
			}

			EatCurrentToken();

			return new ReturnStatement(expr);
		}

		SYSTEM_ERROR("Expected ';' after the return keyword. At line: {} column: {}.", token.Location.Line,
		             token.Location.Column);

		return nullptr;
	}
} // namespace WandeltCore
