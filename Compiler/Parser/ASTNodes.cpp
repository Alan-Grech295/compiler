#include "ASTNodes.h"

ASTProgramNode::ASTProgramNode(std::unique_ptr<ASTBlockNode> blockNode)
    : blockNode(std::move(blockNode))
{
}

ASTBlockNode::ASTBlockNode()
    : statements()
{
}

ASTIdentifier::ASTIdentifier(const std::string& name, Tokens::Type::Name type)
    : name(name), type(type)
{
}

ASTVarDecl::ASTVarDecl(std::unique_ptr<ASTIdentifier> identifier, std::unique_ptr<ASTValueNode> value)
    : identifier(std::move(identifier)), value(std::move(value))
{
}

ASTIntLiteral::ASTIntLiteral(int value)
    : value(value)
{
}

ASTFloatLiteral::ASTFloatLiteral(float value)
    : value(value)
{
}

ASTBooleanLiteral::ASTBooleanLiteral(bool value)
    : value(value)
{
}

ASTColourLiteral::ASTColourLiteral(int value)
    : value(value)
{
}
