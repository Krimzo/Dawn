#include "logic/Instruction.h"


void dawn::Instruction::Execute()
{
    switch (type) {
    case InstructionType::MoveTo:
        MoveTo();
        break;

    case InstructionType::SetMemory:
        SetMemory();
        break;

    case InstructionType::ReadMemory:
        ReadMemory();
        break;

    case InstructionType::CallFunction:
        CallFunction();
        break;
    }
}

void dawn::Instruction::MoveTo()
{}

void dawn::Instruction::SetMemory()
{}

void dawn::Instruction::ReadMemory()
{}

void dawn::Instruction::CallFunction()
{}
