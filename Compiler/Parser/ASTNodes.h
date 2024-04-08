#pragma once
#include "../Lexer/Tokens.h"
#include "../Utils/Visitor.h"

#include <vector>
#include <memory>
#include <string>


class ASTNode
{
public:
    virtual ~ASTNode() = default;

    virtual void accept(Visitor& visitor) = 0;
};

class ASTBlockNode : public ASTNode
{
public:
    ASTBlockNode();

    inline void AddStatement(std::unique_ptr<ASTNode> statement)
    {
        statements.push_back(std::move(statement));
    }

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

class ASTProgramNode : public ASTNode
{
public:
    ASTProgramNode(std::unique_ptr<ASTBlockNode> blockNode);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::unique_ptr<ASTBlockNode> blockNode;
};

class ASTExpressionNode : public ASTNode
{
};

class ASTIdentifierNode : public ASTExpressionNode
{
public:
    ASTIdentifierNode(const std::string& name, Tokens::VarType::Type type = Tokens::VarType::Type::UNKNOWN, int arraySize = -1);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };

    inline bool IsArray() const { return arraySize > 0; }
public:
    std::string name;
    Tokens::VarType::Type type = Tokens::VarType::Type::UNKNOWN;
    int arraySize = -1;
};

class ASTArrayIndexNode : public ASTIdentifierNode
{
public:
    ASTArrayIndexNode(const std::string& name, Scope<ASTExpressionNode> index);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::string name;
    Scope<ASTExpressionNode> index;
};

class ASTArraySetNode : public ASTExpressionNode
{
public:
    ASTArraySetNode() = default;
    ASTArraySetNode(Scope<ASTExpressionNode> lit, int duplication);

    void AddLiterial(Scope<ASTExpressionNode> lit);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::vector<Scope<ASTExpressionNode>> literals;
    int duplication = -1;
};

class ASTIntLiteralNode : public ASTExpressionNode
{
public:
    ASTIntLiteralNode(int value);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    int value;
};

class ASTFloatLiteralNode : public ASTExpressionNode
{
public:
    ASTFloatLiteralNode(float value);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    float value;
};

class ASTBooleanLiteralNode : public ASTExpressionNode
{
public:
    ASTBooleanLiteralNode(bool value);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    bool value;
};

class ASTColourLiteralNode : public ASTExpressionNode
{
public:
    ASTColourLiteralNode(int value);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    int value;
};

class ASTVarDeclNode : public ASTNode
{
public:
    ASTVarDeclNode(std::unique_ptr<ASTIdentifierNode> identifier, std::unique_ptr<ASTExpressionNode> value);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::unique_ptr<ASTIdentifierNode> identifier;
    std::unique_ptr<ASTExpressionNode> value;
};

class ASTBinaryOpNode : public ASTExpressionNode
{
public:
    enum class Type
    {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MOD,

        AND,
        OR,

        EQUAL,
        NOT_EQUAL,
        GREATER,
        LESS_THAN,
        GREATER_EQUAL,
        LESS_THAN_EQUAL,
    };
public:
    ASTBinaryOpNode(Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right);
    ASTBinaryOpNode(Tokens::AdditiveOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right);
    ASTBinaryOpNode(Tokens::MultiplicativeOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right);
    ASTBinaryOpNode(Tokens::RelationalOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
public:
    Type type = Type::ADD;
    std::unique_ptr<ASTExpressionNode> left;
    std::unique_ptr<ASTExpressionNode> right;
};

class ASTNegateNode : public ASTExpressionNode
{
public:
    ASTNegateNode(std::unique_ptr<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTNotNode : public ASTExpressionNode
{
public:
    ASTNotNode(std::unique_ptr<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTCastNode : public ASTExpressionNode
{
public:
    ASTCastNode(Tokens::VarType::Type castType, std::unique_ptr<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Tokens::VarType::Type castType;
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTAssignmentNode : public ASTNode
{
public:
    ASTAssignmentNode(Scope<ASTIdentifierNode> identifier, Scope<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTIdentifierNode> identifier;
    Scope<ASTExpressionNode> expr;
};

class ASTDecisionNode : public ASTNode
{
public:
    ASTDecisionNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> trueStatement, Scope<ASTBlockNode> falseStatement = nullptr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> expr;
    Scope<ASTBlockNode> trueStatement;
    Scope<ASTBlockNode> falseStatement;
};

class ASTReturnNode : public ASTNode
{
public:
    ASTReturnNode(Scope<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> expr;
};

class ASTFunctionNode : public ASTNode
{
public:
    struct Param
    {
    public:
        Param(const std::string& name, Tokens::VarType::Type type, int arraySize = -1)
            : Name(name), Type(type), ArraySize(arraySize)
        {}

        Param() = default;

        bool IsArray() const { return ArraySize > 0; }
    public:
        std::string Name;
        Tokens::VarType::Type Type;
        int ArraySize = -1;
    };
public:
    ASTFunctionNode(const std::string& name, const std::vector<Param>& params, Tokens::VarType::Type returnType, int arraySize, Scope<ASTBlockNode> blockNode);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::string name;
    std::vector<Param> params;
    Tokens::VarType::Type returnType;
    int returnSize = -1;
    Scope<ASTBlockNode> blockNode;
};

class ASTWhileNode : public ASTNode
{
public:
    ASTWhileNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> blockNode);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> expr;
    Scope<ASTBlockNode> blockNode;
};

class ASTForNode : public ASTNode
{
public:
    ASTForNode(Scope<ASTVarDeclNode> variableDecl, Scope<ASTExpressionNode> expr, Scope<ASTAssignmentNode> assignment, Scope<ASTBlockNode> blockNode);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTVarDeclNode> variableDecl;
    Scope<ASTExpressionNode> expr;
    Scope<ASTAssignmentNode> assignment;
    Scope<ASTBlockNode> blockNode;
};

class ASTPrintNode : public ASTNode
{
public:
    ASTPrintNode(Scope<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> expr;
};

class ASTDelayNode : public ASTNode
{
public:
    ASTDelayNode(Scope<ASTExpressionNode> delayExpr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> delayExpr;
};

class ASTWriteNode : public ASTNode
{
public:
    ASTWriteNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> colour);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> x;
    Scope<ASTExpressionNode> y;
    Scope<ASTExpressionNode> colour;
};

class ASTWriteBoxNode : public ASTNode
{
public:
    ASTWriteBoxNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> w, Scope<ASTExpressionNode> h, Scope<ASTExpressionNode> colour);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> x;
    Scope<ASTExpressionNode> y;
    Scope<ASTExpressionNode> w;
    Scope<ASTExpressionNode> h;
    Scope<ASTExpressionNode> colour;
};

class ASTWidthNode : public ASTExpressionNode
{
public:
    ASTWidthNode() {}
    
    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
};

class ASTHeightNode : public ASTExpressionNode
{
public:
    ASTHeightNode() {}

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
};

class ASTReadNode : public ASTExpressionNode
{
public:
    ASTReadNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> x;
    Scope<ASTExpressionNode> y;
};

class ASTClearNode : public ASTExpressionNode
{
public:
    ASTClearNode(std::unique_ptr<ASTExpressionNode> expr);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTRandIntNode : public ASTExpressionNode
{
public:
    ASTRandIntNode(Scope<ASTExpressionNode> max);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    Scope<ASTExpressionNode> max;
};

class ASTFuncCallNode : public ASTExpressionNode
{
public:
    ASTFuncCallNode(const std::string& funcName);

    void AddArg(Scope< ASTExpressionNode> arg);

    inline virtual void accept(Visitor& visitor) override { visitor.visit(*this); };
public:
    std::string funcName;
    std::vector<Scope<ASTExpressionNode>> args;
};