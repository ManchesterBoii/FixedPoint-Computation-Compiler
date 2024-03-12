#include "../ast/ast.h"
#include <memory>

class Parser {
    int CurrentToken;

    std::unique_ptr<AST> ParseExpression();
    std::unique_ptr<AST> ParseTerm();
    std::unique_ptr<AST> ParseFactor();
    std::unique_ptr<Definition> ParseDefinition();
    std::unique_ptr<FPnt> ParseFloatingPointNotation();

public:
    Parser();
    void NextToken();
    std::unique_ptr<ProgramAST> ParseProgram();
};


