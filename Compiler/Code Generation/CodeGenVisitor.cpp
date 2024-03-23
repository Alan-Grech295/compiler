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
    AddInstruction<FuncDeclInstruction>("main");
    node.blockNode->accept(*this);
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
    // For some reason, the top of the memory stack has index 0. This
    // means that the frame index for an indentifier depends on how many
    // memory frames are on the stack currently. To avoid this, the variable
    // stack index is stored in reverse (i.e. frameIndex 0 is the bottom of the stack)
    // and the actual value is calculated when it is to be pushed.
    AddInstruction<PushVarInstruction>(entry.index, VM_FRAME_INDEX(entry.frameIndex));
}

void CodeGenVisitor::visit(ASTVarDeclNode& node)
{
    int index = frameStack.back()->varCountRef->value++;
    int frameIndex = symbolTable.size() - 1;
    symbolTable.AddEntry(node.identifier->name, Entry(index, frameIndex));

    node.value->accept(*this);
    AddInstruction<PushInstruction>(index);
    AddInstruction<PushInstruction>(VM_FRAME_INDEX(frameIndex));
    AddInstruction<StoreInstruction>();
}

void CodeGenVisitor::visit(ASTBinaryOpNode& node)
{
    node.right->accept(*this);
    node.left->accept(*this);

#define X(type, instruction) case ASTBinaryOpNode::Type::type: AddInstruction<instruction>(); return;
    switch (node.type)
    {
        BIN_OP_INSTRUCTIONS
    }
#undef X
}

void CodeGenVisitor::visit(ASTNegateNode& node)
{
    node.expr->accept(*this);
    AddInstruction<PushInstruction>(0);
    AddInstruction<SubtractOpInstruction>();
}

void CodeGenVisitor::visit(ASTNotNode& node)
{
    node.expr->accept(*this);
    AddInstruction<NotInstruction>();
}

void CodeGenVisitor::visit(ASTCastNode& node)
{
    switch (node.castType)
    {
    case Tokens::VarType::Type::INT:
    {
        int index = frameStack.back()->varCountRef->value++;
        int frameIndex = symbolTable.size() - 1;

        node.expr->accept(*this);
        AddInstruction<PushInstruction>(index);
        AddInstruction<PushInstruction>(VM_FRAME_INDEX(frameIndex));
        AddInstruction<StoreInstruction>();

        AddInstruction<PushInstruction>(1);
        AddInstruction<PushVarInstruction>(index, VM_FRAME_INDEX(frameIndex));
        AddInstruction<ModOpInstruction>();

        AddInstruction<PushVarInstruction>(index, VM_FRAME_INDEX(frameIndex));
        AddInstruction<SubtractOpInstruction>();
    }
        break;
    case Tokens::VarType::Type::FLOAT:
        node.expr->accept(*this);
        break;
    case Tokens::VarType::Type::BOOL:
        node.expr->accept(*this);
        break;
    case Tokens::VarType::Type::COLOUR:
        node.expr->accept(*this);
        break;
    }
}

void CodeGenVisitor::visit(ASTAssignmentNode& node)
{
    node.expr->accept(*this);
    StoreVar(node.identifier->name);
}

void CodeGenVisitor::visit(ASTDecisionNode& node)
{
    node.expr->accept(*this);

    // The if statement has a true and false part
    if (node.falseStatement)
    {
        InstructionRef<PushRelativeInstruction> jmpIfTrue = { AddInstruction<PushRelativeInstruction>(), *instructionList };
        AddInstruction<CompareJumpInstruction>();

        node.falseStatement->accept(*this);
        InstructionRef<PushRelativeInstruction> jmpIfFalse = { AddInstruction<PushRelativeInstruction>(), *instructionList };
        int lastFalse = AddInstruction<JumpInstruction>();
        jmpIfTrue->value = (lastFalse + 1) - jmpIfTrue.instructionIndex;

        node.trueStatement->accept(*this);
        jmpIfFalse->value = instructionList->size() - jmpIfFalse.instructionIndex;
    }
    // The if statement only has a true part
    else
    {
        // Invert boolean to jump if the condition is false
        AddInstruction<NotInstruction>();
        InstructionRef<PushRelativeInstruction> jmpIfFalse = { AddInstruction<PushRelativeInstruction>(), *instructionList };
        AddInstruction<CompareJumpInstruction>();

        node.trueStatement->accept(*this);

        jmpIfFalse->value = instructionList->size() - jmpIfFalse.instructionIndex;
    }
}

void CodeGenVisitor::visit(ASTReturnNode& node)
{
    node.expr->accept(*this);
    for (int i = 0; i < symbolTable.size() - symbolTable.isolatedLevel; i++)
    {
        AddInstruction<CloseFrameInstruction>();
    }
    AddInstruction<ReturnInstruction>();
}

void CodeGenVisitor::visit(ASTFunctionNode& node)
{
    AddFuncInstructionList();

    AddInstruction<FuncDeclInstruction>(node.name);

    symbolTable.PushScope(true);

    int index = 0;
    int frameIndex = symbolTable.size() - 1;
    for (auto& param : node.params)
    {
        symbolTable.AddEntry(param.Name, Entry(index, frameIndex));
        index++;
    }

    node.blockNode->accept(*this);

    symbolTable.PopScope();

    SwapMainList();
}

void CodeGenVisitor::visit(ASTWhileNode& node)
{
    int conditionLine = instructionList->size();
    node.expr->accept(*this);
    AddInstruction<NotInstruction>();
    InstructionRef<PushRelativeInstruction> jmpIfFalse = { AddInstruction<PushRelativeInstruction>(), *instructionList };
    AddInstruction<CompareJumpInstruction>();

    node.blockNode->accept(*this);
    
    AddInstruction<PushRelativeInstruction>(REL_LINE(conditionLine));
    AddInstruction<JumpInstruction>();
    jmpIfFalse->value = instructionList->size() - jmpIfFalse.instructionIndex;
}

void CodeGenVisitor::visit(ASTForNode& node)
{
    PushScope();

    if (node.variableDecl)
        node.variableDecl->accept(*this);

    int loopLine = instructionList->size();
    node.expr->accept(*this);
    AddInstruction<NotInstruction>();
    InstructionRef<PushRelativeInstruction> jmpIfFalse = { AddInstruction<PushRelativeInstruction>(), *instructionList };
    AddInstruction<CompareJumpInstruction>();

    node.blockNode->accept(*this);

    if (node.assignment)
        node.assignment->accept(*this);

    AddInstruction<PushRelativeInstruction>(REL_LINE(loopLine));
    AddInstruction<JumpInstruction>();

    jmpIfFalse->value = instructionList->size() - jmpIfFalse.instructionIndex;

    PopScope();
}

void CodeGenVisitor::visit(ASTPrintNode& node)
{
    node.expr->accept(*this);
    AddInstruction<PrintInstruction>();
}

void CodeGenVisitor::visit(ASTDelayNode& node)
{
    node.delayExpr->accept(*this);
    AddInstruction<DelayInstruction>();
}

void CodeGenVisitor::visit(ASTWriteNode& node)
{
    node.colour->accept(*this);
    node.y->accept(*this);
    node.x->accept(*this);
    AddInstruction<WriteInstruction>();
}

void CodeGenVisitor::visit(ASTWriteBoxNode& node)
{
    node.colour->accept(*this);
    node.h->accept(*this);
    node.w->accept(*this);
    node.y->accept(*this);
    node.x->accept(*this);
    AddInstruction<WriteBoxInstruction>();
}

void CodeGenVisitor::visit(ASTWidthNode& node)
{
    AddInstruction<WidthInstruction>();
}

void CodeGenVisitor::visit(ASTHeightNode& node)
{
    AddInstruction<HeightInstruction>();
}

void CodeGenVisitor::visit(ASTReadNode& node)
{
    node.y->accept(*this);
    node.x->accept(*this);
    AddInstruction<ReadInstruction>();
}

void CodeGenVisitor::visit(ASTRandIntNode& node)
{
    node.max->accept(*this);
    AddInstruction<RandIntInstruction>();
}

void CodeGenVisitor::visit(ASTFuncCallNode& node)
{
    for (auto it = node.args.rbegin(); it != node.args.rend(); ++it)
    {
        (*it)->accept(*this);
    }

    AddInstruction<PushInstruction>((int)node.args.size());
    AddInstruction<PushFuncInstruction>(node.funcName);
    AddInstruction<CallInstruction>();
}

std::string CodeGenVisitor::Finalize()
{
    std::string program = GetInstructionListString(mainInstructionList) + "\n";
    for (auto& list : funcInstructionLists)
    {
        program += GetInstructionListString(list) + "\n";
    }
    program.pop_back();
    return program;
}

std::string CodeGenVisitor::GetInstructionListString(InstructionList& instructionList)
{
    std::string instructions = "";
    for (auto& instruction : instructionList)
    {
        instructions += instruction->ToString() + "\n";
    }
    instructions.pop_back();
    return instructions;
}
