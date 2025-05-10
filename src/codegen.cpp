#include "codegen.h"

using namespace std;

CodeGenerator::CodeGenerator(ostream &output)
    : output(output), indent(0) {}

void CodeGenerator::writeIndent()
{
    for (int i = 0; i < indent; ++i)
    {
        output << "    ";
    }
}

void CodeGenerator::write(const string &text)
{
    output << text;
}

void CodeGenerator::writeLine(const string &text)
{
    writeIndent();
    output << text << endl;
}

string CodeGenerator::getCType(shared_ptr<Type> type)
{
    switch (type->kind)
    {
    case TypeKind::INT:
        return "int";
    case TypeKind::FLOAT:
        return "float";
    case TypeKind::STRING:
        return "char*";
    case TypeKind::BOOL:
        return "int";
    case TypeKind::VOID:
        return "void";
    case TypeKind::ARRAY:
    {
        auto arrayType = static_pointer_cast<ArrayType>(type);
        return getCType(arrayType->elementType) + "*";
    }
    default:
        return "void*";
    }
}

void CodeGenerator::generate(shared_ptr<Program> program)
{
    writeLine("#include <stdio.h>");
    writeLine("#include <stdlib.h>");
    writeLine("#include <string.h>");
    writeLine("");

    program->accept(this);
}

void CodeGenerator::visitProgram(Program *node)
{
    for (auto &decl : node->declarations)
    {
        decl->accept(this);
        writeLine("");
    }
}

void CodeGenerator::visitFunction(Function *node)
{

    if (node->name == "main")
    {
        write("int main(");
    }
    else
    {
        write(getCType(node->returnType) + " " + node->name + "(");
    }

    for (size_t i = 0; i < node->parameters.size(); ++i)
    {
        if (i > 0)
            write(", ");
        write(getCType(node->parameters[i].type) + " " + node->parameters[i].name);
    }

    writeLine(") {");
    indent++;

    node->body->accept(this);

    if (node->name == "main" && node->returnType->kind == TypeKind::VOID)
    {
        writeLine("return 0;");
    }

    indent--;
    writeLine("}");
}

void CodeGenerator::visitVarDeclaration(VarDeclaration *node)
{
    writeIndent();

    if (node->type->kind == TypeKind::ARRAY)
    {
        auto arrayType = static_pointer_cast<ArrayType>(node->type);
        write(getCType(arrayType->elementType) + " " + node->name);
        write("[" + to_string(arrayType->size) + "]");
    }
    else
    {
        write(getCType(node->type) + " " + node->name);
    }

    if (node->initializer)
    {
        write(" = ");
        node->initializer->accept(this);
    }
    else if (node->type->kind == TypeKind::STRING)
    {
        write(" = NULL");
    }

    writeLine(";");
}

void CodeGenerator::visitAssignment(Assignment *node)
{
    node->target->accept(this);
    write(" = ");
    node->value->accept(this);
}

void CodeGenerator::visitBlock(Block *node)
{
    for (auto &stmt : node->statements)
    {
        stmt->accept(this);
    }
}

void CodeGenerator::visitIfStatement(IfStatement *node)
{
    writeIndent();
    write("if (");
    node->condition->accept(this);
    writeLine(") {");

    indent++;
    node->thenBranch->accept(this);
    indent--;

    if (node->elseBranch)
    {
        writeLine("} else {");
        indent++;
        node->elseBranch->accept(this);
        indent--;
    }

    writeLine("}");
}

void CodeGenerator::visitWhileStatement(WhileStatement *node)
{
    writeIndent();
    write("while (");
    node->condition->accept(this);
    writeLine(") {");

    indent++;
    node->body->accept(this);
    indent--;

    writeLine("}");
}

void CodeGenerator::visitForStatement(ForStatement *node)
{
    writeIndent();
    write("for (");

    if (node->init)
    {
        if (auto varDecl = dynamic_cast<VarDeclaration *>(node->init.get()))
        {
            write(getCType(varDecl->type) + " " + varDecl->name);
            if (varDecl->initializer)
            {
                write(" = ");
                varDecl->initializer->accept(this);
            }
        }
        else if (auto exprStmt = dynamic_cast<ExpressionStatement *>(node->init.get()))
        {
            exprStmt->expr->accept(this);
        }
    }
    write("; ");

    if (node->condition)
    {
        node->condition->accept(this);
    }
    write("; ");

    if (node->update)
    {
        if (auto exprStmt = dynamic_cast<ExpressionStatement *>(node->update.get()))
        {
            exprStmt->expr->accept(this);
        }
    }

    writeLine(") {");

    indent++;
    node->body->accept(this);
    indent--;

    writeLine("}");
}

void CodeGenerator::visitReturnStatement(ReturnStatement *node)
{
    writeIndent();
    write("return");

    if (node->value)
    {
        write(" ");
        node->value->accept(this);
    }

    writeLine(";");
}

void CodeGenerator::visitPrintStatement(PrintStatement *node)
{
    writeIndent();

    if (node->expr->type->kind == TypeKind::INT)
    {
        write("printf(\"%d\\n\", ");
    }
    else if (node->expr->type->kind == TypeKind::FLOAT)
    {
        write("printf(\"%f\\n\", ");
    }
    else if (node->expr->type->kind == TypeKind::STRING)
    {
        write("printf(\"%s\\n\", ");
    }
    else if (node->expr->type->kind == TypeKind::BOOL)
    {
        write("printf(\"%s\\n\", ");
        write("(");
        node->expr->accept(this);
        write(") ? \"true\" : \"false\"");
        writeLine(");");
        return;
    }

    node->expr->accept(this);
    writeLine(");");
}

void CodeGenerator::visitExpressionStatement(ExpressionStatement *node)
{
    writeIndent();
    node->expr->accept(this);
    writeLine(";");
}

void CodeGenerator::visitIntLiteral(IntLiteral *node)
{
    write(to_string(node->value));
}

void CodeGenerator::visitFloatLiteral(FloatLiteral *node)
{
    write(to_string(node->value));
}

void CodeGenerator::visitStringLiteral(StringLiteral *node)
{
    write("\"" + node->value + "\"");
}

void CodeGenerator::visitBoolLiteral(BoolLiteral *node)
{
    write(node->value ? "1" : "0");
}

void CodeGenerator::visitVariable(Variable *node)
{
    write(node->name);
}

void CodeGenerator::visitArrayAccess(ArrayAccess *node)
{
    node->array->accept(this);
    write("[");
    node->index->accept(this);
    write("]");
}

void CodeGenerator::visitBinaryOp(BinaryOp *node)
{
    write("(");
    node->left->accept(this);
    write(" " + node->op + " ");
    node->right->accept(this);
    write(")");
}

void CodeGenerator::visitUnaryOp(UnaryOp *node)
{
    write("(");
    write(node->op);
    node->expr->accept(this);
    write(")");
}

void CodeGenerator::visitFunctionCall(FunctionCall *node)
{
    write(node->name + "(");

    for (size_t i = 0; i < node->args.size(); ++i)
    {
        if (i > 0)
            write(", ");
        node->args[i]->accept(this);
    }

    write(")");
}