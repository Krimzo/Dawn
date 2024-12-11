#include "node.h"
#include "nodes.h"


dawn::NodeType dawn::Node::type() const
{
    if ( std::any::type() == typeid(ValueNod) )
        return NodeType::VALUE;

    else if ( std::any::type() == typeid(BoxNod) )
        return NodeType::BOX;

    else if ( std::any::type() == typeid(CastNod) )
        return NodeType::CAST;

    else if ( std::any::type() == typeid(VariableNod) )
        return NodeType::VARIABLE;

    else if ( std::any::type() == typeid(IdentifierNod) )
        return NodeType::IDENTIFIER;

    else if ( std::any::type() == typeid(FunctionNod) )
        return NodeType::FUNCTION;

    else if ( std::any::type() == typeid(ReturnNod) )
        return NodeType::RETURN;

    else if ( std::any::type() == typeid(BreakNod) )
        return NodeType::BREAK;

    else if ( std::any::type() == typeid(ContinueNod) )
        return NodeType::CONTINUE;

    else if ( std::any::type() == typeid(IfNod) )
        return NodeType::IF;

    else if ( std::any::type() == typeid(SwitchNod) )
        return NodeType::SWITCH;

    else if ( std::any::type() == typeid(LoopNod) )
        return NodeType::LOOP;

    else if ( std::any::type() == typeid(WhileNod) )
        return NodeType::WHILE;

    else if ( std::any::type() == typeid(ForNod) )
        return NodeType::FOR;

    else if ( std::any::type() == typeid(EnumNod) )
        return NodeType::ENUM;

    else if ( std::any::type() == typeid(StructNod) )
        return NodeType::STRUCT;

    else if ( std::any::type() == typeid(ArrayNod) )
        return NodeType::ARRAY;

    else if ( std::any::type() == typeid(UnaryNod) )
        return NodeType::UNARY;

    else if ( std::any::type() == typeid(OperatorNod) )
        return NodeType::OPERATOR;

    else if ( std::any::type() == typeid(AssignNod) )
        return NodeType::ASSIGN;

    else if ( std::any::type() == typeid(Scope) )
        return NodeType::SCOPE;

    else
        return NodeType::EMPTY;
}
