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

	void Codegen::GenerateIR(const std::vector<Statement*>& statements)
	{
		GenerateEntrypoint();

		for (Statement* statement : statements) GenerateStatement(statement);

		// always return something
		if (statements.empty() || !dynamic_cast<ReturnStatement*>(statements.back()))
		{
			GenerateStatement(new ReturnStatement(SourceLocation{}, new NumberLiteral(SourceLocation{}, 0)));
		}

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
		GenerateBuiltins();

		llvm::Type* int32Type = llvm::Type::getInt32Ty(m_Context);

		llvm::FunctionType* functionType = llvm::FunctionType::get(int32Type, false);

		llvm::Function* mainFunction =
		    llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, "main", m_Module);
		llvm::verifyFunction(*mainFunction);

		llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(m_Context, "entry", mainFunction);

		m_Builder.SetInsertPoint(entryBlock);
	}

	void Codegen::GenerateBuiltins()
	{
		GenerateBuiltinPrintlnFunction();
	}

	void Codegen::GenerateBuiltinPrintlnFunction()
	{
		llvm::FunctionType* type = llvm::FunctionType::get(m_Builder.getInt32Ty(), {m_Builder.getPtrTy()}, true);

		llvm::Function* printf = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "printf", m_Module);
	}

	llvm::Value* Codegen::GenerateStatement(Statement* statement)
	{
		llvm::Value* result = statement->Generate(this);

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
		llvm::Value* lhs = GenerateStatement(binaryExpression->GetLeft());
		llvm::Value* rhs = GenerateStatement(binaryExpression->GetRight());

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

		lhs = IntToDouble(lhs);
		rhs = IntToDouble(rhs);

		if (op == TokenType::EQUAL_EQUAL)
			return BoolToInt(m_Builder.CreateFCmpOEQ(lhs, rhs));
		else if (op == TokenType::BANG_EQUAL)
			return BoolToInt(m_Builder.CreateFCmpONE(lhs, rhs));
		else if (op == TokenType::LESS)
			return BoolToInt(m_Builder.CreateFCmpOLT(lhs, rhs));
		else if (op == TokenType::LESS_EQUAL)
			return BoolToInt(m_Builder.CreateFCmpOLE(lhs, rhs));
		else if (op == TokenType::GREATER)
			return BoolToInt(m_Builder.CreateFCmpOGT(lhs, rhs));
		else if (op == TokenType::GREATER_EQUAL)
			return BoolToInt(m_Builder.CreateFCmpOGE(lhs, rhs));

		llvm_unreachable("unexpected binary operator");

		return nullptr;
	}

	llvm::Value* Codegen::GenerateUnaryExpression(UnaryExpression* unaryExpression)
	{
		llvm::Value* operand = GenerateStatement(unaryExpression->GetOperand());
		const TokenType& op  = unaryExpression->GetOperator();

		if (op == TokenType::MINUS)
			return m_Builder.CreateNeg(operand);

		llvm_unreachable("unexpected unary operator");

		return nullptr;
	}

	llvm::Value* Codegen::GeneratePowerExpression(PowerExpression* powerExpression)
	{
		llvm::Value* base     = GenerateStatement(powerExpression->GetBase());
		llvm::Value* exponent = GenerateStatement(powerExpression->GetExponent());

		// if base and exponent are numbers, we can calculate the result at compile time
		if (llvm::ConstantInt* baseConstant = llvm::dyn_cast<llvm::ConstantInt>(base))
		{
			if (llvm::ConstantInt* exponentConstant = llvm::dyn_cast<llvm::ConstantInt>(exponent))
			{
				llvm::APInt baseValue = baseConstant->getValue();
				int64_t exponentValue = exponentConstant->getValue().getSExtValue();

				llvm::APInt result = baseValue;
				if (exponentValue == 0)
				{
					result = llvm::APInt(baseValue.getBitWidth(), 1); // base^0 = 1
				}
				else
				{
					for (int64_t i = 1; i < exponentValue; ++i)
					{
						result *= baseValue;
					}
				}

				return llvm::ConstantInt::get(base->getType(), result);
			}
		}

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
		return GenerateStatement(groupingExpression->GetExpression());
	}

	llvm::Value* Codegen::GenerateCallExpression(CallExpression* callExpression)
	{
		// llvm::Function* function = m_Module.getFunction(callExpression->GetDeclaration()->GetIdentifier());
		llvm::Function* function = m_Module.getFunction("printf");

		// for now only println(12) is supported

		llvm::Value* arg = GenerateStatement(callExpression->GetArgs().front());

		std::vector<llvm::Value*> args = {m_Builder.CreateGlobalStringPtr("%d\n"), arg};

		return m_Builder.CreateCall(function, args);
	}

	llvm::Value* Codegen::GenerateDeclaration(Declaration* declaration)
	{
		return nullptr;
	}

	llvm::Value* Codegen::GenerateIfStatement(IfStatement* ifStatement)
	{
		llvm::Function* fn = GetCurrentFunction();

		const bool hasElse = ifStatement->HasElseScope();

		llvm::Value* condition = GenerateStatement(ifStatement->GetCondition());

		llvm::BasicBlock* exitBlock  = llvm::BasicBlock::Create(m_Context, "if.exit");
		llvm::BasicBlock* trueBlock  = llvm::BasicBlock::Create(m_Context, "if.true");
		llvm::BasicBlock* falseBlock = hasElse ? llvm::BasicBlock::Create(m_Context, "if.else") : exitBlock;

		m_Builder.CreateCondBr(IntToBool(condition), trueBlock, falseBlock);

		trueBlock->insertInto(fn);
		m_Builder.SetInsertPoint(trueBlock);
		GenerateScope(ifStatement->GetThenScope());
		m_Builder.CreateBr(exitBlock);

		if (hasElse)
		{
			falseBlock->insertInto(fn);
			m_Builder.SetInsertPoint(falseBlock);
			GenerateScope(ifStatement->GetElseScope());
			m_Builder.CreateBr(exitBlock);
		}

		exitBlock->insertInto(fn);
		m_Builder.SetInsertPoint(exitBlock);

		return nullptr;
	}

	llvm::Value* Codegen::GenerateReturnStatement(ReturnStatement* returnStatement)
	{
		llvm::Value* returnValue = GenerateStatement(returnStatement->GetExpression());

		m_Builder.CreateRet(returnValue);

		return nullptr;
	}

	llvm::Value* Codegen::GenerateScope(Scope* scope)
	{
		for (Statement* statement : scope->GetStatements()) GenerateStatement(statement);

		return nullptr;
	}

	llvm::Function* Codegen::GetCurrentFunction()
	{
		return m_Builder.GetInsertBlock()->getParent();
	}

	llvm::Value* Codegen::DoubleToBool(llvm::Value* val)
	{
		ASSERT(val->getType()->isDoubleTy());

		return m_Builder.CreateFCmpONE(val, llvm::ConstantFP::get(m_Builder.getDoubleTy(), 0.0), "double.to.bool");
	}

	llvm::Value* Codegen::BoolToDouble(llvm::Value* val)
	{
		ASSERT(val->getType()->isIntegerTy(1));

		return m_Builder.CreateUIToFP(val, m_Builder.getDoubleTy(), "bool.to.double");
	}

	llvm::Value* Codegen::IntToBool(llvm::Value* val)
	{
		ASSERT(val->getType()->isIntegerTy());

		return m_Builder.CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0), "int.to.bool");
	}

	llvm::Value* Codegen::BoolToInt(llvm::Value* val)
	{
		ASSERT(val->getType()->isIntegerTy(1));

		return m_Builder.CreateZExt(val, m_Builder.getInt32Ty(), "bool.to.int");
	}

	llvm::Value* Codegen::IntToDouble(llvm::Value* val)
	{
		ASSERT(val->getType()->isIntegerTy());

		return m_Builder.CreateSIToFP(val, m_Builder.getDoubleTy(), "int.to.double");
	}

	llvm::Value* Codegen::DoubleToInt(llvm::Value* val)
	{
		ASSERT(val->getType()->isDoubleTy());

		return m_Builder.CreateFPToSI(val, m_Builder.getInt32Ty(), "double.to.int");
	}
} // namespace WandeltCore
