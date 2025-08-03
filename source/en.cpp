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

std::ostream& dawn::operator<<( std::ostream& stream, VariableKind kind )
{
    switch ( kind )
    {
    case VariableKind::CONST: stream << kw_const; break;
    case VariableKind::VAR: stream << kw_var; break;
    case VariableKind::REF: stream << kw_ref; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, ValueType type )
{
    switch ( type )
    {
    case ValueType::NOTHING: stream << tp_nothing; break;
    case ValueType::BOOL: stream << tp_bool; break;
    case ValueType::INT: stream << tp_int; break;
    case ValueType::FLOAT: stream << tp_float; break;
    case ValueType::CHAR: stream << tp_char; break;
    case ValueType::STRING: stream << tp_string; break;
    case ValueType::FUNCTION: stream << tp_function; break;
    case ValueType::ENUM: stream << tp_enum; break;
    case ValueType::STRUCT: stream << tp_struct; break;
    case ValueType::ARRAY: stream << tp_array; break;
    case ValueType::RANGE: stream << tp_range; break;
    }
    return stream;
}
