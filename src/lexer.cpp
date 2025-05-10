#include "lexer.h"
#include <cctype>

using namespace std;

unordered_map<string, TokenType> Lexer::keywords = {
    {"int", TOKEN_INT},
    {"float", TOKEN_FLOAT},
    {"string", TOKEN_STRING},
    {"bool", TOKEN_BOOL},
    {"void", TOKEN_VOID},
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    {"function", TOKEN_FUNCTION},
    {"return", TOKEN_RETURN},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"print", TOKEN_PRINT}};

Lexer::Lexer(const string &input)
    : input(input), pos(0), line(1), column(1) {}

char Lexer::current() const
{
    return pos < input.length() ? input[pos] : '\0';
}

char Lexer::lookahead() const
{
    return pos + 1 < input.length() ? input[pos + 1] : '\0';
}

void Lexer::advance()
{
    if (current() == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace()
{
    while (isspace(current()))
    {
        advance();
    }
}

void Lexer::skipComment()
{
    if (current() == '/' && lookahead() == '/')
    {
        while (current() != '\n' && current() != '\0')
        {
            advance();
        }
    }
}

Token Lexer::identifier()
{
    size_t start = pos;
    int startColumn = column;

    while (isalnum(current()) || current() == '_')
    {
        advance();
    }

    string text = input.substr(start, pos - start);
    TokenType type = TOKEN_IDENT;

    auto it = keywords.find(text);
    if (it != keywords.end())
    {
        type = it->second;
    }

    return Token(type, text, line, startColumn);
}

Token Lexer::number()
{
    size_t start = pos;
    int startColumn = column;
    bool isFloat = false;

    while (isdigit(current()))
    {
        advance();
    }

    if (current() == '.' && isdigit(lookahead()))
    {
        isFloat = true;
        advance();
        while (isdigit(current()))
        {
            advance();
        }
    }

    string text = input.substr(start, pos - start);
    TokenType type = isFloat ? TOKEN_FLOAT_LITERAL : TOKEN_INT_LITERAL;

    return Token(type, text, line, startColumn);
}

Token Lexer::stringLiteral()
{
    advance();
    size_t start = pos;
    int startColumn = column;

    while (current() != '"' && current() != '\0')
    {
        if (current() == '\\')
        {
            advance();
            if (current() != '\0')
                advance();
        }
        else
        {
            advance();
        }
    }

    if (current() == '\0')
    {
        return Token(TOKEN_ERROR, "Unterminated string", line, startColumn);
    }

    string text = input.substr(start, pos - start);
    advance();

    return Token(TOKEN_STRING_LITERAL, text, line, startColumn);
}

Token Lexer::nextToken()
{
    skipWhitespace();
    skipComment();

    while (current() == '/' && lookahead() == '/')
    {
        skipComment();
        skipWhitespace();
    }

    if (current() == '\0')
    {
        return Token(TOKEN_EOF, "", line, column);
    }

    int startColumn = column;

    if (isalpha(current()) || current() == '_')
    {
        return identifier();
    }

    if (isdigit(current()))
    {
        return number();
    }

    if (current() == '"')
    {
        return stringLiteral();
    }

    char c = current();
    advance();

    switch (c)
    {
    case '=':
        if (current() == '=')
        {
            advance();
            return Token(TOKEN_EQUAL, "==", line, startColumn);
        }
        return Token(TOKEN_ASSIGN, "=", line, startColumn);
    case '!':
        if (current() == '=')
        {
            advance();
            return Token(TOKEN_NOT_EQUAL, "!=", line, startColumn);
        }
        return Token(TOKEN_NOT, "!", line, startColumn);
    case '<':
        if (current() == '=')
        {
            advance();
            return Token(TOKEN_LESS_EQUAL, "<=", line, startColumn);
        }
        return Token(TOKEN_LESS, "<", line, startColumn);
    case '>':
        if (current() == '=')
        {
            advance();
            return Token(TOKEN_GREATER_EQUAL, ">=", line, startColumn);
        }
        return Token(TOKEN_GREATER, ">", line, startColumn);
    case '&':
        if (current() == '&')
        {
            advance();
            return Token(TOKEN_AND, "&&", line, startColumn);
        }
        return Token(TOKEN_ERROR, "&", line, startColumn);
    case '|':
        if (current() == '|')
        {
            advance();
            return Token(TOKEN_OR, "||", line, startColumn);
        }
        return Token(TOKEN_ERROR, "|", line, startColumn);
    case '+':
        return Token(TOKEN_PLUS, "+", line, startColumn);
    case '-':
        return Token(TOKEN_MINUS, "-", line, startColumn);
    case '*':
        return Token(TOKEN_STAR, "*", line, startColumn);
    case '/':
        return Token(TOKEN_SLASH, "/", line, startColumn);
    case '%':
        return Token(TOKEN_PERCENT, "%", line, startColumn);
    case '(':
        return Token(TOKEN_LPAREN, "(", line, startColumn);
    case ')':
        return Token(TOKEN_RPAREN, ")", line, startColumn);
    case '{':
        return Token(TOKEN_LBRACE, "{", line, startColumn);
    case '}':
        return Token(TOKEN_RBRACE, "}", line, startColumn);
    case '[':
        return Token(TOKEN_LBRACKET, "[", line, startColumn);
    case ']':
        return Token(TOKEN_RBRACKET, "]", line, startColumn);
    case ';':
        return Token(TOKEN_SEMICOLON, ";", line, startColumn);
    case ',':
        return Token(TOKEN_COMMA, ",", line, startColumn);
    case '.':
        return Token(TOKEN_DOT, ".", line, startColumn);
    default:
        return Token(TOKEN_ERROR, string(1, c), line, startColumn);
    }
}