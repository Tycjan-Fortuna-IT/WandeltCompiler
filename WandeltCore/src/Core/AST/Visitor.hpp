/**
 * @file Visitor.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-10-05
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace llvm
{
	class Value;
}

namespace WandeltCore
{
	class Visitor
	{
	public:
		virtual llvm::Value* GenerateNumberLiteral(class NumberLiteral* numberLiteral)                = 0;
		virtual llvm::Value* GenerateBinaryExpression(class BinaryExpression* binaryExpression)       = 0;
		virtual llvm::Value* GenerateUnaryExpression(class UnaryExpression* unaryExpression)          = 0;
		virtual llvm::Value* GeneratePowerExpression(class PowerExpression* powerExpression)          = 0;
		virtual llvm::Value* GenerateGroupingExpression(class GroupingExpression* groupingExpression) = 0;
		virtual llvm::Value* GenerateCallExpression(class CallExpression* callExpression)             = 0;
		virtual llvm::Value* GenerateDeclaration(class Declaration* declaration)                      = 0;
		virtual llvm::Value* GenerateIfStatement(class IfStatement* ifStatement)                      = 0;
		virtual llvm::Value* GenerateReturnStatement(class ReturnStatement* returnStatement)          = 0;
	};

} // namespace WandeltCore
