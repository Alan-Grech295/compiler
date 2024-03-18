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

class ASTStatementNode : public ASTNode
{
public:
    ASTStatementNode() = default;
    virtual ~ASTStatementNode() = default;

};

class ASTBlockNode : public ASTNode
{
public:
    ASTBlockNode();
public:
    std::vector<std::unique_ptr<ASTStatementNode>> statements;
};

class ASTProgramNode : public ASTNode
{
public:
    ASTProgramNode(std::unique_ptr<ASTBlockNode> blockNode);
public:
    std::unique_ptr<ASTBlockNode> blockNode;
};

class ASTValueNode : public ASTNode
{

};

class ASTIdentifier : public ASTValueNode
{
public:
    ASTIdentifier(const std::string& name, Tokens::Type::Name type);
public:
    std::string name;
    Tokens::Type::Name type;
};

class ASTIntLiteral : public ASTValueNode
{
public:
    ASTIntLiteral(int value);
public:
    int value;
};

class ASTFloatLiteral : public ASTValueNode
{
public:
    ASTFloatLiteral(float value);
public:
    float value;
};

class ASTBooleanLiteral : public ASTValueNode
{
public:
    ASTBooleanLiteral(bool value);
public:
    bool value;
};

class ASTColourLiteral : public ASTValueNode
{
public:
    ASTColourLiteral(int value);
public:
    int value;
};

class ASTVarDecl : public ASTStatementNode
{
public:
    ASTVarDecl(std::unique_ptr<ASTIdentifier> identifier, std::unique_ptr<ASTValueNode> value);
public:
    std::unique_ptr<ASTIdentifier> identifier;
    std::unique_ptr<ASTValueNode> value;
};