#include "nodes.h"


void dawn::NodeHandler::copy( NodeType type, void*& to, void const* from )
{
    switch ( type )
    {
    case NodeType::VALUE:
        to = new ValueNod( *static_cast<ValueNod const*>(from) );
        break;

    case NodeType::BOX:
        to = new BoxNod( *static_cast<BoxNod const*>(from) );
        break;

    case NodeType::CAST:
        to = new CastNod( *static_cast<CastNod const*>(from) );
        break;

    case NodeType::VARIABLE:
        to = new VariableNod( *static_cast<VariableNod const*>(from) );
        break;

    case NodeType::IDENTIFIER:
        to = new IdentifierNod( *static_cast<IdentifierNod const*>(from) );
        break;

    case NodeType::FUNCTION:
        to = new FunctionNod( *static_cast<FunctionNod const*>(from) );
        break;

    case NodeType::RETURN:
        to = new ReturnNod( *static_cast<ReturnNod const*>(from) );
        break;

    case NodeType::BREAK:
        to = new BreakNod( *static_cast<BreakNod const*>(from) );
        break;

    case NodeType::CONTINUE:
        to = new ContinueNod( *static_cast<ContinueNod const*>(from) );
        break;

    case NodeType::IF:
        to = new IfNod( *static_cast<IfNod const*>(from) );
        break;

    case NodeType::SWITCH:
        to = new SwitchNod( *static_cast<SwitchNod const*>(from) );
        break;

    case NodeType::LOOP:
        to = new LoopNod( *static_cast<LoopNod const*>(from) );
        break;

    case NodeType::WHILE:
        to = new WhileNod( *static_cast<WhileNod const*>(from) );
        break;

    case NodeType::FOR:
        to = new ForNod( *static_cast<ForNod const*>(from) );
        break;

    case NodeType::ENUM:
        to = new EnumNod( *static_cast<EnumNod const*>(from) );
        break;

    case NodeType::STRUCT:
        to = new StructNod( *static_cast<StructNod const*>(from) );
        break;

    case NodeType::ARRAY:
        to = new ArrayNod( *static_cast<ArrayNod const*>(from) );
        break;

    case NodeType::UNARY:
        to = new UnaryNod( *static_cast<UnaryNod const*>(from) );
        break;

    case NodeType::OPERATOR:
        to = new OperatorNod( *static_cast<OperatorNod const*>(from) );
        break;

    case NodeType::ASSIGN:
        to = new AssignNod( *static_cast<AssignNod const*>(from) );
        break;

    case NodeType::SCOPE:
        to = new Scope( *static_cast<Scope const*>(from) );
        break;
    }
}

void dawn::NodeHandler::destruct( NodeType type, void* ptr )
{
    switch ( type )
    {
    case NodeType::VALUE:
        delete static_cast<ValueNod*>(ptr);
        break;

    case NodeType::BOX:
        delete static_cast<BoxNod*>(ptr);
        break;

    case NodeType::CAST:
        delete static_cast<CastNod*>(ptr);
        break;

    case NodeType::VARIABLE:
        delete static_cast<VariableNod*>(ptr);
        break;

    case NodeType::IDENTIFIER:
        delete static_cast<IdentifierNod*>(ptr);
        break;

    case NodeType::FUNCTION:
        delete static_cast<FunctionNod*>(ptr);
        break;

    case NodeType::RETURN:
        delete static_cast<ReturnNod*>(ptr);
        break;

    case NodeType::BREAK:
        delete static_cast<BreakNod*>(ptr);
        break;

    case NodeType::CONTINUE:
        delete static_cast<ContinueNod*>(ptr);
        break;

    case NodeType::IF:
        delete static_cast<IfNod*>(ptr);
        break;

    case NodeType::SWITCH:
        delete static_cast<SwitchNod*>(ptr);
        break;

    case NodeType::LOOP:
        delete static_cast<LoopNod*>(ptr);
        break;

    case NodeType::WHILE:
        delete static_cast<WhileNod*>(ptr);
        break;

    case NodeType::FOR:
        delete static_cast<ForNod*>(ptr);
        break;

    case NodeType::ENUM:
        delete static_cast<EnumNod*>(ptr);
        break;

    case NodeType::STRUCT:
        delete static_cast<StructNod*>(ptr);
        break;

    case NodeType::ARRAY:
        delete static_cast<ArrayNod*>(ptr);
        break;

    case NodeType::UNARY:
        delete static_cast<UnaryNod*>(ptr);
        break;

    case NodeType::OPERATOR:
        delete static_cast<OperatorNod*>(ptr);
        break;

    case NodeType::ASSIGN:
        delete static_cast<AssignNod*>(ptr);
        break;

    case NodeType::SCOPE:
        delete static_cast<Scope*>(ptr);
        break;
    }
}
