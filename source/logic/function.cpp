#include "logic/function.h"


faxdawn::function::function()
{}

void faxdawn::function::execute() const
{
    for (auto& instr : instructions) {
        instr.execute();
    }
}
