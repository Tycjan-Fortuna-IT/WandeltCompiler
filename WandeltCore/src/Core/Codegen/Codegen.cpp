#include "Codegen.hpp"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

namespace WandeltCore
{
	Codegen::Codegen() : m_Builder(llvm::IRBuilder<>(m_Context)), m_Module(llvm::Module("wandelt", m_Context))
	{
		m_Module.setTargetTriple(llvm::sys::getDefaultTargetTriple());
		m_Module.setDataLayout("e-m:e-p:32:32-i64:64-f80:32-n8:16:32-S128");
	}

	Codegen::~Codegen()
	{
	}

	void Codegen::GenerateIR(const std::vector<Expression*>& expressions)
	{
		GenerateEntrypoint();

		Expression* mainReturn = expressions.at(0);

		GenerateReturnStatement((ReturnStatement*)mainReturn);

		std::error_code ec;
		llvm::raw_fd_ostream file("output.ll", ec, llvm::sys::fs::OF_Text);

		if (ec)
		{
			SYSTEM_ERROR("Failed to create output.ll file: {}", ec.message());
			return;
		}

		m_Module.print(file, nullptr);
	}

	void Codegen::GenerateEntrypoint()
	{
		llvm::Type* int32Type = llvm::Type::getInt32Ty(m_Context);

		llvm::FunctionType* functionType = llvm::FunctionType::get(int32Type, false);

		llvm::Function* mainFunction =
		    llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, "main", m_Module);
		llvm::verifyFunction(*mainFunction);

		llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(m_Context, "entry", mainFunction);

		m_Builder.SetInsertPoint(entryBlock);

		// m_Builder.CreateRet(llvm::ConstantInt::get(int32Type, 0));
	}

	llvm::Value* Codegen::GenerateExpression(Expression* expression)
	{
		llvm::Value* result = expression->GenerateExpression(this);

		if (result)
			return result;

		llvm_unreachable("unexpected expression");

		return nullptr;
	}

	llvm::Value* Codegen::GenerateNumberLiteral(NumberLiteral* numberLiteral)
	{
		return llvm::ConstantInt::get(m_Builder.getInt32Ty(), numberLiteral->GetValue());
	}

	llvm::Value* Codegen::GenerateBinaryExpression(BinaryExpression* binaryExpression)
	{
		llvm::Value* lhs = GenerateExpression(binaryExpression->GetLeft());
		llvm::Value* rhs = GenerateExpression(binaryExpression->GetRight());

		TokenType op = binaryExpression->GetOperator();

		if (op == TokenType::PLUS)
			return m_Builder.CreateAdd(lhs, rhs);
		else if (op == TokenType::MINUS)
			return m_Builder.CreateSub(lhs, rhs);
		else if (op == TokenType::STAR)
			return m_Builder.CreateMul(lhs, rhs);
		else if (op == TokenType::SLASH)
			return m_Builder.CreateSDiv(lhs, rhs);
		else if (op == TokenType::PERCENT)
			return m_Builder.CreateSRem(lhs, rhs);

		llvm_unreachable("unexpected binary operator");

		return nullptr;
	}

	llvm::Value* Codegen::GenerateUnaryExpression(UnaryExpression* unaryExpression)
	{
		llvm::Value* operand = GenerateExpression(unaryExpression->GetOperand());
		const TokenType& op  = unaryExpression->GetOperator();

		if (op == TokenType::MINUS)
			return m_Builder.CreateNeg(operand);

		llvm_unreachable("unexpected unary operator");

		return nullptr;
	}

	llvm::Value* Codegen::GeneratePowerExpression(PowerExpression* powerExpression)
	{
		llvm::Value* base     = GenerateExpression(powerExpression->GetBase());
		llvm::Value* exponent = GenerateExpression(powerExpression->GetExponent());

		llvm::Function* function = m_Builder.GetInsertBlock()->getParent();

		llvm::BasicBlock* entryBlock     = m_Builder.GetInsertBlock();
		llvm::BasicBlock* loopBlock      = llvm::BasicBlock::Create(m_Context, "loop", function);
		llvm::BasicBlock* afterLoopBlock = llvm::BasicBlock::Create(m_Context, "afterloop", function);

		// Initialize the result to 1
		llvm::Value* result = llvm::ConstantInt::get(base->getType(), 1);

		// Initialize the loop counter to 0
		llvm::Value* counter = llvm::ConstantInt::get(exponent->getType(), 0);

		// Jump to the loop block
		m_Builder.CreateBr(loopBlock);

		// Set the insert point to the loop block
		m_Builder.SetInsertPoint(loopBlock);

		// PHI node for the result
		llvm::PHINode* resultPhi = m_Builder.CreatePHI(base->getType(), 2, "result");
		resultPhi->addIncoming(result, entryBlock);

		// PHI node for the counter
		llvm::PHINode* counterPhi = m_Builder.CreatePHI(exponent->getType(), 2, "counter");
		counterPhi->addIncoming(counter, entryBlock);

		// Multiply the result by the base
		llvm::Value* newResult = m_Builder.CreateMul(resultPhi, base);

		// Increment the counter
		llvm::Value* newCounter = m_Builder.CreateAdd(counterPhi, llvm::ConstantInt::get(exponent->getType(), 1));

		// Check if the counter is less than or equal to the exponent
		llvm::Value* condition = m_Builder.CreateICmpULE(newCounter, exponent);

		// Add the new values to the PHI nodes
		resultPhi->addIncoming(newResult, loopBlock);
		counterPhi->addIncoming(newCounter, loopBlock);

		// Create the conditional branch
		m_Builder.CreateCondBr(condition, loopBlock, afterLoopBlock);

		// Set the insert point to the after loop block
		m_Builder.SetInsertPoint(afterLoopBlock);

		// Return the final result
		return resultPhi;
	}

	llvm::Value* Codegen::GenerateGroupingExpression(GroupingExpression* groupingExpression)
	{
		return GenerateExpression(groupingExpression->GetExpression());
	}

	llvm::Value* Codegen::GenerateReturnStatement(ReturnStatement* returnStatement)
	{
		llvm::Type* int32Type = llvm::Type::getInt32Ty(m_Context);

		llvm::Function* mainFunction = m_Module.getFunction("main");

		llvm::BasicBlock* entryBlock = &mainFunction->getEntryBlock();

		m_Builder.SetInsertPoint(entryBlock);

		llvm::Value* returnValue = GenerateExpression(returnStatement->GetExpression());

		m_Builder.CreateRet(returnValue);

		return nullptr;
	}
} // namespace WandeltCore
