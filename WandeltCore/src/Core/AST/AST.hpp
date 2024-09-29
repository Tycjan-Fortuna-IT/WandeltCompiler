/**
 * @file AST.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#include "Core/Lexer/Token.hpp"

namespace WandeltCore
{
	class Expression
	{
	public:
		virtual ~Expression() = default;

		virtual std::string ToString() const = 0;
	};

	class NumberLiteral : public Expression
	{
	public:
		NumberLiteral(i32 value) : m_Value(value) {}

		std::string ToString() const override { return "NumberLiteral: " + std::to_string(m_Value); }

		i32 GetValue() const { return m_Value; }

	private:
		i32 m_Value;
	};

	class BinaryExpression : public Expression
	{
	public:
		BinaryExpression(Expression* left, Expression* right, TokenType op)
		    : m_Left(left), m_Right(right), m_Operator(op)
		{
		}
		~BinaryExpression()
		{
			delete m_Left;
			delete m_Right;
		}

		std::string ToString() const override
		{
			return "BinaryExpression: " + m_Left->ToString() + " " + std::string(TokenTypeToString(m_Operator)) + " " +
			       m_Right->ToString();
		}

		Expression* GetLeft() const { return m_Left; }
		Expression* GetRight() const { return m_Right; }

		TokenType GetOperator() const { return m_Operator; }

	private:
		Expression* m_Left;
		Expression* m_Right;

		TokenType m_Operator;
	};

	class GroupingExpression : public Expression
	{
	public:
		GroupingExpression(Expression* expression) : m_Expression(expression) {}
		~GroupingExpression() { delete m_Expression; }

		std::string ToString() const override { return "GroupingExpression: " + m_Expression->ToString(); }

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression;
	};

	class ReturnStatement : public Expression
	{
	public:
		ReturnStatement(Expression* expression) : m_Expression(expression) {}
		~ReturnStatement() { delete m_Expression; }

		std::string ToString() const override
		{
			if (m_Expression == nullptr)
			{
				return "ReturnStatement: empty";
			}

			return "ReturnStatement: " + m_Expression->ToString();
		}

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression;
	};
} // namespace WandeltCore
