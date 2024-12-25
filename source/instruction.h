#pragma once

#include "t.h"
#include "value.h"


namespace dawn
{
enum struct InstructionType
{
    NONE = 0,
    JUMP,
    JUMPIFN,
    STORE,
    PUSHS,
    POPS,
    PUSHV,
    FINDV,
    PUSHC,
    CALL,
    RET,
    BRK,
    CNT,
    OP,
};

struct Instruction
{
    InstructionType type = {};
    union
    {
        int64_t value = 0;
        struct
        {
            int32_t val0;
            int32_t val1;
        };
    };
    ValueRef value_ref;

    Instruction()
    {}

    Instruction( InstructionType type, int64_t value )
        : type( type ), value( value )
    {}

    Instruction( InstructionType type, int32_t val0, int32_t val1 )
        : type( type ), val0( val0 ), val1( val1 )
    {}

    Instruction( InstructionType type, ValueRef const& value_ref )
        : type( type ), value_ref( value_ref )
    {}
};

std::ostream& operator<<( std::ostream& stream, InstructionType const& inst_type );
std::ostream& operator<<( std::ostream& stream, Instruction const& inst );
}
