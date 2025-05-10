#include "ast.h"

void Program::accept(ASTVisitor *visitor) { visitor->visitProgram(this); }
void Function::accept(ASTVisitor *visitor) { visitor->visitFunction(this); }
void VarDeclaration::accept(ASTVisitor *visitor) { visitor->visitVarDeclaration(this); }
void Assignment::accept(ASTVisitor *visitor) { visitor->visitAssignment(this); }
void Block::accept(ASTVisitor *visitor) { visitor->visitBlock(this); }
void IfStatement::accept(ASTVisitor *visitor) { visitor->visitIfStatement(this); }
void WhileStatement::accept(ASTVisitor *visitor) { visitor->visitWhileStatement(this); }
void ForStatement::accept(ASTVisitor *visitor) { visitor->visitForStatement(this); }
void ReturnStatement::accept(ASTVisitor *visitor) { visitor->visitReturnStatement(this); }
void PrintStatement::accept(ASTVisitor *visitor) { visitor->visitPrintStatement(this); }
void ExpressionStatement::accept(ASTVisitor *visitor) { visitor->visitExpressionStatement(this); }
void IntLiteral::accept(ASTVisitor *visitor) { visitor->visitIntLiteral(this); }
void FloatLiteral::accept(ASTVisitor *visitor) { visitor->visitFloatLiteral(this); }
void StringLiteral::accept(ASTVisitor *visitor) { visitor->visitStringLiteral(this); }
void BoolLiteral::accept(ASTVisitor *visitor) { visitor->visitBoolLiteral(this); }
void Variable::accept(ASTVisitor *visitor) { visitor->visitVariable(this); }
void ArrayAccess::accept(ASTVisitor *visitor) { visitor->visitArrayAccess(this); }
void BinaryOp::accept(ASTVisitor *visitor) { visitor->visitBinaryOp(this); }
void UnaryOp::accept(ASTVisitor *visitor) { visitor->visitUnaryOp(this); }
void FunctionCall::accept(ASTVisitor *visitor) { visitor->visitFunctionCall(this); }