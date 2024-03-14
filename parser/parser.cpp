#include "parser.h"
#include "../lexer/lexer.h"
#include <iostream>
#include <stdexcept>

Parser::Parser() { NextToken(); }

void Parser::NextToken() {
    CurrentToken = gettok();
}

std::unique_ptr<ProgramAST> Parser::ParseProgram() {
    auto Program = std::make_unique<ProgramAST>();
    while (CurrentToken != tok_eof) {
        auto Def = ParseDefinition();
        if (Def) {
            Program->Stmts.push_back(std::move(Def));
        } else {
            std::cerr << "Unknown definition or syntax error\n";
            NextToken();
        }
    }

    // Print interval table for debugging purposes
    // std::cout << "TABLE:" << std::endl;

    // for (const auto& pair : GetIntervalTable()) {
    //     std::cout << "Key: " << pair.first << " - Value: ";
    //     if (pair.second) {
    //         pair.second->print();
    //     } else {
    //         std::cout << "Nullptr" << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    Program->propagateIntervals();

    return Program;
}

std::unique_ptr<Definition> Parser::ParseDefinition() {
    if (CurrentToken != tok_def) throw std::runtime_error("Expected 'def'");
    NextToken(); 

    if (CurrentToken != tok_identifier) throw std::runtime_error("Expected identifier after 'def'");
    std::string Name = IdentifierStr;
    NextToken(); 

    std::unique_ptr<FPnt> defFPnt = nullptr;
    if (CurrentToken == tok_in) {
        defFPnt = ParseFloatingPointNotation(); 
    }

    if (CurrentToken != '=') throw std::runtime_error("Expected '=' in definition");
    NextToken(); 

    auto Expr = ParseExpression(); 

    if (defFPnt) {
        GetIntervalTable().emplace(Name, std::make_unique<FPnt>(*defFPnt));
        return std::make_unique<Definition>(Name, std::move(defFPnt), std::move(Expr));
    } else {
        return std::make_unique<Definition>(Name, std::move(Expr));
    }
}



std::unique_ptr<FPnt> Parser::ParseFloatingPointNotation() {
    NextToken(); // 'in'
    if (CurrentToken != '(') throw std::runtime_error("Expected '(' after 'in'");
    NextToken(); // '('

    if (CurrentToken != '[') throw std::runtime_error("Expected '['");
    NextToken(); // '['

    if (CurrentToken != tok_number) throw std::runtime_error("Expected lower bound number");
    double lowerBound = NumVal;
    NextToken(); 

    if (CurrentToken != ',') throw std::runtime_error("Expected ',' after lower bound");
    NextToken(); // ','

    if (CurrentToken != tok_number) throw std::runtime_error("Expected upper bound number");
    double upperBound = NumVal;
    NextToken(); 

    if (CurrentToken != ']') throw std::runtime_error("Expected ']' after upper bound");
    NextToken(); // ']'

    if (CurrentToken != ',') throw std::runtime_error("Expected ',' after ']'");
    NextToken(); // ','

    if (CurrentToken != tok_number) throw std::runtime_error("Expected number for decimal bits");
    int decimalBits = NumVal;
    NextToken(); // 

    if (CurrentToken != ')') throw std::runtime_error("Expected ')' after decimal bit number");
    NextToken(); // ')'

    return std::make_unique<FPnt>(lowerBound, upperBound, decimalBits);
}


std::unique_ptr<AST> Parser::ParseExpression() {
    auto left = ParseTerm();
    while (CurrentToken == '+' || CurrentToken == '-') {
        char op = CurrentToken;
        NextToken();
        auto right = ParseTerm();
        left = std::make_unique<BinOpExpr>(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<AST> Parser::ParseTerm() {
    auto left = ParseFactor();
    while (CurrentToken == '*' || CurrentToken == '/') {
        char op = CurrentToken;
        NextToken();
        auto right = ParseFactor();
        left = std::make_unique<BinOpExpr>(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<AST> Parser::ParseFactor() {
    if (CurrentToken == tok_number) {
        auto result = std::make_unique<NumExpr>(NumVal);
        NextToken(); // consume the number
        return result;
    } else if (CurrentToken == tok_identifier) {
        std::string name = IdentifierStr;
        NextToken(); // consume the identifier
        return std::make_unique<NameExpr>(name);
    } else if (CurrentToken == '(') {
        NextToken(); // consume '('
        auto expr = ParseExpression();
        if (CurrentToken != ')') throw std::runtime_error("Expected ')'");
        NextToken(); // consume ')'
        return expr;
    } else {
        throw std::runtime_error("Unexpected token in expression");
    }
}





