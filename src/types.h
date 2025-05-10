#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <memory>
#include <vector>

using namespace std;

enum class TypeKind
{
    INT,
    FLOAT,
    STRING,
    BOOL,
    VOID,
    ARRAY,
    FUNCTION,
    ERROR
};

class Type
{
public:
    TypeKind kind;

    Type(TypeKind kind) : kind(kind) {}
    virtual ~Type() = default;

    virtual string toString() const = 0;
    virtual bool equals(const Type *other) const = 0;
};

class PrimitiveType : public Type
{
public:
    PrimitiveType(TypeKind kind) : Type(kind) {}

    string toString() const override
    {
        switch (kind)
        {
        case TypeKind::INT:
            return "int";
        case TypeKind::FLOAT:
            return "float";
        case TypeKind::STRING:
            return "string";
        case TypeKind::BOOL:
            return "bool";
        case TypeKind::VOID:
            return "void";
        default:
            return "error";
        }
    }

    bool equals(const Type *other) const override
    {
        return other && other->kind == kind;
    }
};

class ArrayType : public Type
{
public:
    shared_ptr<Type> elementType;
    int size;

    ArrayType(shared_ptr<Type> elementType, int size)
        : Type(TypeKind::ARRAY), elementType(elementType), size(size) {}

    string toString() const override
    {
        return elementType->toString() + "[" + to_string(size) + "]";
    }

    bool equals(const Type *other) const override
    {
        if (!other || other->kind != TypeKind::ARRAY)
            return false;
        const ArrayType *arrType = static_cast<const ArrayType *>(other);
        return elementType->equals(arrType->elementType.get()) && size == arrType->size;
    }
};

class FunctionType : public Type
{
public:
    shared_ptr<Type> returnType;
    vector<shared_ptr<Type>> paramTypes;

    FunctionType(shared_ptr<Type> returnType,
                 const vector<shared_ptr<Type>> &paramTypes)
        : Type(TypeKind::FUNCTION), returnType(returnType), paramTypes(paramTypes) {}

    string toString() const override
    {
        string result = returnType->toString() + "(";
        for (size_t i = 0; i < paramTypes.size(); ++i)
        {
            if (i > 0)
                result += ", ";
            result += paramTypes[i]->toString();
        }
        return result + ")";
    }

    bool equals(const Type *other) const override
    {
        if (!other || other->kind != TypeKind::FUNCTION)
            return false;
        const FunctionType *funcType = static_cast<const FunctionType *>(other);
        if (!returnType->equals(funcType->returnType.get()))
            return false;
        if (paramTypes.size() != funcType->paramTypes.size())
            return false;
        for (size_t i = 0; i < paramTypes.size(); ++i)
        {
            if (!paramTypes[i]->equals(funcType->paramTypes[i].get()))
                return false;
        }
        return true;
    }
};

extern shared_ptr<Type> IntType;
extern shared_ptr<Type> FloatType;
extern shared_ptr<Type> StringType;
extern shared_ptr<Type> BoolType;
extern shared_ptr<Type> VoidType;
extern shared_ptr<Type> ErrorType;

#endif 