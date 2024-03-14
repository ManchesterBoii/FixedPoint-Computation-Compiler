#include "../utils/utils.cpp"
#include "ast.h"

std::map<std::string, std::unique_ptr<FPnt>>& GetIntervalTable() {
    static std::map<std::string, std::unique_ptr<FPnt>> intervalTable;
    return intervalTable;
}

std::unique_ptr<AST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

NumExpr::NumExpr(double value) : value(value) {}

NameExpr::NameExpr(const std::string& name) : name(name) {}

BinOpExpr::BinOpExpr(std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs, char o)
    : left(std::move(lhs)), right(std::move(rhs)), op(o) {}

FPnt::FPnt(double lb, double ub, int decimalBits) 
    : lowerBound(lb), upperBound(ub), decimalBits(decimalBits) {}

Definition::Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<AST> expr)
    : AST(std::move(fpnt)), name(n), expression(std::move(expr)) {}

Definition::Definition(const std::string& n, std::unique_ptr<AST> expr)
    : AST(nullptr), name(n), expression(std::move(expr)) {}

void NumExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "NumExpr(" << value;
    if (floatingPointNotation) {
        std::cout << ",\n";
        floatingPointNotation->print(depth + 1);
    } 
    std::cout << "\n" << indent << ")";
}

void NameExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "NameExpr(" << name;
    if (floatingPointNotation) {
        std::cout << ",\n";
        floatingPointNotation->print(depth + 1);
    } 
    std::cout << "\n" << indent << ")";
}

void BinOpExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "BinOpExpr(" << op << ",\n";
    if (floatingPointNotation) {
        floatingPointNotation->print(depth + 1);
        std::cout << ",\n";
    }
    left->print(depth + 1);
    std::cout << ",\n";
    right->print(depth + 1);
    std::cout << "\n" << indent << ")";
}

void Definition::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "Definition(" << name << ",\n";
    if (floatingPointNotation) {
        floatingPointNotation->print(depth + 1);
        std::cout << ",\n";
    }
    expression->print(depth + 1);
    std::cout << "\n" << indent << ")";
}

void ProgramAST::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "ProgramAST(" << std::endl;
    for (auto it = Stmts.begin(); it != Stmts.end(); ++it) {
        (*it)->print(depth + 1);
        if (std::next(it) != Stmts.end()) {
            std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << indent << ")" << std::endl;
}

void FPnt::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "([" << lowerBound << ", " << upperBound << "], " << decimalBits << ")";
}

std::unique_ptr<FPnt> NumExpr::propagateIntervals() {
    floatingPointNotation = std::make_unique<FPnt>(FPnt(value, value, 0));
    return std::make_unique<FPnt>(FPnt(value, value, 0));
}

std::unique_ptr<FPnt> NameExpr::propagateIntervals() {    
    auto it = GetIntervalTable().find(name);
    if (it != GetIntervalTable().end() && it->second) {
        floatingPointNotation = std::make_unique<FPnt>(*(it->second));
        return std::make_unique<FPnt>(*(it->second));
    } else {
        return nullptr;
    }
}

std::unique_ptr<FPnt> BinOpExpr::propagateIntervals() {
    auto leftInterval = left->propagateIntervals();
    if (!leftInterval) {
        std::cerr << "left interval null" << std::endl;
        return nullptr;
    }
    auto rightInterval = right->propagateIntervals();
    if (!leftInterval) {
        std::cerr << "right interval null" << std::endl;
        return nullptr;
    }
    double lowerBound, upperBound;
    int decimalBits;
    if (op == '+') {
        lowerBound = leftInterval->lowerBound + rightInterval->lowerBound;
        upperBound = leftInterval->upperBound + rightInterval->upperBound;
        decimalBits = std::max(leftInterval->decimalBits, rightInterval->decimalBits);
    } else if (op == '*') {
        double p1 = leftInterval->lowerBound * rightInterval->lowerBound;
        double p2 = leftInterval->lowerBound * rightInterval->upperBound;
        double p3 = leftInterval->upperBound * rightInterval->lowerBound;
        double p4 = leftInterval->upperBound * rightInterval->upperBound;

        lowerBound = std::min({p1, p2, p3, p4});
        upperBound = std::max({p1, p2, p3, p4});

        decimalBits = std::max(leftInterval->decimalBits, rightInterval->decimalBits);
    }

    floatingPointNotation = std::make_unique<FPnt>(FPnt(lowerBound, upperBound, decimalBits));
    return std::make_unique<FPnt>(FPnt(lowerBound, upperBound, decimalBits));
}

std::unique_ptr<FPnt> Definition::propagateIntervals() {
    if (floatingPointNotation) {
        expression->propagateIntervals();
        return std::make_unique<FPnt>(*floatingPointNotation);
    }

    // For now, prioritize the Expression's interval over user input.
    // might make sense to modify this logic later

    auto exprFPnt = expression->propagateIntervals();
    if (exprFPnt) {
        floatingPointNotation = std::make_unique<FPnt>(*exprFPnt);
        GetIntervalTable()[name] = std::make_unique<FPnt>(*exprFPnt);
    }
    return exprFPnt ? std::make_unique<FPnt>(*exprFPnt) : nullptr;
}

std::unique_ptr<FPnt> ProgramAST::propagateIntervals() {
    for (auto &stmt : Stmts) {
        stmt->propagateIntervals();
    }
    return nullptr;
}

Value *BinOpExpr::codegen(Function* F) {
    Value *L = left->codegen(F);
    Value *R = right->codegen(F);
    if (!L || !R) {
        return nullptr;
    }

    int resultBitWidth = calcNumBits(
        floatingPointNotation->lowerBound, 
        floatingPointNotation->upperBound, 
        floatingPointNotation->decimalBits);
    Type *resultType = IntegerType::get(*TheContext, resultBitWidth);


    // signed extension to result type of binary expression
    L = Builder->CreateSExtOrTrunc(L, resultType);
    R = Builder->CreateSExtOrTrunc(R, resultType);
    L->print(errs());
    R->print(errs());
    Value* res;

    switch (op) {
    case '+':
        res = Builder->CreateAdd(L, R);
        res->print(errs());
        std::cout << "add" << std::endl;
        return res;
    case '*':
        res = Builder->CreateMul(L, R);
        res->print(errs());
        std::cout << "mult" << std::endl;
        return res;
    default:
        LogError("Unknown variable name");
        return nullptr;
    }

    // need to handle FPnt
}


Value *ProgramAST::codegen(Function* F) {
    Value *stmtRes;
    for (auto &Stmt : Stmts) {
        stmtRes = Stmt->codegen(F);
        stmtRes->print(errs());
        BasicBlock *bb = Builder->GetInsertBlock();
        Module *m = TheModule.get();
        kprintf_val(m, bb, stmtRes);
        kprintf_str(m, bb, "asdf\n");
    }
    TheModule->print(errs(), nullptr);
    return stmtRes;
}


Value *Definition::codegen(Function* F) {
    Value *Val = expression->codegen(F);
    NamedValues[name] = Val;

    Val->print(errs());
    // TODO handle FPnt
    return Val;
}


Value *NameExpr::codegen(Function* F) {
    Value *Val = NamedValues[name];
    if (!Val){
        LogError("Unknown variable name");
        return nullptr;
    }
    Val->print(errs());
    return Val;
}

Value* NumExpr::codegen(llvm::Function* F) {
    auto& context = F->getContext();

    int bitWidth = calcNumBits(floatingPointNotation->lowerBound, floatingPointNotation->upperBound, floatingPointNotation->decimalBits);

    // Select the LLVM integer type based on the calculated bit width
    llvm::Type* intType ;
    std::cout << intType << "\n";
    switch (bitWidth) {
        case 8:
            intType = llvm::Type::getInt8Ty(context);
            break;
        case 16:
            intType = llvm::Type::getInt16Ty(context);
            break;
        case 32:
            intType = llvm::Type::getInt32Ty(context);
            break;
        case 64:
            intType = llvm::Type::getInt64Ty(context);
            break;
        default:
            throw std::runtime_error("Unsupported bit width");
    }

    // might need change later
    uint64_t scaledValue = static_cast<uint64_t>(static_cast<uint64_t>(value) << static_cast<uint64_t>(floatingPointNotation->decimalBits));
    std::cout << scaledValue <<"\n";

    llvm::Constant* constVal = llvm::ConstantInt::get(intType, scaledValue);
    constVal->print(errs());

    return constVal;
}

void kprintf_val(Module *mod, BasicBlock *bb, Value* val) {
  Function *func_printf = mod->getFunction("printf");
  if (!func_printf) {
    PointerType::get(IntegerType::get(mod->getContext(), 8), 0);
    FunctionType *FuncTy9 = FunctionType::get(IntegerType::get(mod->getContext(), 32), true);

    func_printf = Function::Create(FuncTy9, GlobalValue::ExternalLinkage, "printf", mod);
    func_printf->setCallingConv(CallingConv::C);
  }

  IRBuilder <> builder(*TheContext);
  builder.SetInsertPoint(bb);


  Value *str = builder.CreateGlobalStringPtr("%d");

  std::vector <Value *> int32_call_params;
  int32_call_params.push_back(str);
  int32_call_params.push_back(val);

  CallInst::Create(func_printf, int32_call_params, "call", bb);
}

void kprintf_str(Module *mod, BasicBlock *bb, const std::string& to_print) {
  Function *func_printf = mod->getFunction("printf");
  if (!func_printf) {
    PointerType::get(IntegerType::get(mod->getContext(), 8), 0);
    FunctionType *FuncTy9 = FunctionType::get(IntegerType::get(mod->getContext(), 32), true);

    func_printf = Function::Create(FuncTy9, GlobalValue::ExternalLinkage, "printf", mod);
    func_printf->setCallingConv(CallingConv::C);
  }

  IRBuilder <> builder(*TheContext);
  builder.SetInsertPoint(bb);


  Value *str = builder.CreateGlobalStringPtr(to_print);

  std::vector <Value *> int32_call_params;
  int32_call_params.push_back(str);

  CallInst::Create(func_printf, int32_call_params, "call", bb);
}