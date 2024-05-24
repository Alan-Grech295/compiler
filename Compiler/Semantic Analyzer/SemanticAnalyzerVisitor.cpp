#include "SemanticAnalyzerVisitor.h"

void SemanticAnalyzerVisitor::visit(ASTBlockNode& node)
{
    symbolTable.PushScope();
    // Add function declarations before all statements
    // This allows function calls to be made before the function is defined
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
    ASSERT(symbolTable.contains(node.name), "Unidentified identifier \'" + node.name + "\'");
    auto& entry = symbolTable[node.name];
    PushType(entry.type, entry.arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTVarDeclNode& node)
{
    symbolTable.AddEntry(node.identifier->name, Entry(node.identifier->type, node.identifier->arraySize));

    node.identifier->accept(*this);
    node.value->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2, "Variable type and assigned value type do not match");
}

void SemanticAnalyzerVisitor::visit(ASTBinaryOpNode& node)
{
    node.left->accept(*this);
    node.right->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2, "Binary operation left and right types do not match");

    switch (node.type) 
    {
        case ASTBinaryOpNode::Type::ADD:
        case ASTBinaryOpNode::Type::SUBTRACT:
        case ASTBinaryOpNode::Type::MULTIPLY:
        case ASTBinaryOpNode::Type::MOD:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1), "Invalid value type for binary operation");
            PushType(type1);
            return;
        case ASTBinaryOpNode::Type::DIVIDE:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1), "Invalid value type for binary operation");
            // Division always returns a float
            PushType(VarType::Type::FLOAT);
            return;
        case ASTBinaryOpNode::Type::AND:
        case ASTBinaryOpNode::Type::OR:
            ASSERT(type1.first == VarType::Type::BOOL && !IS_ARRAY(type1), "Invalid value type for binary operation");
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::EQUAL:
        case ASTBinaryOpNode::Type::NOT_EQUAL:
            ASSERT(!IS_ARRAY(type1), "Cannot compare arrays");
            PushType(VarType::Type::BOOL);
            return;
        case ASTBinaryOpNode::Type::GREATER:
        case ASTBinaryOpNode::Type::LESS_THAN:
        case ASTBinaryOpNode::Type::GREATER_EQUAL:
        case ASTBinaryOpNode::Type::LESS_THAN_EQUAL:
            ASSERT(type1.first != VarType::Type::BOOL && !IS_ARRAY(type1), "Invalid value type for binary operation");
            PushType(VarType::Type::BOOL);
            return;
    }
}

void SemanticAnalyzerVisitor::visit(ASTNegateNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(!IS_ARRAY(type), "Cannot negated arrays");
    ASSERT(type.first == VarType::Type::INT || type.first == VarType::Type::FLOAT, "Can only negate 'float' or 'int' types");
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTNotNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();

    ASSERT(!IS_ARRAY(type), "Cannot 'not' arrays");
    ASSERT(type.first == VarType::Type::BOOL, "'Not' can only be applied to boolean types");
    PushType(type);
}

void SemanticAnalyzerVisitor::visit(ASTCastNode& node)
{
    // TODO: Add cast types
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(!IS_ARRAY(type), "Cannot cast arrays");

    PushType(node.castType);
}

void SemanticAnalyzerVisitor::visit(ASTAssignmentNode& node)
{
    node.identifier->accept(*this);
    node.expr->accept(*this);
    auto [type1, type2] = PopTypes();
    ASSERT(type1 == type2, "Assigned types are different. Use 'as' to cast types");
}

void SemanticAnalyzerVisitor::visit(ASTDecisionNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(!IS_ARRAY(type) && type.first == VarType::Type::BOOL, "If statement can only accept boolean expressions");

    node.trueStatement->accept(*this);

    if(node.falseStatement)
        node.falseStatement->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTReturnNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type.first == expectedRetType && type.second == expectedRetArrSize, "Returned value does not match expected value");
}

// Checks whether there is a return node in the current block
// Valid returns are those directly in the current block
// or if a return is defined in both the if and else statement
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
    ASSERT(symbolTable.InRootScope(), "Cannot declare functions inside a scope");
    ASSERT(node.name != "main", "Cannot call function 'main'");

    auto funcEntry = symbolTable[node.name];
    ASSERT(funcEntry.IsFunction(), node.name + " is not a function");

    symbolTable.PushScope(true);
    expectedRetType = node.returnType;
    expectedRetArrSize = node.returnSize;

    for (auto& param : funcEntry.funcData->params)
    {
        symbolTable.AddEntry(param.Name, Entry(param.Type, param.ArraySize));
    }

    node.blockNode->accept(*this);

    ASSERT(HasReturnNode(node.blockNode.get()), "No definite return was found. Make sure you return in the outer scope");

    expectedRetType = VarType::Type::UNKNOWN;
    expectedRetArrSize = -1;

    symbolTable.PopScope();
}

void SemanticAnalyzerVisitor::visit(ASTWhileNode& node)
{
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type.first == VarType::Type::BOOL && !IS_ARRAY(type), "While statement can only accept boolean expressions");

    node.blockNode->accept(*this);
}

void SemanticAnalyzerVisitor::visit(ASTForNode& node)
{
    symbolTable.PushScope();

    if(node.variableDecl)
        node.variableDecl->accept(*this);
    node.expr->accept(*this);

    auto type = PopType();
    ASSERT(type.first == VarType::Type::BOOL && !IS_ARRAY(type), "For statement can only accept boolean expressions");
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
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Delay requires an integer value as its 1st positional argument");
}

void SemanticAnalyzerVisitor::visit(ASTWriteNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write requires an integer value as its 1st positional argument");

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write requires an integer value as its 2nd positional argument");

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type), "Write requires a colour value as its 3rd positional argument");
}

void SemanticAnalyzerVisitor::visit(ASTWriteBoxNode& node)
{
    node.x->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write box requires an integer value as its 1st positional argument");

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write box requires an integer value as its 2nd positional argument");

    node.w->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write box requires an integer value as its 3rd positional argument");

    node.h->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Write box requires an integer value as its 4th positional argument");

    node.colour->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type), "Write box requires a colour value as its 1st positional argument");
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
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Read requires an integer value as its 1st positional argument");

    node.y->accept(*this);
    type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Read requires an integer value as its 2nd positional argument");

    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTRandIntNode& node)
{
    node.max->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::INT && !IS_ARRAY(type), "Random integer requires an integer value as its 1st positional argument");

    PushType(VarType::Type::INT);
}

void SemanticAnalyzerVisitor::visit(ASTFuncCallNode& node)
{
    ASSERT(symbolTable.contains(node.funcName), "\'" + node.funcName + "\' is not defined");

    auto& entry = symbolTable[node.funcName];
    ASSERT(node.args.size() == entry.funcData->params.size(), "Invalid number of arguments. Number of arguments passed: " + std::to_string(node.args.size()) + ", Number of arguments expected: " + std::to_string(entry.funcData->params.size()));

    auto funcIt = entry.funcData->params.begin();
    auto argIt = node.args.begin();
    for (; funcIt != entry.funcData->params.end() && argIt != node.args.end(); ++funcIt, ++argIt)
    {
        (*argIt)->accept(*this);
        auto type = PopType();
        ASSERT(funcIt->Type == type.first && funcIt->ArraySize == type.second, "Argument type does not match expected type");
    }

    PushType(entry.type, entry.arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTArraySetNode& node)
{
    ASSERT(node.duplication != 0, "Array of size 0 is not valid");

    node.literals[0]->accept(*this);
    auto type = PopType();
    int arraySize = node.duplication;
    if (node.duplication == -1)
    {
        arraySize = node.literals.size();
        for (int i = 1; i < node.literals.size(); i++)
        {
            node.literals[i]->accept(*this);
            ASSERT(type == PopType(), "Invalid array element type");
        }
    }

    PushType(type.first, arraySize);
}

void SemanticAnalyzerVisitor::visit(ASTArrayIndexNode& node)
{
    ASSERT(symbolTable.contains(node.name), "\'" + node.name + "\' is not defined");
    auto& entry = symbolTable[node.name];
    PushType(entry.type);
}

void SemanticAnalyzerVisitor::visit(ASTClearNode& node)
{
    node.expr->accept(*this);
    auto type = PopType();
    ASSERT(type.first == VarType::Type::COLOUR && !IS_ARRAY(type), "Clear requires a colour value as its 1st positional argument");
}
