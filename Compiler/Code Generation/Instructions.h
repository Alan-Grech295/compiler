#pragma once
#include <vector>
#include <string>
#include <format>
#include <cassert>

#include "../Utils/Utils.h"


class Instruction
{
public:
    enum class Type
    {
        PUSH,
        FUNC_DECL,
        HALT,
        OPEN_FRAME,
        CLOSE_FRAME,
        STORE,

        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        NOT,

        PRINT,
    };
public:
    Instruction(Type type)
        : type(type)
    {}
    virtual ~Instruction() = default;

    virtual const std::string ToString() = 0;

    template<typename T>
    T& As()
    {
        assert("Invalid casting type" && T::Type == type);
        return *static_cast<T*>(this);
    }
public:
    Type type;
};

using InstructionList = std::vector<Scope<Instruction>>;

template<typename T>
struct InstructionRef
{
public:
    InstructionRef(int instructionIndex, InstructionList& instructions)
        : instructionIndex(instructionIndex), instructions(instructions)
    {}


    T& operator*()
    {
        return *static_cast<T*>(instructions[instructionIndex].get());
    }

    T* operator->()
    {
        return static_cast<T*>(instructions[instructionIndex].get());
    }

public:
    int instructionIndex;
    InstructionList& instructions;
};

class PushInstruction : public Instruction
{
public:
    PushInstruction(int value)
        : Instruction(Type::PUSH), value(value)
    {}

    PushInstruction(float value)
        : Instruction(Type::PUSH), value(value)
    {}

    virtual const std::string ToString() override { return std::format("push {}", value); }
public:
    float value;

    static const Type Type = Type::PUSH;
};

class PushVarInstruction : public Instruction
{
public:
    PushVarInstruction(int index, int frameIndex)
        : Instruction(Type::PUSH), index(index), frameIndex(frameIndex)
    {}

    virtual const std::string ToString() override { return std::format("push [{}:{}]", index, frameIndex); }
public:
    int index;
    int frameIndex;

    static const Type Type = Type::PUSH;
};

class FuncDeclInstruction : public Instruction
{
public:
    FuncDeclInstruction(const std::string& funcName)
        : Instruction(Type::FUNC_DECL), funcName(funcName)
    {}

    virtual const std::string ToString() override { return "." + funcName; }

public:
    std::string funcName;

    static const Type Type = Type::FUNC_DECL;
};

class HaltInstruction : public Instruction
{
public:
    HaltInstruction()
        : Instruction(Type::HALT)
    {}

    virtual const std::string ToString() override { return "halt"; }

    static const Type Type = Type::HALT;
};

class OpenFrameInstruction : public Instruction
{
public:
    OpenFrameInstruction(int varCountInstrIndex, InstructionList& instructionList)
        : Instruction(Type::OPEN_FRAME), varCountRef(varCountInstrIndex, instructionList)
    {}

    virtual const std::string ToString() override { return "oframe"; }
public:
    InstructionRef<PushInstruction> varCountRef;

    static const Type Type = Type::OPEN_FRAME;
};


class CloseFrameInstruction : public Instruction
{
public:
    CloseFrameInstruction()
        : Instruction(Type::CLOSE_FRAME)
    {}

    virtual const std::string ToString() override { return "cframe"; }
public:
    static const Type Type = Type::CLOSE_FRAME;
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction()
        : Instruction(Type::STORE)
    {}

    virtual const std::string ToString() override { return "st"; }

    static const Type Type = Type::STORE;
};

class AddOpInstruction : public Instruction
{
public:
    AddOpInstruction()
        : Instruction(Type::ADD)
    {}

    virtual const std::string ToString() override { return "add"; }

    static const Type Type = Type::ADD;
};

class SubtractOpInstruction : public Instruction
{
public:
    SubtractOpInstruction()
        : Instruction(Type::SUBTRACT)
    {}

    virtual const std::string ToString() override { return "sub"; }

    static const Type Type = Type::SUBTRACT;
};

class MultiplyOpInstruction : public Instruction
{
public:
    MultiplyOpInstruction()
        : Instruction(Type::MULTIPLY)
    {}

    virtual const std::string ToString() override { return "mul"; }

    static const Type Type = Type::MULTIPLY;
};

class DivideOpInstruction : public Instruction
{
public:
    DivideOpInstruction()
        : Instruction(Type::DIVIDE)
    {}

    virtual const std::string ToString() override { return "div"; }

    static const Type Type = Type::DIVIDE;
};

class PrintInstruction : public Instruction
{
public:
    PrintInstruction()
        : Instruction(Type::PRINT)
    {}

    virtual const std::string ToString() override { return "print"; }

    static const Type Type = Type::PRINT;
};

class NotInstruction : public Instruction
{
public:
    NotInstruction()
        : Instruction(Type::NOT)
    {}

    virtual const std::string ToString() override { return "not"; }

    static const Type Type = Type::NOT;
};