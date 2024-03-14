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
#include <iostream>

using namespace llvm;
using namespace std;

int main() {
    //Parser parser;
    //std::unique_ptr<ProgramAST> ast = parser.ParseProgram();
    //ast->print();

    TheModule = llvm::make_unique<llvm::Module>("Fixed Point Compiler", TheContext);

    // call codegen

    TheModule->print(errs(), nullptr);

    return 0;
}