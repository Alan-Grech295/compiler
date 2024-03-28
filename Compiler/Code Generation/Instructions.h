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
        RETURN,

        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MOD,
        NOT,

        GREATER,
        GREATER_EQUAL,
        LESS_THAN,
        LESS_THAN_EQUAL,
        EQUAL,
        AND,
        OR,

        PRINT,
        RAND_INT,
        DELAY,
        WRITE,
        WRITE_BOX,
        READ,
        WIDTH,
        HEIGHT,
        CLEAR,

        JUMP,
        CJUMP,
        CALL,
        DROP,

        DUP_ARRAY,
        STORE_ARRAY,
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
    PushInstruction()
        : Instruction(Type::PUSH), value(0)
    {}

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

class PushFuncInstruction : public Instruction
{
public:
    PushFuncInstruction(const std::string& funcName)
        : Instruction(Type::PUSH), funcName(funcName)
    {}

    virtual const std::string ToString() override { return "push ." + funcName; }

public:
    std::string funcName;

    static const Type Type = Type::PUSH;
};

class PushRelativeInstruction : public Instruction
{
public:
    PushRelativeInstruction()
        : Instruction(Type::PUSH), value(0)
    {}

    PushRelativeInstruction(int value)
        : Instruction(Type::PUSH), value(value)
    {}

    virtual const std::string ToString() override { return std::format("push #PC{}{}", value >= 0 ? "+" : "", value); }
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

class PushArrayInstruction : public Instruction
{
public:
    PushArrayInstruction(int index, int frameIndex)
        : Instruction(Type::PUSH), index(index), frameIndex(frameIndex)
    {}

    virtual const std::string ToString() override { return std::format("pusha [{}:{}]", index, frameIndex); }
public:
    int index;
    int frameIndex;

    static const Type Type = Type::PUSH;
};

class PushArrayIndexInstruction : public Instruction
{
public:
    PushArrayIndexInstruction(int index, int frameIndex)
        : Instruction(Type::PUSH), index(index), frameIndex(frameIndex)
    {}

    virtual const std::string ToString() override { return std::format("push +[{}:{}]", index, frameIndex); }
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

class CallInstruction : public Instruction
{
public:
    CallInstruction()
        : Instruction(Type::CALL)
    {}

    virtual const std::string ToString() override { return "call"; }

public:
    static const Type Type = Type::CALL;
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

class AndOpInstruction : public Instruction
{
public:
    AndOpInstruction()
        : Instruction(Type::AND)
    {}

    virtual const std::string ToString() override { return "and"; }

    static const Type Type = Type::AND;
};

class OrOpInstruction : public Instruction
{
public:
    OrOpInstruction()
        : Instruction(Type::OR)
    {}

    virtual const std::string ToString() override { return "or"; }

    static const Type Type = Type::OR;
};

class ModOpInstruction : public Instruction
{
public:
    ModOpInstruction()
        : Instruction(Type::MOD)
    {}

    virtual const std::string ToString() override { return "mod"; }

    static const Type Type = Type::MOD;
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

class PrintArrayInstruction : public Instruction
{
public:
    PrintArrayInstruction()
        : Instruction(Type::PRINT)
    {}

    virtual const std::string ToString() override { return "printa"; }

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

class RandIntInstruction : public Instruction
{
public:
    RandIntInstruction()
        : Instruction(Type::RAND_INT)
    {}

    virtual const std::string ToString() override { return "irnd"; }

    static const Type Type = Type::RAND_INT;
};


class GreaterThanInstruction : public Instruction
{
public:
    GreaterThanInstruction()
        : Instruction(Type::GREATER)
    {}

    virtual const std::string ToString() override { return "gt"; }

    static const Type Type = Type::GREATER;
};

class GreaterThanEqualInstruction : public Instruction
{
public:
    GreaterThanEqualInstruction()
        : Instruction(Type::GREATER_EQUAL)
    {}

    virtual const std::string ToString() override { return "ge"; }

    static const Type Type = Type::GREATER_EQUAL;
};

class LessThanInstruction : public Instruction
{
public:
    LessThanInstruction()
        : Instruction(Type::LESS_THAN)
    {}

    virtual const std::string ToString() override { return "lt"; }

    static const Type Type = Type::LESS_THAN;
};

class LessThanEqualInstruction : public Instruction
{
public:
    LessThanEqualInstruction()
        : Instruction(Type::LESS_THAN_EQUAL)
    {}

    virtual const std::string ToString() override { return "le"; }

    static const Type Type = Type::LESS_THAN_EQUAL;
};

class EqualInstruction : public Instruction
{
public:
    EqualInstruction()
        : Instruction(Type::EQUAL)
    {}

    virtual const std::string ToString() override { return "eq"; }

    static const Type Type = Type::EQUAL;
};

class JumpInstruction : public Instruction
{
public:
    JumpInstruction()
        : Instruction(Type::JUMP)
    {}

    virtual const std::string ToString() override { return "jmp"; }

    static const Type Type = Type::JUMP;
};

class CompareJumpInstruction : public Instruction
{
public:
    CompareJumpInstruction()
        : Instruction(Type::CJUMP)
    {}

    virtual const std::string ToString() override { return "cjmp"; }

    static const Type Type = Type::CJUMP;
};

class DelayInstruction : public Instruction
{
public:
    DelayInstruction()
        : Instruction(Type::DELAY)
    {}

    virtual const std::string ToString() override { return "delay"; }

    static const Type Type = Type::DELAY;
};

class WidthInstruction : public Instruction
{
public:
    WidthInstruction()
        : Instruction(Type::WIDTH)
    {}

    virtual const std::string ToString() override { return "width"; }

    static const Type Type = Type::WIDTH;
};

class HeightInstruction : public Instruction
{
public:
    HeightInstruction()
        : Instruction(Type::HEIGHT)
    {}

    virtual const std::string ToString() override { return "height"; }

    static const Type Type = Type::HEIGHT;
};

class WriteInstruction : public Instruction
{
public:
    WriteInstruction()
        : Instruction(Type::WRITE)
    {}

    virtual const std::string ToString() override { return "write"; }

    static const Type Type = Type::WRITE;
};

class WriteBoxInstruction : public Instruction
{
public:
    WriteBoxInstruction()
        : Instruction(Type::WRITE_BOX)
    {}

    virtual const std::string ToString() override { return "writebox"; }

    static const Type Type = Type::WRITE_BOX;
};

class ReadInstruction : public Instruction
{
public:
    ReadInstruction()
        : Instruction(Type::READ)
    {}

    virtual const std::string ToString() override { return "read"; }

    static const Type Type = Type::READ;
};

class ClearInstruction : public Instruction
{
public:
    ClearInstruction()
        : Instruction(Type::CLEAR)
    {}

    virtual const std::string ToString() override { return "clear"; }

    static const Type Type = Type::CLEAR;
};

class ReturnInstruction : public Instruction
{
public:
    ReturnInstruction()
        : Instruction(Type::RETURN)
    {}

    virtual const std::string ToString() override { return "ret"; }

    static const Type Type = Type::RETURN;
};

class DropInstruction : public Instruction
{
public:
    DropInstruction()
        : Instruction(Type::DROP)
    {}

    virtual const std::string ToString() override { return "drop"; }

    static const Type Type = Type::DROP;
};

class ReturnArrayInstruction : public Instruction
{
public:
    ReturnArrayInstruction()
        : Instruction(Type::RETURN)
    {}

    virtual const std::string ToString() override { return "reta"; }

    static const Type Type = Type::RETURN;
};

class DuplicateArrayInstruction : public Instruction
{
public:
    DuplicateArrayInstruction()
        : Instruction(Type::DUP_ARRAY)
    {}

    virtual const std::string ToString() override { return "dupa"; }

    static const Type Type = Type::DUP_ARRAY;
};

class StoreArrayInstruction : public Instruction
{
public:
    StoreArrayInstruction()
        : Instruction(Type::STORE_ARRAY)
    {}

    virtual const std::string ToString() override { return "sta"; }

    static const Type Type = Type::STORE_ARRAY;
};

