#include "logic/instruction.h"


faxdawn::instruction::instruction()
    : type(instruction_type::none)
{}

faxdawn::instruction::instruction(const instruction_type type, void* first, void* second)
    : type(type), first(first), second(second)
{}

void faxdawn::instruction::execute() noexcept
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
        break;
    }
}

void faxdawn::instruction::move_to() noexcept
{
    
}

void faxdawn::instruction::set_memory() noexcept
{
    
}

void faxdawn::instruction::read_memory() noexcept
{
    
}

void faxdawn::instruction::call_function() noexcept
{
    
}
