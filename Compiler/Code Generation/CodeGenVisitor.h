#pragma once
#include "../Utils/Visitor.h"
#include "../Parser/ASTNodes.h"
#include "../Utils/SymbolTable.h"
#include "Instructions.h"

class CodeGenVisitor : public Visitor
{
public:
    struct Entry
    {
    public:
        Entry() = default;

        Entry(int index, int frameIndex)
            : index(index), frameIndex(frameIndex)
        {}

        Entry(int index, int frameIndex, int arraySize)
            : index(index), frameIndex(frameIndex), arraySize(arraySize)
        {
        }

        const bool IsFunction() const { return false; }
        const bool IsArray() const { return arraySize > 0; }
    public:
        int index;
        int frameIndex;
        int arraySize = 0;
    };
public:
#define VM_FRAME_INDEX(index) symbolTable.size() - index - 1
#define REL_LINE(index) index - instructionList->size()
#define BIN_OP_INSTRUCTIONS \
                            X(ADD, AddOpInstruction) \
                            X(SUBTRACT, SubtractOpInstruction) \
                            X(MULTIPLY, MultiplyOpInstruction) \
                            X(DIVIDE, DivideOpInstruction) \
                            X(GREATER, GreaterThanInstruction) \
                            X(GREATER_EQUAL, GreaterThanEqualInstruction) \
                            X(LESS_THAN, LessThanInstruction) \
                            X(LESS_THAN_EQUAL, LessThanEqualInstruction) \
                            X(EQUAL, EqualInstruction) 

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

    std::string Finalize();

    std::string GetInstructionListString(InstructionList& instructionList);

    template<typename T, typename... Args>
    int AddInstruction(Args&&... args)
    {
        instructionList->emplace_back(CreateScope<T>(std::forward<Args>(args)...));
        return instructionList->size() - 1;
    }

    int AddInstruction(Scope<Instruction> instruction)
    {
        instructionList->emplace_back(std::move(instruction));
        return instructionList->size() - 1;
    }

    void PushScope()
    {
        int pushIndex = AddInstruction<PushInstruction>(0);
        int oframeIndex = AddInstruction<OpenFrameInstruction>(pushIndex, *instructionList);
        frameStack.emplace_back(oframeIndex, *instructionList);
        symbolTable.PushScope();
    }

    void PopScope()
    {
        AddInstruction<CloseFrameInstruction>();
        frameStack.pop_back();
        symbolTable.PopScope();
    }

    void StoreVar(const std::string& name, Scope<ASTExpressionNode> index = nullptr)
    {
        auto& entry = symbolTable[name];
        if (entry.IsArray())
        {
            index->accept(*this);
            AddInstruction<PushInstruction>(entry.arraySize - 1);
            AddInstruction<SubtractOpInstruction>();
            AddInstruction<PushInstruction>(entry.index);
            AddInstruction<AddOpInstruction>();
        }
        else
        {
            AddInstruction<PushInstruction>(entry.index);
        }
        AddInstruction<PushInstruction>(VM_FRAME_INDEX(entry.frameIndex));
        AddInstruction<StoreInstruction>();
    }

    void AddFuncInstructionList()
    {
        instructionList = &funcInstructionLists.emplace_back();
    }

    void SwapMainList()
    {
        instructionList = &mainInstructionList;
    }

private:
    SymbolTable<Entry> symbolTable{};
    InstructionList* instructionList = &mainInstructionList;
    std::vector<InstructionList> funcInstructionLists;
    InstructionList mainInstructionList{};
    std::vector<InstructionRef<OpenFrameInstruction>> frameStack{};


    // Inherited via Visitor
    void visit(ASTArraySetNode& node) override;


    // Inherited via Visitor
    void visit(ASTArrayIndexNode& node) override;

};

