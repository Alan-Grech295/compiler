#pragma once
#include <vector>
#include <tuple>

#include "../Utils/Visitor.h"
#include "../Utils/SymbolTable.h"
#include "../Lexer/Tokens.h"
#include "../Parser/ASTNodes.h"

using namespace Tokens;
using Type = std::pair<Tokens::VarType::Type, int>;

class SemanticErrorException : public std::exception
{
public:
    SemanticErrorException(const std::string& line)
    {
        std::ostringstream oss;
        oss << "Semantic error (line: " << line << ")";
        msg = oss.str();
    }

    const char* what()
    {
        return msg.c_str();
    }

    std::string msg;
};

class SemanticAnalyzerVisitor : public Visitor
{
public:
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

        Entry(const Tokens::VarType::Type& type, int arraySize, const std::vector<ASTFunctionNode::Param>& params)
            : type(type), arraySize(arraySize)
        {
            funcData = CreateRef<FuncData>(params);
        }

        Entry(const Tokens::VarType::Type& type)
            : type(type)
        {
        }

        Entry(const Tokens::VarType::Type& type, int arraySize)
            : type(type), arraySize(arraySize)
        {
        }

        inline const bool IsFunction() const { return funcData.get(); }

        inline const bool IsArray() const { return arraySize > 0; }

    public:
        Tokens::VarType::Type type;
        int arraySize = -1;
        Ref<FuncData> funcData = nullptr;
    };
public:
    void visit(ASTBlockNode& node) override;
    void visit(ASTProgramNode& node) override;
    void visit(ASTIntLiteralNode& node) override;
    void visit(ASTFloatLiteralNode& node) override;
    void visit(ASTBooleanLiteralNode& node) override;
    void visit(ASTColourLiteralNode& node) override;
    void visit(ASTIdentifierNode& node) override;
    void visit(ASTVarDeclNode& node) override;
    void visit(ASTBinaryOpNode& node) override;
    void visit(ASTNegateNode& node) override;
    void visit(ASTNotNode& node) override;
    void visit(ASTCastNode& node) override;
    void visit(ASTAssignmentNode& node) override;
    void visit(ASTDecisionNode& node) override;
    void visit(ASTReturnNode& node) override;
    void visit(ASTFunctionNode& node) override;
    void visit(ASTWhileNode& node) override;
    void visit(ASTForNode& node) override;
    void visit(ASTPrintNode& node) override;
    void visit(ASTDelayNode& node) override;
    void visit(ASTWriteNode& node) override;
    void visit(ASTWriteBoxNode& node) override;
    void visit(ASTWidthNode& node) override;
    void visit(ASTHeightNode& node) override;
    void visit(ASTReadNode& node) override;
    void visit(ASTRandIntNode& node) override;
    void visit(ASTFuncCallNode& node) override;

private:
    inline void PushType(VarType::Type type, int arraySize = -1) { typeStack.emplace_back(type, arraySize); }
    inline void PushType(Type type) { typeStack.emplace_back(type); }
    inline Type PopType()
    {
        auto type = typeStack.back();
        typeStack.pop_back();
        return type;
    }

    inline std::tuple<Type, Type> PopTypes()
    {
        auto type1 = typeStack.back();
        typeStack.pop_back();
        auto type2 = typeStack.back();
        typeStack.pop_back();
        return std::make_tuple<>(type2, type1);
    }

private:
    SymbolTable<Entry> symbolTable{};
    std::vector<Type> typeStack{};
    VarType::Type expectedRetType = VarType::Type::UNKNOWN;
    int expectedRetArrSize = -1;

    // Inherited via Visitor
    void visit(ASTArraySetNode& node) override;

    // Inherited via Visitor
    void visit(ASTArrayIndexNode& node) override;
};

#define ASSERT(check) if(!(check)) { throw SemanticErrorException(std::to_string(__LINE__)); }
#define IS_ARRAY(type) (type.second > 0)