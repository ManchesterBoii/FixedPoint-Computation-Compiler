#include "../ast/ast.h"
#include <memory>

class Parser {
    int CurrentToken;

    std::unique_ptr<Expr> ParseExpression();
    std::unique_ptr<Expr> ParseTerm();
    std::unique_ptr<Expr> ParseFactor();
    std::unique_ptr<Definition> ParseDefinition();
    std::unique_ptr<FPnt> ParseFloatingPointNotation();

public:
    Parser();
    void NextToken();
    std::unique_ptr<ProgramAST> ParseProgram();
};


