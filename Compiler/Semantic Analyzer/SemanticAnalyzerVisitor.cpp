#include "SemanticAnalyzerVisitor.h"

void SemanticAnalyzerVisitor::visit(ASTBlockNode& node)
{
    symbolTable.PushScope();
    // Add function declarations before all statements
    for (auto& statement : node.statements)
    {
        ASTFunctionNode* funcNode = dynamic_cast<ASTFunctionNode*>(statement.get());
        if (funcNode)
        {
            symbolTable.AddFunction(funcNode->name, funcNode->returnType, funcNode->params);
        }
    }

    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }
    symbolTable.PopScope();
}

void SemanticAnalyzerVisitor::visit(ASTProgramNode& node)
{
    node.blockNode->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTIntLiteralNode& node)
{
    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTFloatLiteralNode& node)
{
    PushType(VarType::Type::FLOAT);
}

void SemanticAnalyzerVisitor::visit(ASTBooleanLiteralNode& node)
{
    PushType(VarType::Type::BOOL);
}

void SemanticAnalyzerVisitor::visit(ASTColourLiteralNode& node)
{
    PushType(VarType::Type::COLOUR);
}

void SemanticAnalyzerVisitor::visit(ASTIdentifierNode& node)
{
    ASSERT(symbolTable.contains(node.name));
    PushType(symbolTable[node.name].type);
}

void SemanticAnalyzerVisitor::visit(ASTVarDeclNode& node)
{
    symbolTable.AddVariable(node.identifier->name, node.identifier->type);

    node.identifier->accept(*this);
    node.value->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2);
}

void SemanticAnalyzerVisitor::visit(ASTBinaryOpNode& node)
{
    node.left->accept(*this);
    node.right->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2);

    switch (node.type) 
    {
        case ASTBinaryOpNode::Type::ADD:
        case ASTBinaryOpNode::Type::SUBTRACT:
        case ASTBinaryOpNode::Type::MULTIPLY:
            ASSERT(type1 != VarType::Type::BOOL);
            PushType(type1);
            return;
        case ASTBinaryOpNode::Type::DIVIDE:
            ASSERT(type1 != VarType::Type::BOOL);
            PushType(VarType::Type::FLOAT);
            return;
        case ASTBinaryOpNode::Type::AND:
        case ASTBinaryOpNode::Type::OR:
            ASSERT(type1 == VarType::Type::BOOL);
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::EQUAL:
        case ASTBinaryOpNode::Type::NOT_EQUAL:
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::GREATER:
        case ASTBinaryOpNode::Type::LESS_THAN:
        case ASTBinaryOpNode::Type::GREATER_EQUAL:
        case ASTBinaryOpNode::Type::LESS_THAN_EQUAL:
            ASSERT(type1 != VarType::Type::BOOL);
            PushType(VarType::Type::BOOL);
            return;
    }
}

void SemanticAnalyzerVisitor::visit(ASTNegateNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(type == VarType::Type::INT || type == VarType::Type::FLOAT);
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTNotNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(type == VarType::Type::BOOL);
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTCastNode& node)
{
    // TODO: Add cast types
    node.expr->accept(*this);
    auto type = PopType();

    PushType(node.castType);
}

void SemanticAnalyzerVisitor::visit(ASTAssignmentNode& node)
{
    node.identifier->accept(*this);
    node.expr->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2);
}

void SemanticAnalyzerVisitor::visit(ASTDecisionNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::BOOL);

    node.trueStatement->accept(*this);

    if(node.falseStatement)
        node.falseStatement->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTReturnNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type == expectedRetType);
}

void SemanticAnalyzerVisitor::visit(ASTFunctionNode& node)
{
    ASSERT(symbolTable.InRootScope());

    auto funcEntry = symbolTable[node.name];
    ASSERT(funcEntry.IsFunction());

    symbolTable.PushScope(true);
    expectedRetType = node.returnType;

    for (auto& param : funcEntry.funcData->params)
    {
        symbolTable.AddVariable(param.Name, param.Type);
    }

    node.blockNode->accept(*this);

    expectedRetType = VarType::Type::UNKNOWN;

    symbolTable.PopScope();
}

void SemanticAnalyzerVisitor::visit(ASTWhileNode& node)
{
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type == VarType::Type::BOOL);

    node.blockNode->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTForNode& node)
{
    symbolTable.PushScope();

    if(node.variableDecl)
        node.variableDecl->accept(*this);
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type == VarType::Type::BOOL);
    if(node.assignment)
        node.assignment->accept(*this);

    node.blockNode->accept(*this);

    symbolTable.PopScope();
}

void SemanticAnalyzerVisitor::visit(ASTPrintNode& node)
{
    node.expr->accept(*this);
    PopType();
}

void SemanticAnalyzerVisitor::visit(ASTDelayNode& node)
{
    node.delayExpr->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTWriteNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.y->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::COLOUR);
}

void SemanticAnalyzerVisitor::visit(ASTWriteBoxNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.y->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.w->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.h->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::COLOUR);
}

void SemanticAnalyzerVisitor::visit(ASTWidthNode& node)
{
    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTHeightNode& node)
{
    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTReadNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::INT);

    node.y->accept(*this);
    type = PopType();
    ASSERT(type == VarType::Type::INT);

    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTRandIntNode& node)
{
    node.max->accept(*this);
    auto type = PopType();
    ASSERT(type == VarType::Type::INT);

    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTFuncCallNode& node)
{
    ASSERT(symbolTable.contains(node.funcName));

    auto& entry = symbolTable[node.funcName];
    ASSERT(node.args.size() == entry.funcData->params.size());

    auto funcIt = entry.funcData->params.begin();
    auto argIt = node.args.begin();
    for (; funcIt != entry.funcData->params.end() && argIt != node.args.end(); ++funcIt, ++argIt)
    {
        (*argIt)->accept(*this);
        auto type = PopType();
        ASSERT(funcIt->Type == type);
    }
}
