#pragma once

#include "dawn/Syntax.h"


namespace dawn {
    enum class InstructionType
    {
        None = 0,
        MoveTo,
        SetMemory,
        ReadMemory,
        CallFunction,
    };
}

namespace dawn {
    class Instruction
    {
        void MoveTo();
        void SetMemory();
        void ReadMemory();
        void CallFunction();

    public:
        InstructionType type = InstructionType::None;
        void* addressZero = nullptr;
        void* addressOne = nullptr;

        void Execute();
    };
}
