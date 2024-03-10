#include <string>

using namespace std;

enum Token {
    tok_eof = -1,

    // commands/defintions
    tok_def = -2,
    tok_in = -3,

    // primary
    tok_identifier = -4,
    tok_number = -5
};

extern string IdentifierStr;
extern double NumVal;

int gettok();