#include "logic/instruction.h"


faxdawn::instruction::instruction(const instruction_type type, void* first, void* second)
    : type(type), first(first), second(second)
{}

void faxdawn::instruction::execute() const
{
    switch (type) {
    case instruction_type::move_to:
        move_to();
        break;
    case instruction_type::set_memory:
        set_memory();
        break;
    case instruction_type::read_memory:
        read_memory();
        break;
    case instruction_type::call_function:
        call_function();
        break;

    case instruction_type::none:
    default:
        return;
    }
}

void faxdawn::instruction::move_to() const
{
    
}

void faxdawn::instruction::set_memory() const
{
    
}

void faxdawn::instruction::read_memory() const
{
    
}

void faxdawn::instruction::call_function() const
{
    
}
