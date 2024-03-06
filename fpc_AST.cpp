#include "fpc_AST.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>

class AST {
    public: 
        virtual ~AST() = default;
};

class Expr {
    public: 
        virtual ~Expr() = default;
};


class NumExpr : public Expr {
public:
    double value;

    NumExpr(double value) : value(value) {}
};


class NameExpr: public Expr {
    public:
    std::string name;

    NameExpr(const std::string& name) : name(name) {}
};

class BinOpExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    char op; 

    BinOpExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, char o)
        : left(lhs), right(rhs), op(o) {}
};

class FPnt {
public:
    double lowerBound;
    double upperBound;
    double precision;

    FPnt(double lb, double ub, double prec)
        : lowerBound(lb), upperBound(ub), precision(prec) {}
};

class ProgramAST : public AST {
public:
    std::vector<unique_ptr<Definition> > Stmts;
    
};

class Definition : public AST {
public:
    std::string name;
    std::unique_ptr<FPnt> floatingPointNotation; 
    std::unique_ptr<Expr> expression;

    Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<Expr> expr)
        : name(n), floatingPointNotation(fpnt), expression(expr) {}

    Definition(const std::string& n, std::unique_ptr<Expr> expr)
        : name(n), floatingPointNotation(nullptr), expression(expr) {}
};








