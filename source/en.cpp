#include "en.h"
#include "syntax.h"


std::ostream& dawn::operator<<( std::ostream& stream, TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER: stream << "Integer"; break;
    case TokenType::FLOAT: stream << "Float"; break;
    case TokenType::CHAR: stream << "Char"; break;
    case TokenType::STRING: stream << "String"; break;
    case TokenType::KEYWORD: stream << "Keyword"; break;
    case TokenType::TYPE: stream << "Type"; break;
    case TokenType::NAME: stream << "Name"; break;
    case TokenType::OPERATOR: stream << "Operator"; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, OperatorType type )
{
    switch ( type )
    {
    case OperatorType::ADD: stream << "Add"; break;
    case OperatorType::SUB: stream << "Sub"; break;
    case OperatorType::MUL: stream << "Mul"; break;
    case OperatorType::DIV: stream << "Div"; break;
    case OperatorType::POW: stream << "Pow"; break;
    case OperatorType::MOD: stream << "Mod"; break;
    case OperatorType::EQ: stream << "Eq"; break;
    case OperatorType::NOT_EQ: stream << "Not_Eq"; break;
    case OperatorType::LESS: stream << "Less"; break;
    case OperatorType::GREAT: stream << "Great"; break;
    case OperatorType::LESS_EQ: stream << "Less_Eq"; break;
    case OperatorType::GREAT_EQ: stream << "Great_Eq"; break;
    case OperatorType::NOT: stream << "Not"; break;
    case OperatorType::AND: stream << "And"; break;
    case OperatorType::OR: stream << "Or"; break;
    case OperatorType::RANGE: stream << "Range"; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, ValueType type )
{
    switch ( type )
    {
    case ValueType::VOID: stream << tp_void; break;
    case ValueType::BOOL: stream << tp_bool; break;
    case ValueType::INT: stream << tp_int; break;
    case ValueType::FLOAT: stream << tp_float; break;
    case ValueType::CHAR: stream << tp_char; break;
    case ValueType::STRING: stream << tp_string; break;
    case ValueType::RANGE: stream << tp_range; break;
    case ValueType::FUNCTION: stream << tp_function; break;
    case ValueType::ARRAY: stream << tp_array; break;
    case ValueType::ENUM: stream << kw_enum; break;
    case ValueType::STRUCT: stream << kw_struct; break;
    }
    return stream;
}
