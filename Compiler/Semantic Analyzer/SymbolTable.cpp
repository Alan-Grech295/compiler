#include "SymbolTable.h"

void SymbolTable::PushScope()
{
    scopes.emplace_back();
}

void SymbolTable::PopScope()
{
    scopes.pop_back();
}

void SymbolTable::AddVariable(const std::string& name, Tokens::VarType::Type type)
{
    scopes[scopes.size() - 1].emplace(name, type);
}

const SymbolTable::Entry& SymbolTable::operator[](const std::string& name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        if (it->contains(name))
            return it->at(name);
    }

    throw IdentifierNotFoundException(name);
}
