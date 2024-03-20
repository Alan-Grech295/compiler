#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "../Lexer/Tokens.h"

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

        const char* what()
        {
            return msg.c_str();
        }
    private:
        std::string msg;
    };
    struct Entry
    {
        Tokens::VarType::Type type;
    };
public:
    SymbolTable() {}

    void PushScope();
    void PopScope();

    void AddVariable(const std::string& name, Tokens::VarType::Type type);

    const Entry& operator[](const std::string& name) const;
private:
    std::vector<std::unordered_map<std::string, Entry>> scopes;
};