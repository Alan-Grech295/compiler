#pragma once
#include <string>
#include <algorithm>

#include "../Lexer/Lexer.h"
#include "ASTNodes.h"
#include "Utils/Utils.h"

class Parser
{
public:
    class SyntaxErrorException : public std::exception
    {
    public:
        SyntaxErrorException(const std::string& program, int character)
        {
            std::string programUpToChar = program.substr(0, character - 1);
            int lineCount = std::ranges::count(programUpToChar, '\n') + 1;
            int lastLine = programUpToChar.rfind('\n');
            int charCount = character - lastLine - 1;

            std::ostringstream oss;
            oss << "Syntax error at line " << lineCount << " character " << charCount;
            message = oss.str();
        }
    public:
        const char* what()
        {
            return message.c_str();
        }
    public:
        std::string message;
    };
public:
    Parser();
    
    Scope<ASTProgramNode> Parse(const std::string& program);
private:
    Scope<ASTBlockNode> ParseBlock(bool root = false);
    Scope<ASTNode> ParseStatement();
    Scope<ASTVarDeclNode> ParseVariableDeclaration();

    Scope<ASTAssignmentNode> ParseAssignment();

    Scope<ASTExpressionNode> ParseExpression(bool subExpr = false);
    Scope<ASTExpressionNode> ParseSimpleExpression();
    Scope<ASTExpressionNode> ParseTerm();
    Scope<ASTExpressionNode> ParseFactor();

    Scope<ASTReturnNode> ParseReturnStatement();

    Scope<ASTFunctionNode> ParseFunctionDecl();

    Scope<ASTWhileNode> ParseWhileLoop();
    Scope<ASTForNode> ParseForLoop();

    Scope<ASTPrintNode> ParsePrint();
    Scope<ASTDelayNode> ParseDelay();
    Scope<ASTWriteNode> ParseWrite();
    Scope<ASTWriteBoxNode> ParseWriteBox();

    ASTFunctionNode::Param ParseParam();

    Scope<ASTDecisionNode> ParseIfStatement();

    inline Scope<Token> GetNextToken() { pastProgramIndex = programIndex; return lexer.GetNextToken(program, programIndex, true, true); }
    inline Scope<Token> PeekNextToken() { return lexer.PeekNextToken(program, programIndex, true, true); }
    inline void JumpToken(int tokenLength) { pastProgramIndex = programIndex; programIndex += tokenLength; }
private:
    Lexer lexer{};
    std::string program;
    int programIndex = 0;
    // Used for debugging
    int pastProgramIndex = 0;
};

#define CHECK_SUB_TYPE(var, cls, check) (var->type == ::Tokens::cls::TokenType && var->As<::Tokens::cls>().check)
#define ASSERT(condition) if(!(condition)) { throw SyntaxErrorException(program, pastProgramIndex); }