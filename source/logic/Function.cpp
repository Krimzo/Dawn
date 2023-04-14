#include "logic/Function.h"


void dawn::Function::Execute()
{
    for (auto& instruction : *this) {
        instruction.Execute();
    }
}
