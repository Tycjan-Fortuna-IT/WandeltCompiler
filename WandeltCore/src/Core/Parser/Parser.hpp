/**
 * @file Parser.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#include "Core/AST/AST.hpp"
#include "Core/Lexer/Token.hpp"

namespace WandeltCore
{
	class Parser
	{
	public:
		Parser(const std::vector<Token>& tokens);
		~Parser();

		void Parse();

		const std::vector<Expression*>& GetExpressions() const { return m_Expressions; }

	private:
		// Synchronize the parser after an error has occurred to prevent cascading errors.
		void SynchronizeAfterError();

		// Whether the token is an operator, meaning it can be used in an expression
		// to perform an operation like addition.
		bool IsExpressionOperator(TokenType type) const { return type == TokenType::PLUS; }

		// Check if we are at the end of the source file
		bool IsAtEnd() const { return m_Tokens.at(m_Current).Type == TokenType::END_OF_FILE; }

		const Token& GetPreviousToken() const { return m_Tokens.at(m_Current - 1); }
		const Token& GetCurrentToken() const { return m_Tokens.at(m_Current); }
		const Token& GetAndEatCurrentToken() { return m_Tokens.at(m_Current++); }
		const Token& GetNextToken() const { return m_Tokens.at(m_Current + 1); }

		void EatCurrentToken() { m_Current++; }

		Expression* ParseLiteral();

		Expression* ParsePrefixExpression();
		Expression* ParseExpression();
		Expression* ParseExpressionRHS(Expression*& lhs); // TODO precedence

		Expression* ParseReturnStatement();

	private:
		i32 m_Current = 0;

		std::vector<Token> m_Tokens;
		std::vector<Expression*> m_Expressions;
	};
} // namespace WandeltCore
