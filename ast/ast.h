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
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

class FPnt;

std::map<std::string, std::unique_ptr<FPnt>>& GetIntervalTable();

using namespace llvm;

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<IRBuilder<>> Builder;
static std::map<std::string, Value *> NamedValues;


class AST {
public: 
    virtual ~AST() = default;
    virtual void print(int depth = 0) = 0;

    virtual std::unique_ptr<FPnt> propagateIntervals() = 0;

    virtual Value *codegen(Function* F) = 0;
};

// class Expr: public AST {
// public: 
//     virtual ~Expr() = default;

//     virtual Value *codegen() = 0;
// };

class NumExpr : public AST {
public:
    double value;
    NumExpr(double value);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;

    Value *codegen(Function* F) override;
};

class NameExpr: public AST {
public:
    std::string name;
    NameExpr(const std::string& name);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;

    Value *codegen(Function* F) override;
};

class BinOpExpr : public AST {
public:
    std::unique_ptr<AST> left;
    std::unique_ptr<AST> right;
    char op; 
    BinOpExpr(std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs, char o);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;

    Value *codegen(Function* F) override;
};

class FPnt {
public:
    double lowerBound;
    double upperBound;
    double precision;
    FPnt(double lb, double ub, double prec);

    void print(int depth = 0);

    //Function *codegen(Function* F);
};

class Definition : public AST {
public:
    std::string name;
    std::unique_ptr<FPnt> floatingPointNotation; 
    std::unique_ptr<AST> expression;
    Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<AST> expr);
    Definition(const std::string& n, std::unique_ptr<AST> expr);

    std::unique_ptr<FPnt> propagateIntervals() override;

    void print(int depth = 0) override;

    Value *codegen(Function* F) override;

};

class ProgramAST : public AST {
public:
    std::vector<std::unique_ptr<Definition>> Stmts;

    std::unique_ptr<FPnt> propagateIntervals() override;

    void print(int depth = 0) override;

    Value *codegen(Function* F) override;

};

std::unique_ptr<AST> LogError(const char *Str);
