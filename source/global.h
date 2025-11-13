#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
using MemberGenerator = Func<Value( Location const&, Engine&, Value const& )>;
using MemberFunc = Func<Value( Location const&, Engine&, Value const& )>;
using MethodFunc = Func<Value( Location const&, Engine&, Value const&, Value const*, Int )>;

struct Global
{
    GlobalStorage<Enum> enums;
    GlobalStorage<Struct> structs;
    GlobalStorage<Value> values;

    GlobalStorage<GlobalStorage<FunctionValue>> operators[(Int) OperatorType::_COUNT] = {};
    GlobalStorage<MemberGenerator> member_generators[(Int) ValueType::_COUNT] = {};

    Set<uint64_t> ctime_ops[(Int) OperatorType::_COUNT] = {};
    Set<ID> ctime_funcs;

    Global();

    void load_mod( Engine& engine, Module const& module );
    void load_operator( Operator const& entry );
    void load_function( Function const& entry );
    void load_enum( Engine& engine, Enum const& entry );
    void load_struct( Struct const& entry );
    void load_variable( Engine& engine, Variable const& entry );

    void bind_oper( ID left_type_id, OperatorType op_type, ID right_type_id, Bool is_const, CFunction cfunc );

    void bind_func( ID id, Bool is_ctime, CFunction cfunc );
    Value call_func( Engine& engine, ID id, Value* args, Int arg_count );

    void bind_member( ValueType type, StringRef const& name, MemberFunc const& func );
    void bind_method( ValueType type, String const& name, Bool is_const, MethodFunc const& body );

private:
    void load_standard_operators();
    void load_standard_functions();
    void load_standard_members();
};
}
