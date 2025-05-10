#include "semantic.h"
#include "error.h"

using namespace std;

SemanticAnalyzer::SemanticAnalyzer() : currentFunctionReturnType(nullptr) {}

void SemanticAnalyzer::analyze(shared_ptr<Program> program)
{
    program->accept(this);
}

bool SemanticAnalyzer::isNumericType(shared_ptr<Type> type)
{
    return type->kind == TypeKind::INT || type->kind == TypeKind::FLOAT;
}

bool SemanticAnalyzer::isAssignable(shared_ptr<Type> target, shared_ptr<Type> value)
{
    if (target->equals(value.get()))
    {
        return true;
    }

    if (target->kind == TypeKind::FLOAT && value->kind == TypeKind::INT)
    {
        return true;
    }

    return false;
}

shared_ptr<Type> SemanticAnalyzer::checkBinaryOp(const string &op,
                                                      shared_ptr<Type> left,
                                                      shared_ptr<Type> right)
{
    if (op == "=")
    {
        if (!isAssignable(left, right))
        {
            errorReporter.reportError("Type mismatch in assignment");
            return ErrorType;
        }
        return left;
    }

    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%")
    {
        if (!isNumericType(left) || !isNumericType(right))
        {
            errorReporter.reportError("Numeric operands required for " + op);
            return ErrorType;
        }

        if (left->kind == TypeKind::FLOAT || right->kind == TypeKind::FLOAT)
        {
            return FloatType;
        }
        return IntType;
    }

    if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=")
    {
        if (!isNumericType(left) || !isNumericType(right))
        {
            if (!left->equals(right.get()))
            {
                errorReporter.reportError("Type mismatch in comparison");
                return ErrorType;
            }
        }
        return BoolType;
    }

    if (op == "&&" || op == "||")
    {
        if (left->kind != TypeKind::BOOL || right->kind != TypeKind::BOOL)
        {
            errorReporter.reportError("Boolean operands required for " + op);
            return ErrorType;
        }
        return BoolType;
    }

    errorReporter.reportError("Unknown binary operator: " + op);
    return ErrorType;
}

shared_ptr<Type> SemanticAnalyzer::checkUnaryOp(const string &op,
                                                     shared_ptr<Type> operand)
{
    if (op == "-")
    {
        if (!isNumericType(operand))
        {
            errorReporter.reportError("Numeric operand required for unary -");
            return ErrorType;
        }
        return operand;
    }

    if (op == "!")
    {
        if (operand->kind != TypeKind::BOOL)
        {
            errorReporter.reportError("Boolean operand required for !");
            return ErrorType;
        }
        return BoolType;
    }

    errorReporter.reportError("Unknown unary operator: " + op);
    return ErrorType;
}

void SemanticAnalyzer::visitProgram(Program *node)
{
    for (auto &decl : node->declarations)
    {
        decl->accept(this);
    }
}

void SemanticAnalyzer::visitFunction(Function *node)
{
    if (symbolTable.isDefinedInCurrentScope(node->name))
    {
        errorReporter.reportError("Function '" + node->name + "' already defined");
        return;
    }

    vector<shared_ptr<Type>> paramTypes;
    for (const auto &param : node->parameters)
    {
        paramTypes.push_back(param.type);
    }

    auto funcType = make_shared<FunctionType>(node->returnType, paramTypes);
    symbolTable.define(node->name, funcType, true);

    symbolTable.enterScope();

    currentFunctionReturnType = node->returnType;

    for (const auto &param : node->parameters)
    {
        symbolTable.define(param.name, param.type);
    }

    node->body->accept(this);

    currentFunctionReturnType = nullptr;

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitVarDeclaration(VarDeclaration *node)
{
    if (symbolTable.isDefinedInCurrentScope(node->name))
    {
        errorReporter.reportError("Variable '" + node->name + "' already defined in this scope");
        return;
    }

    if (node->initializer)
    {
        node->initializer->accept(this);

        if (!isAssignable(node->type, node->initializer->type))
        {
            errorReporter.reportError("Type mismatch in variable initialization");
        }
    }

    symbolTable.define(node->name, node->type);
}

void SemanticAnalyzer::visitAssignment(Assignment *node)
{
    node->target->accept(this);
    node->value->accept(this);

    if (!isAssignable(node->target->type, node->value->type))
    {
        errorReporter.reportError("Type mismatch in assignment");
    }
}

void SemanticAnalyzer::visitBlock(Block *node)
{
    symbolTable.enterScope();

    for (auto &stmt : node->statements)
    {
        stmt->accept(this);
    }

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitIfStatement(IfStatement *node)
{
    node->condition->accept(this);

    if (node->condition->type->kind != TypeKind::BOOL)
    {
        errorReporter.reportError("If condition must be boolean");
    }

    node->thenBranch->accept(this);

    if (node->elseBranch)
    {
        node->elseBranch->accept(this);
    }
}

void SemanticAnalyzer::visitWhileStatement(WhileStatement *node)
{
    node->condition->accept(this);

    if (node->condition->type->kind != TypeKind::BOOL)
    {
        errorReporter.reportError("While condition must be boolean");
    }

    node->body->accept(this);
}

void SemanticAnalyzer::visitForStatement(ForStatement *node)
{
    symbolTable.enterScope();

    if (node->init)
    {
        node->init->accept(this);
    }

    if (node->condition)
    {
        node->condition->accept(this);

        if (node->condition->type->kind != TypeKind::BOOL)
        {
            errorReporter.reportError("For condition must be boolean");
        }
    }

    if (node->update)
    {
        node->update->accept(this);
    }

    node->body->accept(this);

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitReturnStatement(ReturnStatement *node)
{
    if (!currentFunctionReturnType)
    {
        errorReporter.reportError("Return statement outside function");
        return;
    }

    if (node->value)
    {
        node->value->accept(this);

        if (!isAssignable(currentFunctionReturnType, node->value->type))
        {
            errorReporter.reportError("Return type mismatch");
        }
    }
    else
    {
        if (currentFunctionReturnType->kind != TypeKind::VOID)
        {
            errorReporter.reportError("Non-void function must return a value");
        }
    }
}

void SemanticAnalyzer::visitPrintStatement(PrintStatement *node)
{
    node->expr->accept(this);
}

void SemanticAnalyzer::visitExpressionStatement(ExpressionStatement *node)
{
    node->expr->accept(this);
}

void SemanticAnalyzer::visitIntLiteral(IntLiteral *node)
{
    node->type = IntType;
}

void SemanticAnalyzer::visitFloatLiteral(FloatLiteral *node)
{
    node->type = FloatType;
}

void SemanticAnalyzer::visitStringLiteral(StringLiteral *node)
{
    node->type = StringType;
}

void SemanticAnalyzer::visitBoolLiteral(BoolLiteral *node)
{
    node->type = BoolType;
}

void SemanticAnalyzer::visitVariable(Variable *node)
{
    auto symbol = symbolTable.resolve(node->name);
    if (!symbol)
    {
        errorReporter.reportError("Undefined variable: " + node->name);
        node->type = ErrorType;
        return;
    }

    node->type = symbol->type;
}

void SemanticAnalyzer::visitArrayAccess(ArrayAccess *node)
{
    node->array->accept(this);
    node->index->accept(this);

    if (node->array->type->kind != TypeKind::ARRAY)
    {
        errorReporter.reportError("Array access on non-array type");
        node->type = ErrorType;
        return;
    }

    if (node->index->type->kind != TypeKind::INT)
    {
        errorReporter.reportError("Array index must be integer");
    }

    auto arrayType = static_pointer_cast<ArrayType>(node->array->type);
    node->type = arrayType->elementType;
}

void SemanticAnalyzer::visitBinaryOp(BinaryOp *node)
{
    node->left->accept(this);
    node->right->accept(this);

    node->type = checkBinaryOp(node->op, node->left->type, node->right->type);
}

void SemanticAnalyzer::visitUnaryOp(UnaryOp *node)
{
    node->expr->accept(this);

    node->type = checkUnaryOp(node->op, node->expr->type);
}

void SemanticAnalyzer::visitFunctionCall(FunctionCall *node)
{
    auto symbol = symbolTable.resolve(node->name);
    if (!symbol)
    {
        errorReporter.reportError("Undefined function: " + node->name);
        node->type = ErrorType;
        return;
    }

    if (!symbol->isFunction)
    {
        errorReporter.reportError(node->name + " is not a function");
        node->type = ErrorType;
        return;
    }

    auto funcType = static_pointer_cast<FunctionType>(symbol->type);

    if (node->args.size() != funcType->paramTypes.size())
    {
        errorReporter.reportError("Function argument count mismatch");
        node->type = ErrorType;
        return;
    }

    for (size_t i = 0; i < node->args.size(); ++i)
    {
        node->args[i]->accept(this);

        if (!isAssignable(funcType->paramTypes[i], node->args[i]->type))
        {
            errorReporter.reportError("Argument type mismatch");
        }
    }

    node->type = funcType->returnType;
}