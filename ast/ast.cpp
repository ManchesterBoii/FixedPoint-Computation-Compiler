#include "ast.h"

NumExpr::NumExpr(double value) : value(value) {}

NameExpr::NameExpr(const std::string& name) : name(name) {}

BinOpExpr::BinOpExpr(std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs, char o)
    : left(std::move(lhs)), right(std::move(rhs)), op(o) {}

FPnt::FPnt(double lb, double ub, double prec) 
    : lowerBound(lb), upperBound(ub), precision(prec) {}

Definition::Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<AST> expr)
    : name(n), floatingPointNotation(std::move(fpnt)), expression(std::move(expr)) {}

Definition::Definition(const std::string& n, std::unique_ptr<AST> expr)
    : name(n), floatingPointNotation(nullptr), expression(std::move(expr)) {}

void NumExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "NumExpr(" << value << ")";
}

void NameExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "NameExpr(" << name << ")";
}

void BinOpExpr::print(int depth) {
    std::string indent(depth * 4, ' ');
    std::cout << indent << "BinOpExpr(" << op << ",\n";
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
    std::cout << indent << "([" << lowerBound << ", " << upperBound << "], " << precision << ")";
}


Value *BinOpExpr::codegen(Function* F) {
    Value *L = left->codegen(F);
    Value *R = right->codegen(F);
    if (!L || !R)
        return nullptr;

    switch (op) {
    case '+':
        return nullptr; // TODO
    case '-':
        return nullptr; // TODO
    case '*':
        return nullptr; // TODO
    default:
        return LogErrorV("invalid binary operator");
    }

    // need to handle FPnt
}

Value *ProgramAST::codegen(Function* F) {

  for (auto &iter: this->Stmts){
    iter->codegen(F);
  }

  return nullptr;
}


Value *Definition::codegen(Function* F) {
    Value *Val = expression->codegen(F);

    NamedValues[name] = Val;
    

    // TODO: handle PFnt

    return nullptr
}


Value *NameExpr::codegen() {
    
  Value *V = NamedValues[name];
  if (!V)
    return LogErrorV("Unknown variable name");
  return V;
}

Value *NumberExprAST::codegen() {
  return ConstantFP::get(*TheContext, APFloat(Val)); // might need to modify this. 
}