#include <vector>
#include <string>
#include <memory>
#include <cassert>

using namespace std;

#include "lexer/lexer.h"

class ExprAST;
class DefinitionAST;

extern int CurTok;
int getNextToken() {
  return CurTok = gettok();
}

class ParseState {
  public:
    int Position;
    vector<string> Tokens;

    ParseState(vector<string>& Tokens_) : Position(0), Tokens(Tokens_) {}

    bool AtEnd() {
      return Position == (int) Tokens.size();
    }

    string peek() {
      if (AtEnd()) {
        return "";
      }
      return Tokens.at(Position);
    }

    string peek(const int Offset) {
      assert(Position + Offset < Tokens.size());
      return Tokens.at(Position + Offset);
    }

    string eat() {
      auto Current = peek();
      Position++;
      return Current;
    }
};

std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseIdentifierExpr();
std::unique_ptr<ExprAST> ParsePrimary();
std::unique_ptr<DefinitionAST> ParseDefinition();