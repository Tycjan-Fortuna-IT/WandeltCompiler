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
	class Dumpable
	{
	public:
		virtual void Dump(u32 indentation = 0) const = 0;
	};

	class Statement : public Dumpable
	{
	public:
		explicit Statement(const SourceLocation& location) : m_Location(location) {}
		virtual ~Statement() = default;

		const SourceLocation& GetLocation() const { return m_Location; }

		virtual llvm::Value* GenerateExpression(Visitor* visitor) = 0;

	private:
		SourceLocation m_Location;
	};

	class Expression : public Statement
	{
	public:
		explicit Expression(const SourceLocation& location) : Statement(location) {}
	};

	class NumberLiteral : public Expression
	{
	public:
		explicit NumberLiteral(const SourceLocation& location, i32 value) : Expression(location), m_Value(value) {}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateNumberLiteral(this); }

		void Dump(u32 indentation = 0) const override;

		i32 GetValue() const { return m_Value; }

	private:
		i32 m_Value;
	};

	class BinaryExpression : public Expression
	{
	public:
		explicit BinaryExpression(const SourceLocation& location, Expression* left, Expression* right, TokenType op)
		    : Expression(location), m_Left(left), m_Right(right), m_Operator(op)
		{
		}
		~BinaryExpression() override
		{
			delete m_Left;
			delete m_Right;
		}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateBinaryExpression(this); }

		void Dump(u32 indentation = 0) const override;

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
		explicit UnaryExpression(const SourceLocation& location, Expression* operand, TokenType op)
		    : Expression(location), m_Operand(operand), m_Operator(op)
		{
		}
		~UnaryExpression() override { delete m_Operand; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateUnaryExpression(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetOperand() const { return m_Operand; }

		TokenType GetOperator() const { return m_Operator; }

	private:
		Expression* m_Operand = nullptr;
		TokenType m_Operator;
	};

	class PowerExpression : public Expression
	{
	public:
		explicit PowerExpression(const SourceLocation& location, Expression* base, Expression* exponent)
		    : Expression(location), m_Base(base), m_Exponent(exponent)
		{
		}
		~PowerExpression() override
		{
			delete m_Base;
			delete m_Exponent;
		}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GeneratePowerExpression(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetBase() const { return m_Base; }
		Expression* GetExponent() const { return m_Exponent; }

	private:
		Expression* m_Base     = nullptr;
		Expression* m_Exponent = nullptr;
	};

	class GroupingExpression : public Expression
	{
	public:
		explicit GroupingExpression(const SourceLocation& location, Expression* expression)
		    : Expression(location), m_Expression(expression)
		{
		}
		~GroupingExpression() override { delete m_Expression; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateGroupingExpression(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression = nullptr;
	};

	class Scope : public Dumpable
	{
	public:
		explicit Scope(const SourceLocation& location, const std::vector<Statement*>& statements)
		    : m_Location(location), m_Statements(statements)
		{
		}

		const SourceLocation& GetLocation() const { return m_Location; }
		const std::vector<Statement*>& GetStatements() const { return m_Statements; }

		void Dump(u32 indentation = 0) const override;

	private:
		SourceLocation m_Location;
		std::vector<Statement*> m_Statements;
	};

	class IfStatement : public Statement
	{
	public:
		IfStatement(const SourceLocation& location, Expression* condition, Scope* thenScope, Scope* elseScope)
		    : Statement(location), m_Condition(condition), m_ThenScope(thenScope), m_ElseScope(elseScope)
		{
		}
		~IfStatement() override
		{
			delete m_Condition;
			delete m_ThenScope;
			delete m_ElseScope;
		}

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateIfStatement(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetCondition() const { return m_Condition; }
		Scope* GetThenScope() const { return m_ThenScope; }
		Scope* GetElseScope() const { return m_ElseScope; }

	private:
		Expression* m_Condition = nullptr;
		Scope* m_ThenScope      = nullptr;
		Scope* m_ElseScope      = nullptr;
	};

	class ReturnStatement : public Statement
	{
	public:
		explicit ReturnStatement(const SourceLocation& location, Expression* expression)
		    : Statement(location), m_Expression(expression)
		{
		}
		~ReturnStatement() override { delete m_Expression; }

		llvm::Value* GenerateExpression(Visitor* visitor) override { return visitor->GenerateReturnStatement(this); }

		void Dump(u32 indentation = 0) const override;

		Expression* GetExpression() const { return m_Expression; }

	private:
		Expression* m_Expression = nullptr;
	};
} // namespace WandeltCore
