#include "Parser.hpp"

#include <iostream>

namespace WandeltCore
{
	Parser::Parser(const std::vector<Token>& tokens) : m_Tokens(tokens)
	{
	}

	Parser::~Parser()
	{
		for (Statement* statement : m_Statements) delete statement;
	}

	void Parser::Parse()
	{
		while (!IsAtEnd())
		{
			const TokenType type = GetCurrentToken().Type;

			// SYSTEM_WARN("Parsing token: {}", TokenTypeToString(type));

			if (type == TokenType::IF_KEYWORD)
			{
				// Parse if statement
				Statement* ifStatement = ParseIfStatement();
				if (!ifStatement)
				{
					SynchronizeAfterError();
					continue;
				}

				m_Statements.push_back(ifStatement);

				continue;
			}
			else if (type == TokenType::RETURN_KEYWORD)
			{
				Statement* expr = ParseReturnStatement();
				if (!expr)
				{
					SynchronizeAfterError();
					continue;
				}

				m_Statements.push_back(expr);

				continue;
			}

			const Token& token = GetAndEatCurrentToken();
			if (token.Type == TokenType::END_OF_FILE)
				break;

			SYSTEM_ERROR("[Parser] Unexpected token: {} at line: {} column: {}",
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
			return 6;
		case TokenType::STAR:
		case TokenType::SLASH:
			return 5;
		case TokenType::PLUS:
		case TokenType::MINUS:
			return 4;
		case TokenType::LESS:
		case TokenType::LESS_EQUAL:
		case TokenType::GREATER:
		case TokenType::GREATER_EQUAL:
			return 3;
		case TokenType::EQUAL_EQUAL:
		case TokenType::BANG_EQUAL:
			return 2;
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

			return new GroupingExpression(token.Location, expr);
		}

		if (token.Type == TokenType::NUMBER)
		{
			EatCurrentToken();

			return new NumberLiteral(token.Location, std::stoi(token.Lexeme.value()));
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

			valueOrReturnNullptr(Expression*, expr, ParseLiteral());

			return new UnaryExpression(token.Location, expr, token.Type);
		}

		return ParseLiteral();
	}

	Expression* Parser::ParseExpression()
	{
		valueOrReturnNullptr(Expression*, lhs, ParsePrefixExpression());

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

			valueOrReturnNullptr(Expression*, rhs, ParseLiteral());

			if (tokenPrecedence < GetTokenPrecedence(GetCurrentToken().Type))
			{
				rhs = ParseExpressionRHS(rhs, tokenPrecedence + 1);
			}

			if (token.Type == TokenType::DOUBLE_STAR)
				lhs = new PowerExpression(token.Location, lhs, rhs);
			else
				lhs = new BinaryExpression(token.Location, lhs, rhs, token.Type);
		}

		return lhs;
	}

	Scope* Parser::ParseScope()
	{
		const Token& token = GetAndEatCurrentToken(); // eat the left brace

		std::vector<Statement*> statements;

		while (true)
		{
			const Token& currentToken = GetCurrentToken();

			if (currentToken.Type == TokenType::RIGHT_BRACE)
				break;

			if (currentToken.Type == TokenType::END_OF_FILE)
			{
				SYSTEM_ERROR("Expected '}' at line: {} column: {}.", currentToken.Location.Line,
				             currentToken.Location.Column);
				return nullptr;
			}

			Statement* statement = ParseStatement();
			if (!statement)
			{
				SynchronizeAfterError();
				continue;
			}

			statements.push_back(statement);
		}

		EatCurrentToken(); // eat the right brace

		return new Scope(token.Location, statements);
	}

	Statement* Parser::ParseStatement()
	{
		const Token& token = GetCurrentToken();

		if (token.Type == TokenType::IF_KEYWORD)
		{
			return ParseIfStatement();
		}
		else if (token.Type == TokenType::RETURN_KEYWORD)
		{
			return ParseReturnStatement();
		}

		SYSTEM_ERROR("Unexpected token: {} at line: {} column: {}",
		             token.Lexeme.has_value() ? token.Lexeme.value() : TokenTypeToStringRepresentation(token.Type),
		             token.Location.Line, token.Location.Column);

		return nullptr;
	}

	Statement* Parser::ParseIfStatement()
	{
		const Token& token = GetAndEatCurrentToken(); // eat the if keyword

		valueOrReturnNullptr(Expression*, condition, ParseExpression());

		valueOrReturnNullptr(Scope*, trueScope, ParseScope());

		if (GetCurrentToken().Type != TokenType::ELSE_KEYWORD)
			return new IfStatement(token.Location, condition, trueScope, nullptr);

		EatCurrentToken(); // eat the else keyword

		Scope* falseScope = nullptr;

		const Token& nextToken = GetCurrentToken();

		if (nextToken.Type == TokenType::IF_KEYWORD)
		{
			// else if
			valueOrReturnNullptr(Statement*, elseIf, ParseIfStatement());

			falseScope = new Scope(nextToken.Location, {elseIf});
		}
		else
		{
			// just else
			falseScope = ParseScope();
		}

		if (!falseScope)
			return nullptr;

		return new IfStatement(token.Location, condition, trueScope, falseScope);
	}

	Statement* Parser::ParseReturnStatement()
	{
		EatCurrentToken(); // eat the return keyword
		const Token& token = GetCurrentToken();

		if (token.Type != TokenType::SEMICOLON)
		{
			valueOrReturnNullptr(Expression*, expr, ParseExpression());

			const Token& afterToken = GetCurrentToken(); // should be a semicolon

			if (afterToken.Type != TokenType::SEMICOLON)
			{
				SYSTEM_ERROR("Expected ';' after the return keyword. At line: {} column: {}.", afterToken.Location.Line,
				             afterToken.Location.Column);

				return nullptr;
			}

			EatCurrentToken();

			return new ReturnStatement(token.Location, expr);
		}

		SYSTEM_ERROR("Expected ';' after the return keyword. At line: {} column: {}.", token.Location.Line,
		             token.Location.Column);

		return nullptr;
	}
} // namespace WandeltCore
