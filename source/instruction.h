#pragma once

#include "node.h"


namespace dawn
{
enum struct InstructionType
{
    NONE = 0,
};

struct Instruction
{
    InstructionType type;
};
}
