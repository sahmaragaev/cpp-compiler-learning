#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <string>
#include "types.h"

using namespace std;

class ASTVisitor;

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor *visitor) = 0;
};

class Expression : public ASTNode
{
public:
    shared_ptr<Type> type;
};

class Statement : public ASTNode
{
};

class IntLiteral : public Expression
{
public:
    int value;

    IntLiteral(int value) : value(value) { type = IntType; }
    void accept(ASTVisitor *visitor) override;
};

class FloatLiteral : public Expression
{
public:
    float value;

    FloatLiteral(float value) : value(value) { type = FloatType; }
    void accept(ASTVisitor *visitor) override;
};

class StringLiteral : public Expression
{
public:
    string value;

    StringLiteral(const string &value) : value(value) { type = StringType; }
    void accept(ASTVisitor *visitor) override;
};

class BoolLiteral : public Expression
{
public:
    bool value;

    BoolLiteral(bool value) : value(value) { type = BoolType; }
    void accept(ASTVisitor *visitor) override;
};

class Variable : public Expression
{
public:
    string name;

    Variable(const string &name) : name(name) {}
    void accept(ASTVisitor *visitor) override;
};

class ArrayAccess : public Expression
{
public:
    shared_ptr<Expression> array;
    shared_ptr<Expression> index;

    ArrayAccess(shared_ptr<Expression> array, shared_ptr<Expression> index)
        : array(array), index(index) {}
    void accept(ASTVisitor *visitor) override;
};

class BinaryOp : public Expression
{
public:
    string op;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

    BinaryOp(const string &op, shared_ptr<Expression> left,
             shared_ptr<Expression> right)
        : op(op), left(left), right(right) {}
    void accept(ASTVisitor *visitor) override;
};

class UnaryOp : public Expression
{
public:
    string op;
    shared_ptr<Expression> expr;

    UnaryOp(const string &op, shared_ptr<Expression> expr)
        : op(op), expr(expr) {}
    void accept(ASTVisitor *visitor) override;
};

class FunctionCall : public Expression
{
public:
    string name;
    vector<shared_ptr<Expression>> args;

    FunctionCall(const string &name,
                 const vector<shared_ptr<Expression>> &args)
        : name(name), args(args) {}
    void accept(ASTVisitor *visitor) override;
};

class VarDeclaration : public Statement
{
public:
    shared_ptr<Type> type;
    string name;
    shared_ptr<Expression> initializer;

    VarDeclaration(shared_ptr<Type> type, const string &name,
                   shared_ptr<Expression> initializer = nullptr)
        : type(type), name(name), initializer(initializer) {}
    void accept(ASTVisitor *visitor) override;
};

class Assignment : public Statement
{
public:
    shared_ptr<Expression> target;
    shared_ptr<Expression> value;

    Assignment(shared_ptr<Expression> target, shared_ptr<Expression> value)
        : target(target), value(value) {}
    void accept(ASTVisitor *visitor) override;
};

class Block : public Statement
{
public:
    vector<shared_ptr<Statement>> statements;

    Block(const vector<shared_ptr<Statement>> &statements)
        : statements(statements) {}
    void accept(ASTVisitor *visitor) override;
};

class IfStatement : public Statement
{
public:
    shared_ptr<Expression> condition;
    shared_ptr<Statement> thenBranch;
    shared_ptr<Statement> elseBranch;

    IfStatement(shared_ptr<Expression> condition,
                shared_ptr<Statement> thenBranch,
                shared_ptr<Statement> elseBranch = nullptr)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
    void accept(ASTVisitor *visitor) override;
};

class WhileStatement : public Statement
{
public:
    shared_ptr<Expression> condition;
    shared_ptr<Statement> body;

    WhileStatement(shared_ptr<Expression> condition,
                   shared_ptr<Statement> body)
        : condition(condition), body(body) {}
    void accept(ASTVisitor *visitor) override;
};

class ForStatement : public Statement
{
public:
    shared_ptr<Statement> init;
    shared_ptr<Expression> condition;
    shared_ptr<Statement> update;
    shared_ptr<Statement> body;

    ForStatement(shared_ptr<Statement> init,
                 shared_ptr<Expression> condition,
                 shared_ptr<Statement> update,
                 shared_ptr<Statement> body)
        : init(init), condition(condition), update(update), body(body) {}
    void accept(ASTVisitor *visitor) override;
};

class ReturnStatement : public Statement
{
public:
    shared_ptr<Expression> value;

    ReturnStatement(shared_ptr<Expression> value = nullptr)
        : value(value) {}
    void accept(ASTVisitor *visitor) override;
};

class PrintStatement : public Statement
{
public:
    shared_ptr<Expression> expr;

    PrintStatement(shared_ptr<Expression> expr) : expr(expr) {}
    void accept(ASTVisitor *visitor) override;
};

class ExpressionStatement : public Statement
{
public:
    shared_ptr<Expression> expr;

    ExpressionStatement(shared_ptr<Expression> expr) : expr(expr) {}
    void accept(ASTVisitor *visitor) override;
};

class Parameter
{
public:
    shared_ptr<Type> type;
    string name;

    Parameter(shared_ptr<Type> type, const string &name)
        : type(type), name(name) {}
};

class Function : public ASTNode
{
public:
    shared_ptr<Type> returnType;
    string name;
    vector<Parameter> parameters;
    shared_ptr<Block> body;

    Function(shared_ptr<Type> returnType, const string &name,
             const vector<Parameter> &parameters,
             shared_ptr<Block> body)
        : returnType(returnType), name(name), parameters(parameters), body(body) {}
    void accept(ASTVisitor *visitor) override;
};

class Program : public ASTNode
{
public:
    vector<shared_ptr<ASTNode>> declarations;

    Program(const vector<shared_ptr<ASTNode>> &declarations)
        : declarations(declarations) {}
    void accept(ASTVisitor *visitor) override;
};

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;

    virtual void visitProgram(Program *node) = 0;
    virtual void visitFunction(Function *node) = 0;
    virtual void visitVarDeclaration(VarDeclaration *node) = 0;
    virtual void visitAssignment(Assignment *node) = 0;
    virtual void visitBlock(Block *node) = 0;
    virtual void visitIfStatement(IfStatement *node) = 0;
    virtual void visitWhileStatement(WhileStatement *node) = 0;
    virtual void visitForStatement(ForStatement *node) = 0;
    virtual void visitReturnStatement(ReturnStatement *node) = 0;
    virtual void visitPrintStatement(PrintStatement *node) = 0;
    virtual void visitExpressionStatement(ExpressionStatement *node) = 0;
    virtual void visitIntLiteral(IntLiteral *node) = 0;
    virtual void visitFloatLiteral(FloatLiteral *node) = 0;
    virtual void visitStringLiteral(StringLiteral *node) = 0;
    virtual void visitBoolLiteral(BoolLiteral *node) = 0;
    virtual void visitVariable(Variable *node) = 0;
    virtual void visitArrayAccess(ArrayAccess *node) = 0;
    virtual void visitBinaryOp(BinaryOp *node) = 0;
    virtual void visitUnaryOp(UnaryOp *node) = 0;
    virtual void visitFunctionCall(FunctionCall *node) = 0;
};

#endif 