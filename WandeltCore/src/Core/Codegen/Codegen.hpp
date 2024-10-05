/**
 * @file Codegen.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

// #include <llvm/ADT/APFloat.h>
// #include <llvm/ADT/STLExtras.h>
// #include <llvm/IR/BasicBlock.h>
// #include <llvm/IR/Constants.h>
// #include <llvm/IR/DerivedTypes.h>
// #include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
// #include <llvm/IR/PassManager.h>
// #include <llvm/IR/Verifier.h>
// #include <llvm/Passes/PassBuilder.h>
// #include <llvm/Passes/StandardInstrumentations.h>
// #include <llvm/Support/TargetSelect.h>
// #include <llvm/Target/TargetMachine.h>
// #include <llvm/Transforms/InstCombine/InstCombine.h>
// #include <llvm/Transforms/Scalar.h>
// #include <llvm/Transforms/Scalar/GVN.h>
// #include <llvm/Transforms/Scalar/Reassociate.h>
// #include <llvm/Transforms/Scalar/SimplifyCFG.h>

#include "Core/AST/AST.hpp"

namespace WandeltCore
{
	class Codegen : public Visitor
	{
	public:
		Codegen();
		~Codegen();

		void GenerateIR(const std::vector<Expression*>& expressions);

		const llvm::Module& GetModuleWithGeneratedIR() const { return m_Module; }

	private:
		void GenerateEntrypoint();

		llvm::Value* GenerateExpression(Expression* expression);

		llvm::Value* GenerateNumberLiteral(NumberLiteral* numberLiteral) override;
		llvm::Value* GenerateBinaryExpression(BinaryExpression* binaryExpression) override;
		llvm::Value* GenerateUnaryExpression(UnaryExpression* unaryExpression) override;
		llvm::Value* GeneratePowerExpression(PowerExpression* powerExpression) override;
		llvm::Value* GenerateGroupingExpression(GroupingExpression* groupingExpression) override;
		llvm::Value* GenerateReturnStatement(ReturnStatement* returnStatement) override;

	private:
		llvm::LLVMContext m_Context;
		llvm::IRBuilder<> m_Builder;
		llvm::Module m_Module;
	};
} // namespace WandeltCore
