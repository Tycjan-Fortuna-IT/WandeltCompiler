/**
 * @file Codegen.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "Core/AST/AST.hpp"

namespace WandeltCore
{
	class Codegen
	{
	public:
		Codegen();

		void GenerateIR(const std::vector<Expression*>& expressions);

		const llvm::Module& GetModuleWithGeneratedIR() const { return m_Module; }

	private:
		void GenerateEntrypoint();

		llvm::Value* GenerateExpression(Expression* expression);
		llvm::Value* GenerateBinaryExpression(BinaryExpression* binaryExpression);
		llvm::Value* GeneratePowerExpression(PowerExpression* powerExpression);

		void GenerateReturnStatement(ReturnStatement* returnStatement);

	private:
		llvm::LLVMContext m_Context;
		llvm::IRBuilder<> m_Builder;
		llvm::Module m_Module;
	};
} // namespace WandeltCore
