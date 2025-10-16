#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
struct Engine
{
    using MemberGenerator = Func<Value( Location const&, Engine&, Value const& )>;
    using CustomMemberFunc = Func<Value( Location const&, Engine&, Value const& )>;
    using CustomMethodFunc = Func<Value( Location const&, Engine&, Value const&, Value const* )>;

    friend struct Value;
    friend struct EnumValue;
    friend struct Optimizer;
    friend Value create_default_value( Engine* engine, ID typeid_, Location const& location );

    Stack stack;
    GlobalStorage<Enum> enums;
    GlobalStorage<Struct> structs;
    GlobalStorage<GlobalStorage<FunctionValue>> operators[(Int) OperatorType::_COUNT] = {};
    GlobalStorage<MemberGenerator> member_generators[(Int) ValueType::_COUNT] = {};

    Engine();

    void load_mod( Module const& module );
    void load_operator( Operator const& entry );
    void load_function( Function const& entry );
    void load_enum( Enum const& entry );
    void load_struct( Struct const& entry );
    void load_variable( Variable const& entry );

    void bind_oper( ID left_type_id, OperatorType op_type, ID right_type_id, Bool is_const, CFunction cfunc );

    void bind_func( ID id, Bool is_ctime, CFunction cfunc );
    Value call_func( ID id, Value* args, Int arg_count );

    void add_var( Location const& location, VarType const& type, ID id, Value const& value );
    Value* get_var( ID id );

    void bind_member( ValueType type, StringRef const& name, CustomMemberFunc const& func );
    void bind_method( ValueType type, String const& name, Bool is_const, Int expected_args, CustomMethodFunc const& body );

private:
    Set<uint64_t> m_ctime_ops[(Int) OperatorType::_COUNT] = {};
    Set<ID> m_ctime_funcs;

    void load_standard_operators();
    void load_standard_functions();
    void load_standard_members();

    void handle_var_node( VariableNode const& node );
    Value const& handle_id_node( IdentifierNode const& node );
    Value handle_call_node( CallNode const& node );
    Value handle_index_node( IndexNode const& node );
    void handle_return_node( ReturnNode const& node, Opt<Value>& retval );
    void handle_break_node( BreakNode const& node, Bool* didbrk );
    void handle_continue_node( ContinueNode const& node, Bool* didcon );
    void handle_throw_node( ThrowNode const& node );
    void handle_try_node( TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_if_node( IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNode const& node, Opt<Value>& retval );
    void handle_while_node( WhileNode const& node, Opt<Value>& retval );
    void handle_for_node( ForNode const& node, Opt<Value>& retval );
    Value const& handle_lambda_node( LambdaNode const& node );
    Value handle_enum_node( EnumNode const& node );
    Value handle_struct_node( StructNode const& node );
    Value handle_array_node( ArrayNode const& node );
    Value handle_ac_node( AccessNode const& node );
    Value handle_op_node( OperatorNode const& node );
    Value handle_as_node( AssignNode const& node );

    void handle_scope( Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon ); // Should not inline since scope calls instr and instr calls scope.

    __forceinline Value handle_oper( Location const& location, Value const& left, const OperatorType op_type, Value const& right )
    {
        auto& op_left_ids = operators[(Int) op_type];
        auto* op_right_ids = op_left_ids.get( left.type_id() );
        if ( !op_right_ids )
            ENGINE_PANIC( location, "type [", IDSystem::get( left.type_id() ), "] does not support operator [", op_type, "]" );
        auto* func = op_right_ids->get( right.type_id() );
        if ( !func )
            ENGINE_PANIC( location, "type [", IDSystem::get( left.type_id() ), "] does not support operator [", op_type, "] with right type being [", IDSystem::get( right.type_id() ), "]" );

        using ProxyArg = uint64_t;
        static_assert( sizeof( ProxyArg ) == sizeof( Value ), "ProxyArg size must be the same as Value" );
        static_assert( alignof( ProxyArg ) == alignof( Value ), "ProxyArg alignment must be the same as Value" );
        ProxyArg proxy_args[2] = { reinterpret_cast<ProxyArg const&>( left ), reinterpret_cast<ProxyArg const&>( right ) }; // Improves performance by not calling the constructors or destructors of Value.

        return handle_func( location, *func, reinterpret_cast<Value*>( proxy_args ), (Int) std::size( proxy_args ) );
    }

    __forceinline Value handle_func( Location const& location, FunctionValue const& func, Value const* args, Int arg_count )
    {
        if ( auto* dfunc = func.dfunction() )
        {
            if ( dfunc->args.size() != arg_count )
            {
                if ( func.is_global() )
                    ENGINE_PANIC( location, "invalid argument count for function [", IDSystem::get( func.as_global().id ), "]" );
                else if ( func.is_method() )
                    ENGINE_PANIC( location, "invalid argument count for method [", IDSystem::get( func.as_method().id ), "]" );
                else
                    ENGINE_PANIC( location, "invalid argument count for lambda" );
            }

            auto pop_handler = stack.push_from(
                func.is_lambda() ? func.as_lambda().frame : RegisterRef<Frame>{} );

            for ( Int i = 0; i < arg_count; i++ )
                add_var( location, dfunc->args[i].type, dfunc->args[i].id, args[i] );

            Opt<Value> retval;
            handle_scope( dfunc->body, retval, nullptr, nullptr );
            return retval ? *retval : Value{};
        }
        else
        {
            auto& cfunc = *func.cfunction();
            return cfunc( location, *this, args, arg_count );
        }
    }

    __forceinline void handle_instr( Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
    {
        switch ( node.type() )
        {
        case NodeType::SCOPE:
        {
            const auto pop_handler = stack.push();
            handle_scope( std::get<Scope>( node ), retval, didbrk, didcon );
        }
        break;

        case NodeType::VARIABLE:
            handle_var_node( std::get<VariableNode>( node ) );
            break;

        case NodeType::RETURN:
            handle_return_node( std::get<ReturnNode>( node ), retval );
            break;

        case NodeType::BREAK:
            handle_break_node( std::get<BreakNode>( node ), didbrk );
            break;

        case NodeType::CONTINUE:
            handle_continue_node( std::get<ContinueNode>( node ), didcon );
            break;

        case NodeType::THROW:
            handle_throw_node( std::get<ThrowNode>( node ) );
            break;

        case NodeType::TRY:
            handle_try_node( std::get<TryNode>( node ), retval, didbrk, didcon );
            break;

        case NodeType::IF:
            handle_if_node( std::get<IfNode>( node ), retval, didbrk, didcon );
            break;

        case NodeType::SWITCH:
            handle_switch_node( std::get<SwitchNode>( node ), retval, didbrk, didcon );
            break;

        case NodeType::LOOP:
            handle_loop_node( std::get<LoopNode>( node ), retval );
            break;

        case NodeType::WHILE:
            handle_while_node( std::get<WhileNode>( node ), retval );
            break;

        case NodeType::FOR:
            handle_for_node( std::get<ForNode>( node ), retval );
            break;

        default:
            handle_expr( node );
        }
    }

    __forceinline Value handle_expr( Node const& node )
    {
        switch ( node.type() )
        {
        case NodeType::VALUE:
            return std::get<Value>( node );

        case NodeType::IDENTIFIER:
            return handle_id_node( std::get<IdentifierNode>( node ) );

        case NodeType::CALL:
            return handle_call_node( std::get<CallNode>( node ) );

        case NodeType::INDEX:
            return handle_index_node( std::get<IndexNode>( node ) );

        case NodeType::LAMBDA:
            return handle_lambda_node( std::get<LambdaNode>( node ) );

        case NodeType::ENUM:
            return handle_enum_node( std::get<EnumNode>( node ) );

        case NodeType::STRUCT:
            return handle_struct_node( std::get<StructNode>( node ) );

        case NodeType::ARRAY:
            return handle_array_node( std::get<ArrayNode>( node ) );

        case NodeType::ACCESS:
            return handle_ac_node( std::get<AccessNode>( node ) );

        case NodeType::OPERATOR:
            return handle_op_node( std::get<OperatorNode>( node ) );

        case NodeType::ASSIGN:
            return handle_as_node( std::get<AssignNode>( node ) );

        default:
            ENGINE_PANIC( node.location(), "unknown expr node type: ", (Int) node.type() );
        }
    }
};

__forceinline Value create_default_value( Engine* engine, ID typeid_, Location const& location )
{
    static const ID _id_void = IDSystem::get( tp_void );
    static const ID _id_bool = IDSystem::get( tp_bool );
    static const ID _id_int = IDSystem::get( tp_int );
    static const ID _id_float = IDSystem::get( tp_float );
    static const ID _id_char = IDSystem::get( tp_char );
    static const ID _id_string = IDSystem::get( tp_string );
    static const ID _id_range = IDSystem::get( tp_range );
    static const ID _id_function = IDSystem::get( tp_function );
    static const ID _id_array = IDSystem::get( tp_array );

    if ( typeid_ == _id_void )
        return Value{};

    else if ( typeid_ == _id_bool )
        return Value{ Bool{}, location };

    else if ( typeid_ == _id_int )
        return Value{ Int{}, location };

    else if ( typeid_ == _id_float )
        return Value{ Float{}, location };

    else if ( typeid_ == _id_char )
        return Value{ Char{}, location };

    else if ( typeid_ == _id_string )
        return Value{ StringRef{}, location };

    else if ( typeid_ == _id_range )
        return Value{ RangeValue{}, location };

    else if ( typeid_ == _id_function )
        return Value{ FunctionValue{}, location };

    else if ( typeid_ == _id_array )
        return Value{ ArrayValue{}, location };

    else if ( auto* enum_ptr = engine->enums.get( typeid_ ) )
    {
        auto& entry = *enum_ptr->entries.begin();
        EnumNode node{ location };
        node.type_id = typeid_;
        node.key_id = entry.id;
        return engine->handle_enum_node( node );
    }

    else if ( auto* struct_ptr = engine->structs.get( typeid_ ) )
    {
        StructNode node{ location };
        node.type_id = typeid_;
        return engine->handle_struct_node( node );
    }

    else
        ENGINE_PANIC( location, "type [", IDSystem::get( typeid_ ), "] does not exist" );
}
}
