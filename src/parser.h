#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <vector>

using namespace std;

class Parser
{
private:
    Lexer lexer;
    Token currentToken;

    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const string &message);

    shared_ptr<Type> parseType();
    shared_ptr<Program> parseProgram();
    shared_ptr<Function> parseFunction();
    shared_ptr<Statement> parseStatement();
    shared_ptr<VarDeclaration> parseVarDeclaration();
    shared_ptr<Block> parseBlock();
    shared_ptr<IfStatement> parseIfStatement();
    shared_ptr<WhileStatement> parseWhileStatement();
    shared_ptr<ForStatement> parseForStatement();
    shared_ptr<ReturnStatement> parseReturnStatement();
    shared_ptr<PrintStatement> parsePrintStatement();
    shared_ptr<Statement> parseExpressionStatement();

    shared_ptr<Expression> parseExpression();
    shared_ptr<Expression> parseAssignment();
    shared_ptr<Expression> parseLogicalOr();
    shared_ptr<Expression> parseLogicalAnd();
    shared_ptr<Expression> parseEquality();
    shared_ptr<Expression> parseComparison();
    shared_ptr<Expression> parseAddition();
    shared_ptr<Expression> parseMultiplication();
    shared_ptr<Expression> parseUnary();
    shared_ptr<Expression> parsePostfix();
    shared_ptr<Expression> parsePrimary();

public:
    Parser(const string &input);
    shared_ptr<Program> parse();
};

#endif 