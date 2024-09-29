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
		if (NumberLiteral* numberLiteral = dynamic_cast<NumberLiteral*>(expression))
			return llvm::ConstantInt::get(m_Builder.getInt32Ty(), numberLiteral->GetValue());

		if (BinaryExpression* binaryExpression = dynamic_cast<BinaryExpression*>(expression))
			return GenerateBinaryExpression(binaryExpression);

		llvm_unreachable("unexpected expression");

		return nullptr;
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

		llvm_unreachable("unexpected binary operator");

		return nullptr;
	}

	void Codegen::GenerateReturnStatement(ReturnStatement* returnStatement)
	{
		llvm::Type* int32Type = llvm::Type::getInt32Ty(m_Context);

		llvm::Function* mainFunction = m_Module.getFunction("main");

		llvm::BasicBlock* entryBlock = &mainFunction->getEntryBlock();

		m_Builder.SetInsertPoint(entryBlock);

		llvm::Value* returnValue = GenerateExpression(returnStatement->GetExpression());

		m_Builder.CreateRet(returnValue);
	}
} // namespace WandeltCore
