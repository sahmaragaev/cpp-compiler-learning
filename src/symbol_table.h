#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "types.h"

using namespace std;

struct Symbol
{
    string name;
    shared_ptr<Type> type;
    bool isFunction;

    Symbol(const string &name, shared_ptr<Type> type, bool isFunction = false)
        : name(name), type(type), isFunction(isFunction) {}
};

class Scope
{
public:
    unordered_map<string, shared_ptr<Symbol>> symbols;
    shared_ptr<Scope> parent;

    Scope(shared_ptr<Scope> parent = nullptr) : parent(parent) {}

    void define(const string &name, shared_ptr<Type> type, bool isFunction = false);
    shared_ptr<Symbol> resolve(const string &name);
    bool isDefined(const string &name);
};

class SymbolTable
{
private:
    shared_ptr<Scope> currentScope;

public:
    SymbolTable();

    void enterScope();
    void exitScope();

    void define(const string &name, shared_ptr<Type> type, bool isFunction = false);
    shared_ptr<Symbol> resolve(const string &name);
    bool isDefined(const string &name);
    bool isDefinedInCurrentScope(const string &name);
};

#endif 