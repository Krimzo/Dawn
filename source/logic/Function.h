#pragma once

#include "logic/Instruction.h"


namespace dawn {
    struct Function : public Array<Instruction>
    {
        void Execute();
    };
}
