#include "SymbolTable.h"

void SymbolTable::PushScope(bool isolate)
{
    scopes.emplace_back();
    if (isolate)
        Isolate();
}

void SymbolTable::PopScope()
{
    scopes.pop_back();
    // Undo isolation if isolated scope is popped
    if (scopes.size() < isolatedLevel)
        isolatedLevel = -1;
}

void SymbolTable::AddVariable(const std::string& name, Tokens::VarType::Type type)
{
    scopes[scopes.size() - 1][name] = { type };
}

void SymbolTable::AddFunction(const std::string& name, Tokens::VarType::Type returnType, const std::vector<ASTFunctionNode::Param>& params)
{
    scopes[scopes.size() - 1][name] = { returnType, params };
}

void SymbolTable::Isolate()
{
    isolatedLevel = scopes.size();
}

void SymbolTable::IsolateNext()
{
    isolatedLevel = scopes.size() + 1;
}

bool SymbolTable::contains(const std::string& name) const
{
    int level = scopes.size();
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, --level)
    {
        if (it->contains(name))
        {
            if (level < isolatedLevel)
            {
                auto& entry = it->at(name);
                return entry.IsFunction();
            }
            return true;
        }
    }

    return false;
}

const SymbolTable::Entry& SymbolTable::operator[](const std::string& name) const
{
    int level = scopes.size();

    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, --level)
    {
        if (it->contains(name))
        {
            if (level < isolatedLevel)
            {
                auto& entry = it->at(name);
                if (entry.IsFunction())
                    return entry;
                else
                    throw IdentifierNotFoundException(name);
            }
            return it->at(name);
        }
    }

    throw IdentifierNotFoundException(name);
}
