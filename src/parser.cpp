#include "parser.h"
#include "error.h"
#include <sstream>

using namespace std;

Parser::Parser(const string &input) : lexer(input)
{
    advance();
}

void Parser::advance()
{
    currentToken = lexer.nextToken();
}

bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type)
{
    return currentToken.type == type;
}

void Parser::consume(TokenType type, const string &message)
{
    if (check(type))
    {
        advance();
        return;
    }
    errorReporter.reportError(message, currentToken.line, currentToken.column);
}

shared_ptr<Type> Parser::parseType()
{
    shared_ptr<Type> baseType;

    if (match(TOKEN_INT))
    {
        baseType = IntType;
    }
    else if (match(TOKEN_FLOAT))
    {
        baseType = FloatType;
    }
    else if (match(TOKEN_STRING))
    {
        baseType = StringType;
    }
    else if (match(TOKEN_BOOL))
    {
        baseType = BoolType;
    }
    else if (match(TOKEN_VOID))
    {
        baseType = VoidType;
    }
    else
    {
        errorReporter.reportError("Expected type", currentToken.line, currentToken.column);
        return ErrorType;
    }

    if (match(TOKEN_LBRACKET))
    {
        if (currentToken.type != TOKEN_INT_LITERAL)
        {
            errorReporter.reportError("Expected array size", currentToken.line, currentToken.column);
            return ErrorType;
        }
        int size = stoi(currentToken.text);
        advance();
        consume(TOKEN_RBRACKET, "Expected ']'");
        return make_shared<ArrayType>(baseType, size);
    }

    return baseType;
}

shared_ptr<Program> Parser::parseProgram()
{
    vector<shared_ptr<ASTNode>> declarations;

    while (!check(TOKEN_EOF) && !check(TOKEN_ERROR))
    {
        shared_ptr<ASTNode> decl;
        if (check(TOKEN_FUNCTION))
        {
            decl = parseFunction();
        }
        else
        {
            decl = parseStatement();
        }

        if (decl)
        {
            declarations.push_back(decl);
        }

        if (errorReporter.hadError())
        {
            break;
        }
    }

    return make_shared<Program>(declarations);
}

shared_ptr<Function> Parser::parseFunction()
{
    consume(TOKEN_FUNCTION, "Expected 'function'");

    shared_ptr<Type> returnType = parseType();

    if (currentToken.type != TOKEN_IDENT)
    {
        errorReporter.reportError("Expected function name", currentToken.line, currentToken.column);
        return nullptr;
    }

    string name = currentToken.text;
    advance();

    consume(TOKEN_LPAREN, "Expected '('");

    vector<Parameter> parameters;

    if (!check(TOKEN_RPAREN))
    {
        do
        {
            shared_ptr<Type> paramType = parseType();

            if (currentToken.type != TOKEN_IDENT)
            {
                errorReporter.reportError("Expected parameter name", currentToken.line, currentToken.column);
                return nullptr;
            }

            string paramName = currentToken.text;
            advance();

            parameters.push_back(Parameter(paramType, paramName));
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expected ')'");
    consume(TOKEN_LBRACE, "Expected '{'");

    shared_ptr<Block> body = parseBlock();

    return make_shared<Function>(returnType, name, parameters, body);
}

shared_ptr<Statement> Parser::parseStatement()
{
    if (check(TOKEN_INT) || check(TOKEN_FLOAT) || check(TOKEN_STRING) || check(TOKEN_BOOL))
    {
        return parseVarDeclaration();
    }

    if (match(TOKEN_LBRACE))
    {
        return parseBlock();
    }

    if (check(TOKEN_IF))
    {
        return parseIfStatement();
    }

    if (check(TOKEN_WHILE))
    {
        return parseWhileStatement();
    }

    if (check(TOKEN_FOR))
    {
        return parseForStatement();
    }

    if (check(TOKEN_RETURN))
    {
        return parseReturnStatement();
    }

    if (check(TOKEN_PRINT))
    {
        return parsePrintStatement();
    }

    return parseExpressionStatement();
}

shared_ptr<VarDeclaration> Parser::parseVarDeclaration()
{
    shared_ptr<Type> type = parseType();

    if (currentToken.type != TOKEN_IDENT)
    {
        errorReporter.reportError("Expected variable name", currentToken.line, currentToken.column);
        return nullptr;
    }

    string name = currentToken.text;
    advance();

    shared_ptr<Expression> initializer = nullptr;

    if (match(TOKEN_ASSIGN))
    {
        initializer = parseExpression();
    }

    consume(TOKEN_SEMICOLON, "Expected ';'");

    return make_shared<VarDeclaration>(type, name, initializer);
}

shared_ptr<Block> Parser::parseBlock()
{
    vector<shared_ptr<Statement>> statements;

    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF))
    {
        auto stmt = parseStatement();
        if (stmt)
        {
            statements.push_back(stmt);
        }
        if (errorReporter.hadError())
        {
            break;
        }
    }

    consume(TOKEN_RBRACE, "Expected '}'");

    return make_shared<Block>(statements);
}

shared_ptr<IfStatement> Parser::parseIfStatement()
{
    consume(TOKEN_IF, "Expected 'if'");
    consume(TOKEN_LPAREN, "Expected '('");

    shared_ptr<Expression> condition = parseExpression();

    consume(TOKEN_RPAREN, "Expected ')'");

    shared_ptr<Statement> thenBranch = parseStatement();
    shared_ptr<Statement> elseBranch = nullptr;

    if (match(TOKEN_ELSE))
    {
        elseBranch = parseStatement();
    }

    return make_shared<IfStatement>(condition, thenBranch, elseBranch);
}

shared_ptr<WhileStatement> Parser::parseWhileStatement()
{
    consume(TOKEN_WHILE, "Expected 'while'");
    consume(TOKEN_LPAREN, "Expected '('");

    shared_ptr<Expression> condition = parseExpression();

    consume(TOKEN_RPAREN, "Expected ')'");

    shared_ptr<Statement> body = parseStatement();

    return make_shared<WhileStatement>(condition, body);
}

shared_ptr<ForStatement> Parser::parseForStatement()
{
    consume(TOKEN_FOR, "Expected 'for'");
    consume(TOKEN_LPAREN, "Expected '('");

    shared_ptr<Statement> init = nullptr;
    if (!check(TOKEN_SEMICOLON))
    {
        if (check(TOKEN_INT) || check(TOKEN_FLOAT) || check(TOKEN_STRING) || check(TOKEN_BOOL))
        {
            init = parseVarDeclaration();
        }
        else
        {
            init = parseExpressionStatement();
        }
    }
    else
    {
        advance();
    }

    shared_ptr<Expression> condition = nullptr;
    if (!check(TOKEN_SEMICOLON))
    {
        condition = parseExpression();
    }
    consume(TOKEN_SEMICOLON, "Expected ';'");

    shared_ptr<Statement> update = nullptr;
    if (!check(TOKEN_RPAREN))
    {
        shared_ptr<Expression> updateExpr = parseExpression();
        update = make_shared<ExpressionStatement>(updateExpr);
    }

    consume(TOKEN_RPAREN, "Expected ')'");

    shared_ptr<Statement> body = parseStatement();

    return make_shared<ForStatement>(init, condition, update, body);
}

shared_ptr<ReturnStatement> Parser::parseReturnStatement()
{
    consume(TOKEN_RETURN, "Expected 'return'");

    shared_ptr<Expression> value = nullptr;

    if (!check(TOKEN_SEMICOLON))
    {
        value = parseExpression();
    }

    consume(TOKEN_SEMICOLON, "Expected ';'");

    return make_shared<ReturnStatement>(value);
}

shared_ptr<PrintStatement> Parser::parsePrintStatement()
{
    consume(TOKEN_PRINT, "Expected 'print'");
    consume(TOKEN_LPAREN, "Expected '('");

    shared_ptr<Expression> expr = parseExpression();

    consume(TOKEN_RPAREN, "Expected ')'");
    consume(TOKEN_SEMICOLON, "Expected ';'");

    return make_shared<PrintStatement>(expr);
}

shared_ptr<Statement> Parser::parseExpressionStatement()
{
    shared_ptr<Expression> expr = parseExpression();
    if (!expr)
    {

        while (!check(TOKEN_SEMICOLON) && !check(TOKEN_EOF))
        {
            advance();
        }
    }
    consume(TOKEN_SEMICOLON, "Expected ';'");
    return expr ? make_shared<ExpressionStatement>(expr) : nullptr;
}

shared_ptr<Expression> Parser::parseExpression()
{
    return parseAssignment();
}

shared_ptr<Expression> Parser::parseAssignment()
{
    shared_ptr<Expression> expr = parseLogicalOr();
    if (!expr)
        return nullptr;

    if (match(TOKEN_ASSIGN))
    {
        shared_ptr<Expression> value = parseAssignment();
        if (!value)
            return nullptr;
        return make_shared<BinaryOp>("=", expr, value);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseLogicalOr()
{
    shared_ptr<Expression> expr = parseLogicalAnd();
    if (!expr)
        return nullptr;

    while (match(TOKEN_OR))
    {
        string op = "||";
        shared_ptr<Expression> right = parseLogicalAnd();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseLogicalAnd()
{
    shared_ptr<Expression> expr = parseEquality();
    if (!expr)
        return nullptr;

    while (match(TOKEN_AND))
    {
        string op = "&&";
        shared_ptr<Expression> right = parseEquality();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseEquality()
{
    shared_ptr<Expression> expr = parseComparison();
    if (!expr)
        return nullptr;

    while (check(TOKEN_EQUAL) || check(TOKEN_NOT_EQUAL))
    {
        TokenType type = currentToken.type;
        advance();
        string op = (type == TOKEN_EQUAL) ? "==" : "!=";
        shared_ptr<Expression> right = parseComparison();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseComparison()
{
    shared_ptr<Expression> expr = parseAddition();
    if (!expr)
        return nullptr;

    while (check(TOKEN_LESS) || check(TOKEN_LESS_EQUAL) ||
           check(TOKEN_GREATER) || check(TOKEN_GREATER_EQUAL))
    {
        TokenType type = currentToken.type;
        advance();
        string op;
        switch (type)
        {
        case TOKEN_LESS:
            op = "<";
            break;
        case TOKEN_LESS_EQUAL:
            op = "<=";
            break;
        case TOKEN_GREATER:
            op = ">";
            break;
        case TOKEN_GREATER_EQUAL:
            op = ">=";
            break;
        default:
            op = "";
        }
        shared_ptr<Expression> right = parseAddition();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseAddition()
{
    shared_ptr<Expression> expr = parseMultiplication();
    if (!expr)
        return nullptr;

    while (check(TOKEN_PLUS) || check(TOKEN_MINUS))
    {
        TokenType type = currentToken.type;
        advance();
        string op = (type == TOKEN_PLUS) ? "+" : "-";
        shared_ptr<Expression> right = parseMultiplication();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseMultiplication()
{
    shared_ptr<Expression> expr = parseUnary();
    if (!expr)
        return nullptr;

    while (check(TOKEN_STAR) || check(TOKEN_SLASH) || check(TOKEN_PERCENT))
    {
        TokenType type = currentToken.type;
        advance();
        string op;
        switch (type)
        {
        case TOKEN_STAR:
            op = "*";
            break;
        case TOKEN_SLASH:
            op = "/";
            break;
        case TOKEN_PERCENT:
            op = "%";
            break;
        default:
            op = "";
        }
        shared_ptr<Expression> right = parseUnary();
        if (!right)
            return nullptr;
        expr = make_shared<BinaryOp>(op, expr, right);
    }

    return expr;
}

shared_ptr<Expression> Parser::parseUnary()
{
    if (check(TOKEN_NOT) || check(TOKEN_MINUS))
    {
        TokenType type = currentToken.type;
        advance();
        string op = (type == TOKEN_NOT) ? "!" : "-";
        shared_ptr<Expression> expr = parseUnary();
        if (!expr)
            return nullptr;
        return make_shared<UnaryOp>(op, expr);
    }

    return parsePostfix();
}

shared_ptr<Expression> Parser::parsePostfix()
{
    shared_ptr<Expression> expr = parsePrimary();
    if (!expr)
        return nullptr;

    while (true)
    {
        if (match(TOKEN_LBRACKET))
        {
            shared_ptr<Expression> index = parseExpression();
            if (!index)
                return nullptr;
            consume(TOKEN_RBRACKET, "Expected ']'");
            expr = make_shared<ArrayAccess>(expr, index);
        }
        else if (match(TOKEN_LPAREN))
        {
            vector<shared_ptr<Expression>> args;

            if (!check(TOKEN_RPAREN))
            {
                do
                {
                    auto arg = parseExpression();
                    if (arg)
                    {
                        args.push_back(arg);
                    }
                } while (match(TOKEN_COMMA));
            }

            consume(TOKEN_RPAREN, "Expected ')'");

            if (auto var = dynamic_pointer_cast<Variable>(expr))
            {
                expr = make_shared<FunctionCall>(var->name, args);
            }
            else
            {
                errorReporter.reportError("Function call must be on identifier",
                                          currentToken.line, currentToken.column);
            }
        }
        else
        {
            break;
        }
    }

    return expr;
}

shared_ptr<Expression> Parser::parsePrimary()
{
    if (check(TOKEN_INT_LITERAL))
    {
        string text = currentToken.text;
        advance();
        int value = stoi(text);
        return make_shared<IntLiteral>(value);
    }

    if (check(TOKEN_FLOAT_LITERAL))
    {
        string text = currentToken.text;
        advance();
        float value = stof(text);
        return make_shared<FloatLiteral>(value);
    }

    if (check(TOKEN_STRING_LITERAL))
    {
        string text = currentToken.text;
        advance();
        return make_shared<StringLiteral>(text);
    }

    if (match(TOKEN_TRUE))
    {
        return make_shared<BoolLiteral>(true);
    }

    if (match(TOKEN_FALSE))
    {
        return make_shared<BoolLiteral>(false);
    }

    if (check(TOKEN_IDENT))
    {
        string name = currentToken.text;
        advance();
        return make_shared<Variable>(name);
    }

    if (match(TOKEN_LPAREN))
    {
        shared_ptr<Expression> expr = parseExpression();
        consume(TOKEN_RPAREN, "Expected ')'");
        return expr;
    }

    errorReporter.reportError("Expected expression", currentToken.line, currentToken.column);
    advance();
    return nullptr;
}

shared_ptr<Program> Parser::parse()
{
    return parseProgram();
}