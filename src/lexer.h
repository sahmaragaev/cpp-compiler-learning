#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <unordered_map>

using namespace std;

class Lexer
{
private:
    string input;
    size_t pos;
    int line;
    int column;

    static unordered_map<string, TokenType> keywords;

    char current() const;
    char lookahead() const;
    void advance();
    void skipWhitespace();
    void skipComment();

    Token identifier();
    Token number();
    Token stringLiteral();

public:
    Lexer(const string &input);
    Token nextToken();
};

#endif 
