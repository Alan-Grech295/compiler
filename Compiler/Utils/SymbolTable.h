#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "../Lexer/Tokens.h"
#include "../Parser/ASTNodes.h"

template<typename T>
class SymbolTable
{
public:
    class IdentifierNotFoundException : public std::exception
    {
    public:
        IdentifierNotFoundException(const std::string& name)
        {
            std::ostringstream oss;
            oss << "The identifier \"" << name << "\" was not found";
            msg = oss.str();
        }
    public:
        const char* what()
        {
            return msg.c_str();
        }
    public:
        std::string msg;
    };
public:
    SymbolTable() {}

    void PushScope(bool isolate = false)
    {
        scopes.emplace_back();
        if (isolate)
            Isolate();
    }

    void PopScope()
    {
        scopes.pop_back();
        // Undo isolation if isolated scope is popped
        if (scopes.size() < isolatedLevel)
            isolatedLevel = -1;
    }

    void AddEntry(const std::string& name, T entry)
    {
        scopes[scopes.size() - 1][name] = entry;
    }

    void Isolate()
    {
        isolatedLevel = scopes.size();
    }

    void IsolateNext()
    {
        isolatedLevel = scopes.size() + 1;
    }

    bool contains(const std::string& name) const
    {
        int level = scopes.size();
        // Checks all scopes for the definition
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, --level)
        {
            if (it->contains(name))
            {
                if (level < isolatedLevel)
                {
                    auto entry = it->at(name);
                    // Allows functions to be referenced outside of the current scope
                    return entry.IsFunction();
                }
                return true;
            }
        }

        return false;
    }

    bool InRootScope() const { return scopes.size() == 1; }

    const int size() const { return scopes.size(); }

    const T& operator[](const std::string& name) const
    {
        int level = scopes.size();

        // Checks all scopes for the item
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
public:
    int isolatedLevel = -1;
private:
    std::vector<std::unordered_map<std::string, T>> scopes;
};