#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "../Lexer/Tokens.h"
#include "../Parser/ASTNodes.h"

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
        struct FuncData
        {
            FuncData() = default;

            FuncData(const std::vector<ASTFunctionNode::Param>& params)
                : params(params)
            {
                
            }

        public:
            std::vector<ASTFunctionNode::Param> params;
        };

    public:
        Entry()
            : type(Tokens::VarType::Type::UNKNOWN), funcData(nullptr)
        {}

        Entry(const Tokens::VarType::Type& type, const std::vector<ASTFunctionNode::Param>& params)
            : type(type)
        {
            funcData = CreateRef<FuncData>(params);
        }

        Entry(const Tokens::VarType::Type& type)
            : type(type)
        {
        }

        inline const bool IsFunction() const { return funcData.get(); }

    public:
        Tokens::VarType::Type type;
        Ref<FuncData> funcData = nullptr;
    };
public:
    SymbolTable() {}

    void PushScope(bool isolate = false);
    void PopScope();

    void AddVariable(const std::string& name, Tokens::VarType::Type type);
    void AddFunction(const std::string& name, Tokens::VarType::Type returnType, const std::vector<ASTFunctionNode::Param>& params);

    void Isolate();
    void IsolateNext();

    bool contains(const std::string& name) const;

    bool InRootScope() const { return scopes.size() == 1; }

    const Entry& operator[](const std::string& name) const;
private:
    std::vector<std::unordered_map<std::string, Entry>> scopes;
    int isolatedLevel = -1;
};