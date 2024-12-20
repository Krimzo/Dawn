#include "memory.h"
#include "values.h"
#include "stack.h"


dawn::GlobalMemory& dawn::get_global_memory()
{
    static GlobalMemory instance;
    return instance;
}
