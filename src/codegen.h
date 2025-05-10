#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <iostream>
#include <string>

using namespace std;

class CodeGenerator : public ASTVisitor
{
private:
    ostream &output;
    int indent;

    void writeIndent();
    void write(const string &text);
    void writeLine(const string &text);
    string getCType(shared_ptr<Type> type);

public:
    CodeGenerator(ostream &output);

    void generate(shared_ptr<Program> program);

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