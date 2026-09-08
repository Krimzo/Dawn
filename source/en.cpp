#include "en.h"
#include "syntax.h"

std::ostream& dawn::operator<<(std::ostream& stream, TokenType type)
{
    switch (type)
    {
    case TokenType::INTEGER:
        stream << "Integer";
        break;
    case TokenType::FLOAT:
        stream << "Float";
        break;
    case TokenType::CHAR:
        stream << "Char";
        break;
    case TokenType::STRING:
        stream << "String";
        break;
    case TokenType::KEYWORD:
        stream << "Keyword";
        break;
    case TokenType::TYPE:
        stream << "Type";
        break;
    case TokenType::NAME:
        stream << "Name";
        break;
    case TokenType::OPERATOR:
        stream << "Operator";
        break;
    }
    return stream;
}

std::ostream& dawn::operator<<(std::ostream& stream, OperatorType type)
{
    switch (type)
    {
    case OperatorType::ADD:
        stream << "Add";
        break;
    case OperatorType::SUB:
        stream << "Sub";
        break;
    case OperatorType::MUL:
        stream << "Mul";
        break;
    case OperatorType::DIV:
        stream << "Div";
        break;
    case OperatorType::POW:
        stream << "Pow";
        break;
    case OperatorType::MOD:
        stream << "Mod";
        break;
    case OperatorType::EQ:
        stream << "Eq";
        break;
    case OperatorType::NOT_EQ:
        stream << "Not_Eq";
        break;
    case OperatorType::LESS:
        stream << "Less";
        break;
    case OperatorType::GREAT:
        stream << "Great";
        break;
    case OperatorType::LESS_EQ:
        stream << "Less_Eq";
        break;
    case OperatorType::GREAT_EQ:
        stream << "Great_Eq";
        break;
    case OperatorType::NOT:
        stream << "Not";
        break;
    case OperatorType::AND:
        stream << "And";
        break;
    case OperatorType::OR:
        stream << "Or";
        break;
    case OperatorType::RANGE:
        stream << "Range";
        break;
    }
    return stream;
}

std::ostream& dawn::operator<<(std::ostream& stream, NodeType type)
{
    switch (type)
    {
    case NodeType::NONE:
        stream << "None";
        break;
    case NodeType::SCOPE:
        stream << "Scope";
        break;
    case NodeType::VARIABLE:
        stream << "Variable";
        break;
    case NodeType::RETURN:
        stream << "Return";
        break;
    case NodeType::BREAK:
        stream << "Break";
        break;
    case NodeType::CONTINUE:
        stream << "Continue";
        break;
    case NodeType::THROW:
        stream << "Throw";
        break;
    case NodeType::TRY:
        stream << "Try";
        break;
    case NodeType::IF:
        stream << "If";
        break;
    case NodeType::SWITCH:
        stream << "Switch";
        break;
    case NodeType::LOOP:
        stream << "Loop";
        break;
    case NodeType::WHILE:
        stream << "While";
        break;
    case NodeType::FOR:
        stream << "For";
        break;
    case NodeType::VALUE:
        stream << "Value";
        break;
    case NodeType::IDENTIFIER:
        stream << "Identifier";
        break;
    case NodeType::CALL:
        stream << "Call";
        break;
    case NodeType::INDEX:
        stream << "Index";
        break;
    case NodeType::LAMBDA:
        stream << "Lambda";
        break;
    case NodeType::ENUM:
        stream << "Enum";
        break;
    case NodeType::STRUCT:
        stream << "Struct";
        break;
    case NodeType::ARRAY:
        stream << "Array";
        break;
    case NodeType::ACCESS:
        stream << "Access";
        break;
    case NodeType::OPERATOR:
        stream << "Operator";
        break;
    case NodeType::ASSIGN:
        stream << "Assign";
        break;
    case NodeType::CAST:
        stream << "Cast";
        break;
    }
    return stream;
}
