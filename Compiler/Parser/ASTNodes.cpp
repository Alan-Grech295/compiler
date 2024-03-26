#include "ASTNodes.h"

ASTProgramNode::ASTProgramNode(std::unique_ptr<ASTBlockNode> blockNode)
    : blockNode(std::move(blockNode))
{
}

ASTBlockNode::ASTBlockNode()
    : statements()
{
}

ASTIdentifierNode::ASTIdentifierNode(const std::string& name, Tokens::VarType::Type type, int arraySize)
    : name(name), type(type), arraySize(arraySize)
{
}

ASTVarDeclNode::ASTVarDeclNode(std::unique_ptr<ASTIdentifierNode> identifier, std::unique_ptr<ASTExpressionNode> value)
    : identifier(std::move(identifier)), value(std::move(value))
{
}

ASTIntLiteralNode::ASTIntLiteralNode(int value)
    : value(value)
{
}

ASTFloatLiteralNode::ASTFloatLiteralNode(float value)
    : value(value)
{
}

ASTBooleanLiteralNode::ASTBooleanLiteralNode(bool value)
    : value(value)
{
}

ASTColourLiteralNode::ASTColourLiteralNode(int value)
    : value(value)
{
}

ASTBinaryOpNode::ASTBinaryOpNode(Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right)
    : type(type), left(std::move(left)), right(std::move(right))
{
}

ASTBinaryOpNode::ASTBinaryOpNode(Tokens::AdditiveOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right)
    : type(Type::ADD), left(std::move(left)), right(std::move(right))
{
    switch (type)
    {
        case Tokens::AdditiveOp::Type::ADD:
            this->type = Type::ADD;
            break;
        case Tokens::AdditiveOp::Type::SUBTRACT:
            this->type = Type::SUBTRACT;
            break;
        case Tokens::AdditiveOp::Type::OR:
            this->type = Type::OR;
            break;
    }
}

ASTBinaryOpNode::ASTBinaryOpNode(Tokens::MultiplicativeOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right)
    : type(Type::ADD), left(std::move(left)), right(std::move(right))
{
    switch (type)
    {
        case Tokens::MultiplicativeOp::Type::MULTIPLY:
            this->type = Type::MULTIPLY;
            break;
        case Tokens::MultiplicativeOp::Type::DIVIDE:
            this->type = Type::DIVIDE;
            break;
        case Tokens::MultiplicativeOp::Type::AND:
            this->type = Type::AND;
            break;
    }
}

ASTBinaryOpNode::ASTBinaryOpNode(Tokens::RelationalOp::Type type, std::unique_ptr<ASTExpressionNode> left, std::unique_ptr<ASTExpressionNode> right)
    : type(Type::ADD), left(std::move(left)), right(std::move(right))
{
    switch (type)
    {
        case Tokens::RelationalOp::Type::EQUAL:
            this->type = Type::EQUAL;
            break;
        case Tokens::RelationalOp::Type::NOT_EQUAL:
            this->type = Type::NOT_EQUAL;
            break;
        case Tokens::RelationalOp::Type::GREATER:
            this->type = Type::GREATER;
            break;
        case Tokens::RelationalOp::Type::GREATER_EQUAL:
            this->type = Type::GREATER_EQUAL;
            break;
        case Tokens::RelationalOp::Type::LESS_THAN:
            this->type = Type::LESS_THAN;
            break;
        case Tokens::RelationalOp::Type::LESS_THAN_EQUAL:
            this->type = Type::LESS_THAN_EQUAL;
            break;
    }
}

ASTNegateNode::ASTNegateNode(std::unique_ptr<ASTExpressionNode> expr)
    : expr(std::move(expr))
{
}

ASTNotNode::ASTNotNode(std::unique_ptr<ASTExpressionNode> expr)
    : expr(std::move(expr))
{
}

ASTAssignmentNode::ASTAssignmentNode(Scope<ASTIdentifierNode> identifier, Scope<ASTExpressionNode> expr)
    : identifier(std::move(identifier)), expr(std::move(expr))
{
}

ASTDecisionNode::ASTDecisionNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> trueStatement, Scope<ASTBlockNode> falseStatement)
    : expr(std::move(expr)), trueStatement(std::move(trueStatement)), falseStatement(std::move(falseStatement))
{
}

ASTReturnNode::ASTReturnNode(Scope<ASTExpressionNode> expr)
    : expr(std::move(expr))
{
}

ASTFunctionNode::ASTFunctionNode(const std::string& name, const std::vector<Param>& params, Tokens::VarType::Type returnType, int arraySize, Scope<ASTBlockNode> blockNode)
    : name(name), params(params), returnType(returnType), returnSize(arraySize), blockNode(std::move(blockNode))
{
}

ASTWhileNode::ASTWhileNode(Scope<ASTExpressionNode> expr, Scope<ASTBlockNode> blockNode)
    : expr(std::move(expr)), blockNode(std::move(blockNode))
{
}

ASTForNode::ASTForNode(Scope<ASTVarDeclNode> variableDecl, Scope<ASTExpressionNode> expr, Scope<ASTAssignmentNode> assignment, Scope<ASTBlockNode> blockNode)
    : variableDecl(std::move(variableDecl)), expr(std::move(expr)), assignment(std::move(assignment)), blockNode(std::move(blockNode))
{
}

ASTPrintNode::ASTPrintNode(Scope<ASTExpressionNode> expr)
    : expr(std::move(expr))
{
}

ASTDelayNode::ASTDelayNode(Scope<ASTExpressionNode> delayExpr)
    : delayExpr(std::move(delayExpr))
{
}

ASTWriteNode::ASTWriteNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> colour)
    : x(std::move(x)), y(std::move(y)), colour(std::move(colour))
{
}

ASTWriteBoxNode::ASTWriteBoxNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y, Scope<ASTExpressionNode> w, Scope<ASTExpressionNode> h, Scope<ASTExpressionNode> colour)
    : x(std::move(x)), y(std::move(y)), w(std::move(w)), h(std::move(h)), colour(std::move(colour))
{
}

ASTReadNode::ASTReadNode(Scope<ASTExpressionNode> x, Scope<ASTExpressionNode> y)
    : x(std::move(x)), y(std::move(y))
{
}

ASTRandIntNode::ASTRandIntNode(Scope<ASTExpressionNode> max)
    : max(std::move(max))
{
}

ASTFuncCallNode::ASTFuncCallNode(const std::string& funcName)
    : funcName(funcName), args()
{
}

void ASTFuncCallNode::AddArg(Scope<ASTExpressionNode> arg)
{
    args.push_back(std::move(arg));
}

ASTCastNode::ASTCastNode(Tokens::VarType::Type castType, std::unique_ptr<ASTExpressionNode> expr)
    : castType(castType), expr(std::move(expr))
{
}

ASTArraySetNode::ASTArraySetNode(Scope<ASTExpressionNode> lit, int duplication)
    : duplication(duplication)
{
    literals.push_back(std::move(lit));
}

void ASTArraySetNode::AddLiterial(Scope<ASTExpressionNode> lit)
{
    literals.push_back(std::move(lit));
}

ASTArrayIndexNode::ASTArrayIndexNode(const std::string& name, Scope<ASTExpressionNode> index)
    : ASTIdentifierNode(name), name(name), index(std::move(index))
{
}
