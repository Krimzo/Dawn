#include "instruction.h"
#include "stack.h"
#include "err.h"


std::ostream& dawn::operator<<( std::ostream& stream, InstructionType const& inst_type )
{
    switch ( inst_type )
    {
    case InstructionType::NONE:
        stream << "NONE";
        break;

    case InstructionType::JUMP:
        stream << "JUMP";
        break;

    case InstructionType::JUMPIFN:
        stream << "JUMPIFN";
        break;

    case InstructionType::STORE:
        stream << "STORE";
        break;

    case InstructionType::PUSHS:
        stream << "PUSHS";
        break;

    case InstructionType::POPS:
        stream << "POPS";
        break;

    case InstructionType::PUSHV:
        stream << "PUSHV";
        break;

    case InstructionType::FINDV:
        stream << "FINDV";
        break;

    case InstructionType::PUSHC:
        stream << "PUSHC";
        break;

    case InstructionType::CALL:
        stream << "CALL";
        break;

    case InstructionType::RET:
        stream << "RET";
        break;

    case InstructionType::BRK:
        stream << "BRK";
        break;

    case InstructionType::CNT:
        stream << "CNT";
        break;

    case InstructionType::OP:
        stream << "OP";
        break;

    default:
        PANIC( "invalid instruction type ", (Int) inst_type );
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, Instruction const& inst )
{
    stream << inst.type << " " << inst.value;
    return stream;
}
