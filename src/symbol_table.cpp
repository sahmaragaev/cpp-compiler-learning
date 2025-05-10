#include "symbol_table.h"

using namespace std;

void Scope::define(const string &name, shared_ptr<Type> type, bool isFunction)
{
    symbols[name] = make_shared<Symbol>(name, type, isFunction);
}

shared_ptr<Symbol> Scope::resolve(const string &name)
{
    auto it = symbols.find(name);
    if (it != symbols.end())
    {
        return it->second;
    }
    if (parent)
    {
        return parent->resolve(name);
    }
    return nullptr;
}

bool Scope::isDefined(const string &name)
{
    return resolve(name) != nullptr;
}

SymbolTable::SymbolTable()
{
    currentScope = make_shared<Scope>();
}

void SymbolTable::enterScope()
{
    currentScope = make_shared<Scope>(currentScope);
}

void SymbolTable::exitScope()
{
    if (currentScope->parent)
    {
        currentScope = currentScope->parent;
    }
}

void SymbolTable::define(const string &name, shared_ptr<Type> type, bool isFunction)
{
    currentScope->define(name, type, isFunction);
}

shared_ptr<Symbol> SymbolTable::resolve(const string &name)
{
    return currentScope->resolve(name);
}

bool SymbolTable::isDefined(const string &name)
{
    return currentScope->isDefined(name);
}

bool SymbolTable::isDefinedInCurrentScope(const string &name)
{
    auto it = currentScope->symbols.find(name);
    return it != currentScope->symbols.end();
}