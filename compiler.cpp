#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "parser/parser.h"

using namespace llvm;

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;

static void InitializeModule() {
  // Open a new context and module.
  TheContext = std::make_unique<LLVMContext>();
  TheModule = std::make_unique<Module>("Fixed Point Compiler", *TheContext);

  // Create a new builder for the module.
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

int main() {
    Parser parser;
    std::unique_ptr<ProgramAST> program = parser.ParseProgram();
    // program->print();

    InitializeModule();

    std::vector<Type *> Args(0, Type::getDoubleTy(*TheContext));
    FunctionType *FT = FunctionType::get(Type::getVoidTy(*TheContext), Args, false);

    Function *F = Function::Create(FT, Function::ExternalLinkage, "__anon_expr", TheModule.get());
    BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(&(F->getEntryBlock()));

    program->codegen(F);
    llvm::outs() << *F << "\n";

    Builder->CreateRet(nullptr);

    TheModule->print(errs(), nullptr);

    return 0;
}