#pragma once
class ASTBlockNode;
class ASTProgramNode;
class ASTIntLiteralNode;
class ASTFloatLiteralNode;
class ASTBooleanLiteralNode;
class ASTColourLiteralNode;
class ASTIdentifierNode;
class ASTArraySetNode;
class ASTVarDeclNode;
class ASTBinaryOpNode;
class ASTNegateNode;
class ASTNotNode;
class ASTCastNode;
class ASTAssignmentNode;
class ASTDecisionNode;
class ASTReturnNode;
class ASTFunctionNode;
class ASTWhileNode;
class ASTForNode;
class ASTPrintNode;
class ASTDelayNode;
class ASTWriteNode;
class ASTWriteBoxNode;
class ASTWidthNode;
class ASTHeightNode;
class ASTReadNode;
class ASTRandIntNode;
class ASTFuncCallNode;
class ASTArrayIndexNode;

class Visitor
{
public:
    virtual void visit(ASTBlockNode& node)          = 0;
    virtual void visit(ASTProgramNode& node)        = 0;
    virtual void visit(ASTIntLiteralNode& node)     = 0;
    virtual void visit(ASTFloatLiteralNode& node)   = 0;
    virtual void visit(ASTBooleanLiteralNode& node) = 0;
    virtual void visit(ASTColourLiteralNode& node)  = 0;
    virtual void visit(ASTIdentifierNode& node)     = 0;
    virtual void visit(ASTArrayIndexNode& node)     = 0;
    virtual void visit(ASTArraySetNode& node)       = 0;
    virtual void visit(ASTVarDeclNode& node)        = 0;
    virtual void visit(ASTBinaryOpNode& node)       = 0;
    virtual void visit(ASTNegateNode& node)         = 0;
    virtual void visit(ASTNotNode& node)            = 0;
    virtual void visit(ASTCastNode& node)           = 0;
    virtual void visit(ASTAssignmentNode& node)     = 0;
    virtual void visit(ASTDecisionNode& node)       = 0;
    virtual void visit(ASTReturnNode& node)         = 0;
    virtual void visit(ASTFunctionNode& node)       = 0;
    virtual void visit(ASTWhileNode& node)          = 0;
    virtual void visit(ASTForNode& node)            = 0;
    virtual void visit(ASTPrintNode& node)          = 0;
    virtual void visit(ASTDelayNode& node)          = 0;
    virtual void visit(ASTWriteNode& node)          = 0;
    virtual void visit(ASTWriteBoxNode& node)       = 0;
    virtual void visit(ASTWidthNode& node)          = 0;
    virtual void visit(ASTHeightNode& node)         = 0;
    virtual void visit(ASTReadNode& node)           = 0;
    virtual void visit(ASTRandIntNode& node)        = 0;
    virtual void visit(ASTFuncCallNode& node)       = 0;
};