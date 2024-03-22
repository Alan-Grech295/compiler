#pragma once
#include "../Utils/Visitor.h"
#include "../Parser/ASTNodes.h"

class CodeGenVisitor : public Visitor
{
    virtual void visit(ASTBlockNode& node) override;
    virtual void visit(ASTProgramNode& node) override;
    virtual void visit(ASTIntLiteralNode& node) override;
    virtual void visit(ASTFloatLiteralNode& node) override;
    virtual void visit(ASTBooleanLiteralNode& node) override;
    virtual void visit(ASTColourLiteralNode& node) override;
    virtual void visit(ASTIdentifierNode& node) override;
    virtual void visit(ASTVarDeclNode& node) override;
    virtual void visit(ASTBinaryOpNode& node) override;
    virtual void visit(ASTNegateNode& node) override;
    virtual void visit(ASTNotNode& node) override;
    virtual void visit(ASTCastNode& node) override;
    virtual void visit(ASTAssignmentNode& node) override;
    virtual void visit(ASTDecisionNode& node) override;
    virtual void visit(ASTReturnNode& node) override;
    virtual void visit(ASTFunctionNode& node) override;
    virtual void visit(ASTWhileNode& node) override;
    virtual void visit(ASTForNode& node) override;
    virtual void visit(ASTPrintNode& node) override;
    virtual void visit(ASTDelayNode& node) override;
    virtual void visit(ASTWriteNode& node) override;
    virtual void visit(ASTWriteBoxNode& node) override;
    virtual void visit(ASTWidthNode& node) override;
    virtual void visit(ASTHeightNode& node) override;
    virtual void visit(ASTReadNode& node) override;
    virtual void visit(ASTRandIntNode& node) override;
    virtual void visit(ASTFuncCallNode& node) override;

private:
    std::string code;
};