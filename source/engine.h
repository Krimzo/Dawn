#pragma once

#include "parser.h"


namespace dawn
{
struct EngineError
{
    String msg;

    template<typename... Args>
    EngineError( Args const&... args )
    {
        StringStream stream;
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, EngineError const& error );

struct EngineVariable
{
    virtual ~EngineVariable() = default;

    virtual Ref<Value> const& get_value() const = 0;
    virtual Opt<EngineError> set_value( Ref<Value> const& value ) = 0;

protected:
    EngineVariable() = default;
};

struct EngineVariableLet : EngineVariable
{
    EngineVariableLet( Ref<Value> const& value );

    Ref<Value> const& get_value() const override;
    Opt<EngineError> set_value( Ref<Value> const& value ) override;

private:
    Ref<Value> m_value;
};

struct EngineVariableVar : EngineVariable
{
    EngineVariableVar( Ref<Value> const& value );

    Ref<Value> const& get_value() const override;
    Opt<EngineError> set_value( Ref<Value> const& value ) override;

private:
    Ref<Value> m_value;
};

struct EngineVariableRef : EngineVariable
{
    EngineVariableRef( Ref<EngineVariable> const& ref_var );

    Ref<Value> const& get_value() const override;
    Opt<EngineError> set_value( Ref<Value> const& value ) override;

private:
    Ref<EngineVariable> m_ref_var;
};

template<typename T>
struct Stack
{
    void push( StringRef const& name, T const& val )
    {
        m_data.emplace_back( name, val );
    }

    void pop( Int n = 1 )
    {
        m_data.resize( m_data.size() - n );
    }

    T const& peek() const
    {
        return m_data.back().second;
    }

    T* get( StringRef const& name )
    {
        for ( auto it = m_data.rbegin(); it != m_data.rend(); ++it )
        {
            if ( it->first == name )
                return &it->second;
        }
        return nullptr;
    }

private:
    List<Pair<String, T>> m_data;
};

struct Engine
{
    Stack<Ref<EngineVariable>> variables;
    Stack<Function> functions;
    Stack<Enum> enums;
    Stack<Struct> structs;

    void load_default_mods();
    Opt<EngineError> load_mod( Module const& module );

    void bind_func( String const& name, Function::CppFunc cpp_func );
    Opt<EngineError> call_func( String const& name, Array<Ref<Node>> const& args, Ref<Value>& retval );

    void add_var( String const& name, Bool is_var, Ref<Value> const& value );
    Opt<EngineError> add_var( Variable const& var );
    EngineVariable* get_var( String const& name );

private:
    Opt<EngineError> handle_func( Function const& func, Array<Ref<Node>> const& args, Ref<Value>& retval );
    Opt<EngineError> handle_scope( Scope const& scope, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_instr( Ref<Node> const& node, Ref<Value>& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_expr( Ref<Node> const& node, Ref<Value>& value );

    Opt<EngineError> handle_nothing_node( NothingNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_val_node( ValueNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_array_node( ArrayNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_struct_node( StructNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_cast_node( CastNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_var_node( VariableNode const& node, Int& push_count );
    Opt<EngineError> handle_id_node( IdentifierNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_func_node( FunctionNode const& node, Ref<Value>& retval );
    Opt<EngineError> handle_return_node( ReturnNode const& node, Ref<Value>& retval, Bool& didret );
    Opt<EngineError> handle_break_node( BreakNode const& node, Bool* didbrk );
    Opt<EngineError> handle_continue_node( ContinueNode const& node, Bool* didcon );
    Opt<EngineError> handle_if_node( IfNode const& node, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_switch_node( SwitchNode const& node, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_loop_node( LoopNode const& node, Ref<Value>& retval, Bool& didret );
    Opt<EngineError> handle_while_node( WhileNode const& node, Ref<Value>& retval, Bool& didret );
    Opt<EngineError> handle_for_node( ForNode const& node, Ref<Value>& retval, Bool& didret );
    Opt<EngineError> handle_un_node( UnaryNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_op_node( OperatorNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_ac_node( OperatorNodeAccess const& node, Ref<Value>& value );
    Opt<EngineError> handle_as_node( AssignNode const& node, Ref<Value>& value );
};
}
