#include "parser.h"

#include <iostream>

/**
 * Simple program to print AST for testing purposes.
*/
int main() {
    Parser parser;
    std::unique_ptr<ProgramAST> ast = parser.ParseProgram();
    ast->print();
}