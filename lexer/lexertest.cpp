#include "lexer.h"

#include <iostream>

/**
 * Simple line by line output of tokens for a given program to test functionality of lexer.
*/
int main() {
    while (true) {
        int token = gettok();

        switch (token) {
            case tok_eof:
                std::cout << "EOF\n";
                return 0;
            case tok_def:
                std::cout << "def\n";
                break;
            case tok_in:
                std::cout << "in\n";
                break;
            case tok_identifier:
                std::cout << "id " << IdentifierStr << "\n";
                break;
            case tok_number:
                std::cout << "num " << NumVal << "\n";
                break;
            default:
                std::cout << static_cast<char>(token) << "\n";
        }
    }
}