#pragma once
#include "../Lexer/Tokens.h"

#include <vector>
#include <memory>
#include <string>

class ASTNode
{
public:
    virtual ~ASTNode() = default;
};

class ASTBlockNode : public ASTNode
{
public:
    ASTBlockNode();

    inline void AddStatement(std::unique_ptr<ASTNode> statement)
    {
        statements.push_back(std::move(statement));
    }
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

class ASTProgramNode : public ASTNode
{
public:
    ASTProgramNode(std::unique_ptr<ASTBlockNode> blockNode);
public:
    std::unique_ptr<ASTBlockNode> blockNode;
};

class ASTExpressionNode : public ASTNode
{

};

class ASTIdentifierNode : public ASTExpressionNode
{
public:
    ASTIdentifierNode(const std::string& name, Tokens::VarType::Type type = Tokens::VarType::Type::UNKNOWN);
public:
    std::string name;
    Tokens::VarType::Type type = Tokens::VarType::Type::UNKNOWN;
};

class ASTIntLiteralNode : public ASTExpressionNode
{
public:
    ASTIntLiteralNode(int value);
public:
    int value;
};

class ASTFloatLiteralNode : public ASTExpressionNode
{
public:
    ASTFloatLiteralNode(float value);
public:
    float value;
};

class ASTBooleanLiteralNode : public ASTExpressionNode
{
public:
    ASTBooleanLiteralNode(bool value);
public:
    bool value;
};

class ASTColourLiteralNode : public ASTExpressionNode
{
public:
    ASTColourLiteralNode(int value);
public:
    int value;
};

class ASTVarDeclNode : public ASTNode
{
public:
    ASTVarDeclNode(std::unique_ptr<ASTIdentifierNode> identifier, std::unique_ptr<ASTExpressionNode> value);
public:
    std::unique_ptr<ASTIdentifierNode> identifier;
    std::unique_ptr<ASTExpressionNode> value;
};

class ASTBinaryOpNode : public ASTExpressionNode
{
    enum class Type
    {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,

        AND,
        OR,

        EQUAL,
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
public:
    Type type = Type::ADD;
    std::unique_ptr<ASTExpressionNode> left;
    std::unique_ptr<ASTExpressionNode> right;
};

class ASTNegateNode : public ASTExpressionNode
{
public:
    ASTNegateNode(std::unique_ptr<ASTExpressionNode> expr);
public:
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTNotNode : public ASTExpressionNode
{
public:
    ASTNotNode(std::unique_ptr<ASTExpressionNode> expr);
public:
    std::unique_ptr<ASTExpressionNode> expr;
};

class ASTAssignmentNode : public ASTNode
{
public:
    ASTAssignmentNode(Scope<ASTIdentifierNode> identifier, Scope<ASTExpressionNode> expr);
public:
    Scope<ASTIdentifierNode> identifier;
    Scope<ASTExpressionNode> expr;
};

class ASTDecisionNode : public ASTNode
{
public:
    ASTDecisionNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> trueStatement, Scope<ASTBlockNode> falseStatement = nullptr);

public:
    Scope<ASTExpressionNode> expr;
    Scope<ASTBlockNode> trueStatement;
    Scope<ASTBlockNode> falseStatement;
};

class ASTReturnNode : public ASTNode
{
public:
    ASTReturnNode(Scope<ASTExpressionNode> expr);

public:
    Scope<ASTExpressionNode> expr;
};

class ASTFunctionNode : public ASTNode
{
public:
    struct Param
    {
    public:
        Param(const std::string& name, Tokens::VarType::Type type)
            : Name(name), Type(type)
        {}

        Param() = default;
    public:
        std::string Name;
        Tokens::VarType::Type Type;
    };
public:
    ASTFunctionNode(const std::string& name, const std::vector<Param>& params, Tokens::VarType::Type returnType, Scope<ASTBlockNode> blockNode);
public:
    std::string name;
    std::vector<Param> params;
    Tokens::VarType::Type returnType;
    Scope<ASTBlockNode> blockNode;
};

class ASTWhileNode : public ASTNode
{
public:
    ASTWhileNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> blockNode);
public:
    Scope<ASTExpressionNode> expr;
    Scope<ASTBlockNode> blockNode;
};

class ASTForNode : public ASTNode
{
public:
    ASTForNode(Scope<ASTVarDeclNode> variableDecl, Scope<ASTExpressionNode> expr, Scope<ASTAssignmentNode> assignment, Scope<ASTBlockNode> blockNode);
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
public:
    Scope<ASTExpressionNode> expr;
};

class ASTDelayNode : public ASTNode
{
public:
    ASTDelayNode(Scope<ASTExpressionNode> delayExpr);
public:
    Scope<ASTExpressionNode> delayExpr;
};

class ASTWriteNode : public ASTNode
{
public:
    ASTWriteNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> colour);
public:
    Scope<ASTExpressionNode> x;
    Scope<ASTExpressionNode> y;
    Scope<ASTExpressionNode> colour;
};

class ASTWriteBoxNode : public ASTNode
{
public:
    ASTWriteBoxNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> w, Scope<ASTExpressionNode> h, Scope<ASTExpressionNode> colour);
public:
    Scope<ASTExpressionNode> x;
    Scope<ASTExpressionNode> y;
    Scope<ASTExpressionNode> w;
    Scope<ASTExpressionNode> h;
    Scope<ASTExpressionNode> colour;
};