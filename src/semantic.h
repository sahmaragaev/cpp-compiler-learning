#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

using namespace std;

class SemanticAnalyzer : public ASTVisitor
{
private:
    SymbolTable symbolTable;
    shared_ptr<Type> currentFunctionReturnType;

    shared_ptr<Type> checkBinaryOp(const string &op,
                                        shared_ptr<Type> left,
                                        shared_ptr<Type> right);
    shared_ptr<Type> checkUnaryOp(const string &op,
                                       shared_ptr<Type> operand);
    bool isNumericType(shared_ptr<Type> type);
    bool isAssignable(shared_ptr<Type> target, shared_ptr<Type> value);

public:
    SemanticAnalyzer();

    void analyze(shared_ptr<Program> program);

    void visitProgram(Program *node) override;
    void visitFunction(Function *node) override;
    void visitVarDeclaration(VarDeclaration *node) override;
    void visitAssignment(Assignment *node) override;
    void visitBlock(Block *node) override;
    void visitIfStatement(IfStatement *node) override;
    void visitWhileStatement(WhileStatement *node) override;
    void visitForStatement(ForStatement *node) override;
    void visitReturnStatement(ReturnStatement *node) override;
    void visitPrintStatement(PrintStatement *node) override;
    void visitExpressionStatement(ExpressionStatement *node) override;
    void visitIntLiteral(IntLiteral *node) override;
    void visitFloatLiteral(FloatLiteral *node) override;
    void visitStringLiteral(StringLiteral *node) override;
    void visitBoolLiteral(BoolLiteral *node) override;
    void visitVariable(Variable *node) override;
    void visitArrayAccess(ArrayAccess *node) override;
    void visitBinaryOp(BinaryOp *node) override;
    void visitUnaryOp(UnaryOp *node) override;
    void visitFunctionCall(FunctionCall *node) override;
};

#endif 