#include "AST.hpp"

namespace WandeltCore
{
	void NumberLiteral::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "NumberLiteral: '" + std::to_string(m_Value) + "'");
	}

	void BinaryExpression::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "BinaryExpression: '" +
		             std::string(TokenTypeToStringRepresentation(m_Operator)) + "'");

		if (m_Left)
			m_Left->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");

		if (m_Right)
			m_Right->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");
	}

	void UnaryExpression::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "UnaryExpression: '" +
		             std::string(TokenTypeToStringRepresentation(m_Operator)) + "'");

		if (m_Operand)
			m_Operand->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");
	}

	void PowerExpression::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "PowerExpression: ");

		if (m_Base)
			m_Base->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");

		if (m_Exponent)
			m_Exponent->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");
	}

	void GroupingExpression::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "GroupingExpression: ");

		if (m_Expression)
			m_Expression->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");
	}

	void Scope::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "Scope: ");

		for (const auto& statement : m_Statements)
		{
			statement->Dump(indentation + 1);
		}
	}

	void IfStatement::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "IfStatement: ");
		SYSTEM_DEBUG(getIndent(indentation + 1) + "Condition: ");

		if (m_Condition)
			m_Condition->Dump(indentation + 2);
		else
			SYSTEM_DEBUG(getIndent(indentation + 2) + "nullptr");

		SYSTEM_DEBUG(getIndent(indentation + 1) + "Then: ");

		if (m_ThenScope)
			m_ThenScope->Dump(indentation + 2);
		else
			SYSTEM_DEBUG(getIndent(indentation + 2) + "nullptr");

		SYSTEM_DEBUG(getIndent(indentation + 1) + "Else: ");

		if (m_ElseScope)
			m_ElseScope->Dump(indentation + 2);
		else
			SYSTEM_DEBUG(getIndent(indentation + 2) + "nullptr");
	}

	void ReturnStatement::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "ReturnStatement: ");

		if (m_Expression)
			m_Expression->Dump(indentation + 1);
		else
			SYSTEM_DEBUG(getIndent(indentation + 1) + "nullptr");
	}

	void Declaration::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "Declaration: ");
		SYSTEM_DEBUG(getIndent(indentation + 1) + "Identifier: {}", m_Identifier);
	}

	void CallExpression::Dump(u32 indentation) const
	{
		SYSTEM_DEBUG(getIndent(indentation) + "CallExpression: ");

		m_Declaration->Dump(indentation + 1);

		SYSTEM_DEBUG(getIndent(indentation) + "Args: ");

		for (const Expression* expression : m_Args) expression->Dump(indentation + 2);
	}
} // namespace WandeltCore
