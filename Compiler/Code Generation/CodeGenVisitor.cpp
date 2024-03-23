#include "CodeGenVisitor.h"

void CodeGenVisitor::visit(ASTBlockNode& node)
{
    PushScope();

    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }

    PopScope();
}

void CodeGenVisitor::visit(ASTProgramNode& node)
{
    //TEMP
    AddInstruction<FuncDeclInstruction>("main");
    node.blockNode->accept(*this);
    //TEMP
    AddInstruction<HaltInstruction>();
}

void CodeGenVisitor::visit(ASTIntLiteralNode& node)
{
    AddInstruction<PushInstruction>(node.value);
}

void CodeGenVisitor::visit(ASTFloatLiteralNode& node)
{
    AddInstruction<PushInstruction>(node.value);
}

void CodeGenVisitor::visit(ASTBooleanLiteralNode& node)
{
    AddInstruction<PushInstruction>(node.value);
}

void CodeGenVisitor::visit(ASTColourLiteralNode& node)
{
    AddInstruction<PushInstruction>(node.value);
}

void CodeGenVisitor::visit(ASTIdentifierNode& node)
{
    auto& entry = symbolTable[node.name];
    AddInstruction<PushVarInstruction>(entry.index, entry.frameIndex);
}

void CodeGenVisitor::visit(ASTVarDeclNode& node)
{
    int index = frameStack.back()->varCountRef->value++;
    int frameIndex = symbolTable.size() - 1;
    symbolTable.AddEntry(node.identifier->name, Entry(index, frameIndex));

    node.value->accept(*this);
    AddInstruction<PushInstruction>(index);
    AddInstruction<PushInstruction>(frameIndex);
    AddInstruction<StoreInstruction>();
}

void CodeGenVisitor::visit(ASTBinaryOpNode& node)
{
    node.right->accept(*this);
    node.left->accept(*this);

    switch (node.type)
    {
    case ASTBinaryOpNode::Type::ADD:
        AddInstruction<AddOpInstruction>();
        return;
    case ASTBinaryOpNode::Type::SUBTRACT:
        AddInstruction<SubtractOpInstruction>();
        return;
    case ASTBinaryOpNode::Type::MULTIPLY:
        AddInstruction<MultiplyOpInstruction>();
        return;
    case ASTBinaryOpNode::Type::DIVIDE:
        AddInstruction<DivideOpInstruction>();
        return;
    }
}

void CodeGenVisitor::visit(ASTNegateNode& node)
{
}

void CodeGenVisitor::visit(ASTNotNode& node)
{
    node.expr->accept(*this);
    AddInstruction<NotInstruction>();
}

void CodeGenVisitor::visit(ASTCastNode& node)
{
}

void CodeGenVisitor::visit(ASTAssignmentNode& node)
{
    node.expr->accept(*this);
    StoreVar(node.identifier->name);
}

void CodeGenVisitor::visit(ASTDecisionNode& node)
{
}

void CodeGenVisitor::visit(ASTReturnNode& node)
{
}

void CodeGenVisitor::visit(ASTFunctionNode& node)
{
}

void CodeGenVisitor::visit(ASTWhileNode& node)
{
}

void CodeGenVisitor::visit(ASTForNode& node)
{
}

void CodeGenVisitor::visit(ASTPrintNode& node)
{
    node.expr->accept(*this);
    AddInstruction<PrintInstruction>();
}

void CodeGenVisitor::visit(ASTDelayNode& node)
{
}

void CodeGenVisitor::visit(ASTWriteNode& node)
{
}

void CodeGenVisitor::visit(ASTWriteBoxNode& node)
{
}

void CodeGenVisitor::visit(ASTWidthNode& node)
{
}

void CodeGenVisitor::visit(ASTHeightNode& node)
{
}

void CodeGenVisitor::visit(ASTReadNode& node)
{
}

void CodeGenVisitor::visit(ASTRandIntNode& node)
{
}

void CodeGenVisitor::visit(ASTFuncCallNode& node)
{
}

std::string CodeGenVisitor::Finalize()
{
    std::string program = "";
    for (auto& instruction : instructionList)
    {
        program += instruction->ToString() + "\n";
    }
    program.pop_back();
    return program;
}
