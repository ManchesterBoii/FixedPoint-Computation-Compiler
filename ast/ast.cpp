#include "AST.h"

NumExpr::NumExpr(double value) : value(value) {}

NameExpr::NameExpr(const std::string& name) : name(name) {}

BinOpExpr::BinOpExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, char o)
    : left(std::move(lhs)), right(std::move(rhs)), op(o) {}

FPnt::FPnt(double lb, double ub, double prec) 
    : lowerBound(lb), upperBound(ub), precision(prec) {}

Definition::Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<Expr> expr)
    : name(n), floatingPointNotation(std::move(fpnt)), expression(std::move(expr)) {}

Definition::Definition(const std::string& n, std::unique_ptr<Expr> expr)
    : name(n), floatingPointNotation(nullptr), expression(std::move(expr)) {}

