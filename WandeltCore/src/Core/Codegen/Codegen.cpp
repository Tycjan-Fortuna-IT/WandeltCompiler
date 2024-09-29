#include "Codegen.hpp"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

namespace WandeltCore
{
	Codegen::Codegen() : m_Builder(llvm::IRBuilder<>(m_Context)), m_Module(llvm::Module("wandelt", m_Context))
	{
	}

	void Codegen::GenerateIR()
	{
		GenerateEntrypoint();

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

		m_Builder.CreateRet(llvm::ConstantInt::get(int32Type, 12));
	}
} // namespace WandeltCore
