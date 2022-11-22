#include "logic/function.h"


void faxdawn::function::execute() const
{
    for (auto& instr : instructions) {
        instr.execute();
    }
}
