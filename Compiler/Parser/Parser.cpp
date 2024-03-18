#include "Parser.h"

Parser::Parser()
{
}

std::unique_ptr<ASTProgramNode> Parser::Parse(const std::string& program)
{
    parseIndex = 0;
    return std::make_unique<ASTProgramNode>(ParseBlock());
}

std::unique_ptr<ASTBlockNode> Parser::ParseBlock()
{
    return std::move(std::make_unique<ASTBlockNode>());
}
