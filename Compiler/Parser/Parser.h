#pragma once
#include <string>

#include "../Lexer/Lexer.h"
#include "ASTNodes.h"

class Parser
{
public:
    Parser();
    
    std::unique_ptr<ASTProgramNode> Parse(const std::string& program);
private:
    std::unique_ptr<ASTBlockNode> ParseBlock();
private:
    Lexer lexer{};
    int parseIndex = 0;
};