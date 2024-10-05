/**
 * @file AST.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#include "Core/AST/Visitor.hpp"
#include "Core/Lexer/Token.hpp"

namespace WandeltCore
{
	// Statement
	//  Expression
	//    NumberLiteral
	//    BinaryExpression
	//    UnaryExpression
	//    PowerExpression
	//    GroupingExpression
	//  Type
	//  ReturnStatement
	//  IfStatement
	//  DeclarationStatement
	//   VariableDeclaration
	//   FunctionDeclaration
	class Statement
	{
	public:
		Statement(const SourceLocation& location) : m_Location(location) {}
		virtual ~Statement() = default;

		const SourceLocation& GetLocation() const { return m_Location; }

		virtual llvm::Value* GenerateExpression(Visitor* visitor) = 0;

		virtual std::string ToString(u32 indentation = 0) const = 0;

	private:
		SourceLocation m_Location;
	};

	class Expression
	{
	public:
		virtual ~Expression() = default;

		virtual llvm::Value* GenerateExpression(Visitor* visitor) = 0;

		virtual void Dump(u32 indentation = 0) const = 0;
	};

	class NumberLiteral : public Expression
	{
	public:
		explicit NumberLiteral(i32 value) : m_Value(value) {}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateNumberLiteral(this); }

		void Dump(u32 indentation = 0) const override;

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
		~BinaryExpression() override
		{
			delete m_Left;
			delete m_Right;
		}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateBinaryExpression(this); }

		void Dump(u32 indentation = 0) const override;
		// {
		// 	return "BinaryExpression: " + m_Left->ToString() + " " + std::string(TokenTypeToString(m_Operator)) + " " +
		// 	       m_Right->ToString();
		// }

		Expression* GetLeft() const { return m_Left; }
		Expression* GetRight() const { return m_Right; }

		TokenType GetOperator() const { return m_Operator; }

	private:
		Expression* m_Left  = nullptr;
		Expression* m_Right = nullptr;

		TokenType m_Operator;
	};

	class UnaryExpression : public Expression
	{
	public:
		UnaryExpression(Expression* operand, TokenType op) : m_Operand(operand), m_Operator(op) {}
		~UnaryExpression() override { delete m_Operand; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateUnaryExpression(this); }

		void Dump(u32 indentation = 0) const override;
		// {
		// 	return "UnaryExpression: " + std::string(TokenTypeToString(m_Operator)) + " " + m_Operand->ToString();
		// }

		Expression* GetOperand() const { return m_Operand; }

		TokenType GetOperator() const { return m_Operator; }

	private:
		Expression* m_Operand = nullptr;
		TokenType m_Operator;
	};

	class PowerExpression : public Expression
	{
	public:
		PowerExpression(Expression* base, Expression* exponent) : m_Base(base), m_Exponent(exponent) {}
		~PowerExpression() override
		{
			delete m_Base;
			delete m_Exponent;
		}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GeneratePowerExpression(this); }

		void Dump(u32 indentation = 0) const override;
		// {
		// 	return "PowerExpression: " + m_Base->ToString() + " ** " + m_Exponent->ToString();
		// }

		Expression* GetBase() const { return m_Base; }
		Expression* GetExponent() const { return m_Exponent; }

	private:
		Expression* m_Base     = nullptr;
		Expression* m_Exponent = nullptr;
	};

	class GroupingExpression : public Expression
	{
	public:
		explicit GroupingExpression(Expression* expression) : m_Expression(expression) {}
		~GroupingExpression() override { delete m_Expression; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateGroupingExpression(this); }

		void Dump(u32 indentation = 0) const override;
		// {
		// 	return "GroupingExpression: " + m_Expression->ToString();
		// }

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression = nullptr;
	};

	class ReturnStatement : public Expression
	{
	public:
		explicit ReturnStatement(Expression* expression) : m_Expression(expression) {}
		~ReturnStatement() override { delete m_Expression; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateReturnStatement(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression = nullptr;
	};
} // namespace WandeltCore
