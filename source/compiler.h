#pragma once

#include "parser.h"
#include "btype.h"
#include "instruction.h"


namespace dawn
{
struct Binary
{
    Set<String> imports;
    Vector<BVariable> variables;
    Vector<BFunction> functions;
    Vector<BEnum> enums;
    Vector<BStruct> structs;
};

struct Compiler
{
    void compile( Module const& module, Binary& binary );
    void compile_function( Function const& func, BFunction& bfunc );
    void compile_variable( Variable const& var, BVariable& bvar );
    void compile_struct( Struct const& struc, BStruct& bstruc );
    void compile_enum( Enum const& enu, BEnum& benu );
    void compile_node( Node const& node, Vector<Instruction>& instrs );
    void compile_scope( Scope const& scope, Vector<Instruction>& instrs );
    void compile_var( VarNod const& nod, Vector<Instruction>& instrs );
    void compile_func( FuncNod const& nod, Vector<Instruction>& instrs );
    void compile_return( ReturnNod const& nod, Vector<Instruction>& instrs );
    void compile_break( BreakNod const& nod, Vector<Instruction>& instrs );
    void compile_continue( ContinueNod const& nod, Vector<Instruction>& instrs );
    void compile_throw( ThrowNod const& nod, Vector<Instruction>& instrs );
    void compile_try( TryNod const& nod, Vector<Instruction>& instrs );
    void compile_if( IfNod const& nod, Vector<Instruction>& instrs );
    void compile_switch( SwitchNod const& nod, Vector<Instruction>& instrs );
    void compile_loop( LoopNod const& nod, Vector<Instruction>& instrs );
    void compile_while( WhileNod const& nod, Vector<Instruction>& instrs );
    void compile_for( ForNod const& nod, Vector<Instruction>& instrs );
    void compile_ref( RefNod const& nod, Vector<Instruction>& instrs );
    void compile_identifier( IdentifierNod const& nod, Vector<Instruction>& instrs );
    void compile_call( CallNod const& nod, Vector<Instruction>& instrs );
    void compile_index( IndexNod const& nod, Vector<Instruction>& instrs );
    void compile_enu( EnumNod const& nod, Vector<Instruction>& instrs );
    void compile_struc( StructNod const& nod, Vector<Instruction>& instrs );
    void compile_array( ArrayNod const& nod, Vector<Instruction>& instrs );
    void compile_unary( UnaryNod const& nod, Vector<Instruction>& instrs );
    void compile_operator( OperatorNod const& nod, Vector<Instruction>& instrs );
    void compile_assign( AssignNod const& nod, Vector<Instruction>& instrs );
};
}
