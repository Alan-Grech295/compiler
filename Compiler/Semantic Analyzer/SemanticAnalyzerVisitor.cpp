#include "SemanticAnalyzerVisitor.h"

// TODO: Check for functions called main
// TODO: Check for returns in a functions (including if and elses)

void SemanticAnalyzerVisitor::visit(ASTBlockNode& node)
{
    symbolTable.PushScope();
    // Add function declarations before all statements
    for (auto& statement : node.statements)
    {
        ASTFunctionNode* funcNode = dynamic_cast<ASTFunctionNode*>(statement.get());
        if (funcNode)
        {
            symbolTable.AddEntry(funcNode->name, Entry(funcNode->returnType, funcNode->returnSize, funcNode->params));
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
    auto& entry = symbolTable[node.name];
    PushType(entry.type, entry.arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTVarDeclNode& node)
{
    symbolTable.AddEntry(node.identifier->name, Entry(node.identifier->type, node.identifier->arraySize));

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
        case ASTBinaryOpNode::Type::MOD:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1));
            PushType(type1);
            return;
        case ASTBinaryOpNode::Type::DIVIDE:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1));
            PushType(VarType::Type::FLOAT);
            return;
        case ASTBinaryOpNode::Type::AND:
        case ASTBinaryOpNode::Type::OR:
            ASSERT(type1.first == VarType::Type::BOOL && !IS_ARRAY(type1));
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::EQUAL:
        case ASTBinaryOpNode::Type::NOT_EQUAL:
            ASSERT(type1.second == -1);
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::GREATER:
        case ASTBinaryOpNode::Type::LESS_THAN:
        case ASTBinaryOpNode::Type::GREATER_EQUAL:
        case ASTBinaryOpNode::Type::LESS_THAN_EQUAL:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1));
            PushType(VarType::Type::BOOL);
            return;
    }
}

void SemanticAnalyzerVisitor::visit(ASTNegateNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(!IS_ARRAY(type));
    ASSERT(type.first == VarType::Type::INT || type.first == VarType::Type::FLOAT);
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTNotNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(!IS_ARRAY(type));
    ASSERT(type.first == VarType::Type::BOOL);
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTCastNode& node)
{
    // TODO: Add cast types
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(!IS_ARRAY(type));

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
    ASSERT(!IS_ARRAY(type));
    ASSERT(type.first == VarType::Type::BOOL);

    node.trueStatement->accept(*this);

    if(node.falseStatement)
        node.falseStatement->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTReturnNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type.first == expectedRetType && type.second == expectedRetArrSize);
}

static bool HasReturnNode(ASTBlockNode* blockNode) 
{
    for (auto& statement : blockNode->statements)
    {
        if (dynamic_cast<ASTReturnNode*>(statement.get()))
            return true;

        if (auto decisionNode = dynamic_cast<ASTDecisionNode*>(statement.get()))
        {
            if (decisionNode->falseStatement && 
                HasReturnNode(decisionNode->trueStatement.get()) &&
                HasReturnNode(decisionNode->falseStatement.get()))
            {
                return true;
            }
        }
    }

    return false;
}

void SemanticAnalyzerVisitor::visit(ASTFunctionNode& node)
{
    ASSERT(symbolTable.InRootScope());
    ASSERT(node.name != "main");

    auto funcEntry = symbolTable[node.name];
    ASSERT(funcEntry.IsFunction());

    symbolTable.PushScope(true);
    expectedRetType = node.returnType;
    expectedRetArrSize = node.returnSize;

    for (auto& param : funcEntry.funcData->params)
    {
        symbolTable.AddEntry(param.Name, Entry(param.Type, param.ArraySize));
    }

    node.blockNode->accept(*this);

    ASSERT(HasReturnNode(node.blockNode.get()));

    expectedRetType = VarType::Type::UNKNOWN;
    expectedRetArrSize = -1;

    symbolTable.PopScope();
}

void SemanticAnalyzerVisitor::visit(ASTWhileNode& node)
{
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type.first == VarType::Type::BOOL && !IS_ARRAY(type));

    node.blockNode->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTForNode& node)
{
    symbolTable.PushScope();

    if(node.variableDecl)
        node.variableDecl->accept(*this);
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type.first == VarType::Type::BOOL && !IS_ARRAY(type));
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
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));
}

void SemanticAnalyzerVisitor::visit(ASTWriteNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type));
}

void SemanticAnalyzerVisitor::visit(ASTWriteBoxNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.w->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.h->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type));
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
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTRandIntNode& node)
{
    node.max->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type));

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
        ASSERT(funcIt->Type == type.first && funcIt->ArraySize == type.second);
    }

    PushType(entry.type, entry.arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTArraySetNode& node)
{
    ASSERT(node.duplication != 0);

    node.literals[0]->accept(*this);
    auto type = PopType();
    int arraySize = node.duplication;
    if (node.duplication == -1)
    {
        arraySize = node.literals.size();
        for (int i = 1; i < node.literals.size(); i++)
        {
            node.literals[i]->accept(*this);
            ASSERT(type == PopType());
        }
    }

    PushType(type.first, arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTArrayIndexNode& node)
{
    ASSERT(symbolTable.contains(node.name));
    auto& entry = symbolTable[node.name];
    PushType(entry.type);
}

void SemanticAnalyzerVisitor::visit(ASTClearNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type));
}
