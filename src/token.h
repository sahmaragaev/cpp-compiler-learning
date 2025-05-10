#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum TokenType {
    TOKEN_EOF,
    TOKEN_ERROR,
    
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_BOOL,
    
    TOKEN_IDENT,
    TOKEN_INT_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_PRINT,
    TOKEN_VOID,
    
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT
};

struct Token {
    TokenType type;
    string text;
    int line;
    int column;
    
    Token(TokenType type = TOKEN_EOF, const string& text = "", 
          int line = 0, int column = 0)
        : type(type), text(text), line(line), column(column) {}
};

#endif 