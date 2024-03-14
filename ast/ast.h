#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

class FPnt;

std::map<std::string, std::unique_ptr<FPnt>>& GetIntervalTable();

class AST {
public: 
    virtual ~AST() = default;
    virtual void print(int depth = 0) = 0;

    virtual std::unique_ptr<FPnt> propagateIntervals() = 0;
};

class Expr: public AST {
public: 
    virtual ~Expr() = default;
};

class NumExpr : public Expr {
public:
    double value;
    NumExpr(double value);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;
};

class NameExpr: public Expr {
public:
    std::string name;
    NameExpr(const std::string& name);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;
};

class BinOpExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    char op; 
    BinOpExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, char o);

    std::unique_ptr<FPnt> propagateIntervals() override;
    void print(int depth = 0) override;
};

class FPnt {
public:
    double lowerBound;
    double upperBound;
    double precision;
    FPnt(double lb, double ub, double prec);

    void print(int depth = 0);
};

class Definition : public AST {
public:
    std::string name;
    std::unique_ptr<FPnt> floatingPointNotation; 
    std::unique_ptr<Expr> expression;
    Definition(const std::string& n, std::unique_ptr<FPnt> fpnt, std::unique_ptr<Expr> expr);
    Definition(const std::string& n, std::unique_ptr<Expr> expr);

    std::unique_ptr<FPnt> propagateIntervals() override;

    void print(int depth = 0) override;
};

class ProgramAST : public AST {
public:
    std::vector<std::unique_ptr<Definition>> Stmts;

    std::unique_ptr<FPnt> propagateIntervals() override;

    void print(int depth = 0) override;
};

