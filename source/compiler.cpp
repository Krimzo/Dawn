#include "compiler.h"


void dawn::Compiler::compile( Module const& module, Binary& binary )
{
    binary.imports = module.imports;

    for ( auto& entry : module.functions )
        compile_function( entry, binary.functions.emplace_back() );

    for ( auto& entry : module.variables )
        compile_variable( entry, binary.variables.emplace_back() );

    for ( auto& entry : module.structs )
        compile_struct( entry, binary.structs.emplace_back() );

    for ( auto& entry : module.enums )
        compile_enum( entry, binary.enums.emplace_back() );
}

void dawn::Compiler::compile_function( Function const& func, BFunction& bfunc )
{
    bfunc.id = func.id;

    for ( auto& arg : func.args )
    {
        auto& barg = bfunc.args.emplace_back();
        barg.kind = arg.kind;
        barg.id = arg.id;
    }

    for ( auto& node : func.body.instr )
        compile_node( node, *std::get_if<BFunction::DawnFunc>( &bfunc.body ) );
}

void dawn::Compiler::compile_variable( Variable const& var, BVariable& bvar )
{
    bvar.id = var.id;
    bvar.kind = var.kind;
    compile_node( var.expr.value(), bvar.expr );
}

void dawn::Compiler::compile_struct( Struct const& struc, BStruct& bstruc )
{
    COMPILER_PANIC( "comp struct not impl" );
}

void dawn::Compiler::compile_enum( Enum const& enu, BEnum& benu )
{
    COMPILER_PANIC( "comp enum not impl" );
}

void dawn::Compiler::compile_node( Node const& node, Vector<Instruction>& instrs )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
        compile_scope( node.as<Scope>(), instrs );
        break;

    case NodeType::VARIABLE:
        compile_var( node.as<VarNod>(), instrs );
        break;

    case NodeType::FUNCTION:
        compile_func( node.as<FuncNod>(), instrs );
        break;

    case NodeType::RETURN:
        compile_return( node.as<ReturnNod>(), instrs );
        break;

    case NodeType::BREAK:
        compile_break( node.as<BreakNod>(), instrs );
        break;

    case NodeType::CONTINUE:
        compile_continue( node.as<ContinueNod>(), instrs );
        break;

    case NodeType::THROW:
        compile_throw( node.as<ThrowNod>(), instrs );
        break;

    case NodeType::TRY:
        compile_try( node.as<TryNod>(), instrs );
        break;

    case NodeType::IF:
        compile_if( node.as<IfNod>(), instrs );
        break;

    case NodeType::SWITCH:
        compile_switch( node.as<SwitchNod>(), instrs );
        break;

    case NodeType::LOOP:
        compile_loop( node.as<LoopNod>(), instrs );
        break;

    case NodeType::WHILE:
        compile_while( node.as<WhileNod>(), instrs );
        break;

    case NodeType::FOR:
        compile_for( node.as<ForNod>(), instrs );
        break;

    case NodeType::REF:
        compile_ref( node.as<RefNod>(), instrs );
        break;

    case NodeType::IDENTIFIER:
        compile_identifier( node.as<IdentifierNod>(), instrs );
        break;

    case NodeType::CALL:
        compile_call( node.as<CallNod>(), instrs );
        break;

    case NodeType::INDEX:
        compile_index( node.as<IndexNod>(), instrs );
        break;

    case NodeType::ENUM:
        compile_enu( node.as<EnumNod>(), instrs );
        break;

    case NodeType::STRUCT:
        compile_struc( node.as<StructNod>(), instrs );
        break;

    case NodeType::ARRAY:
        compile_array( node.as<ArrayNod>(), instrs );
        break;

    case NodeType::UNARY:
        compile_unary( node.as<UnaryNod>(), instrs );
        break;

    case NodeType::OPERATOR:
        compile_operator( node.as<OperatorNod>(), instrs );
        break;

    case NodeType::ASSIGN:
        compile_assign( node.as<AssignNod>(), instrs );
        break;

    default:
        COMPILER_PANIC( "unknown node type ", (Int) node.type() );
    }
}

void dawn::Compiler::compile_scope( Scope const& scope, Vector<Instruction>& instrs )
{
    instrs.emplace_back( InstructionType::PUSHS, 0 );

    for ( auto& node : scope.instr )
        compile_node( node, instrs );

    instrs.emplace_back( InstructionType::POPS, 0 );
}

void dawn::Compiler::compile_var( VarNod const& nod, Vector<Instruction>& instrs )
{
    compile_node( nod.var.expr.value(), instrs );
    instrs.emplace_back( InstructionType::PUSHV, (int32_t) nod.var.kind, (int32_t) nod.var.id );
}

void dawn::Compiler::compile_func( FuncNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile func not impl" );
}

void dawn::Compiler::compile_return( ReturnNod const& nod, Vector<Instruction>& instrs )
{
    compile_node( nod.expr.value(), instrs );
    instrs.emplace_back( InstructionType::RET, 0 );
}

void dawn::Compiler::compile_break( BreakNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile break not impl" );
}

void dawn::Compiler::compile_continue( ContinueNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile continue not impl" );
}

void dawn::Compiler::compile_throw( ThrowNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile throw not impl" );
}

void dawn::Compiler::compile_try( TryNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile try not impl" );
}

void dawn::Compiler::compile_if( IfNod const& nod, Vector<Instruction>& instrs )
{
    Vector<Int> jumpover_indices;

    for ( auto& part : nod.parts )
    {
        compile_node( part.expr, instrs );
        instrs.emplace_back( InstructionType::JUMPIFN, 0 );
        Int ifnot_index = (Int) instrs.size() - 1;

        compile_scope( part.scope, instrs );
        instrs.emplace_back( InstructionType::JUMP, 0 );
        jumpover_indices.emplace_back( (Int) instrs.size() - 1 );

        instrs[ifnot_index].value = (Int) instrs.size();
    }

    for ( auto& index : jumpover_indices )
        instrs[index].value = (Int) instrs.size();
}

void dawn::Compiler::compile_switch( SwitchNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile switch not impl" );
}

void dawn::Compiler::compile_loop( LoopNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile loop not impl" );
}

void dawn::Compiler::compile_while( WhileNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile while not impl" );
}

void dawn::Compiler::compile_for( ForNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile for not impl" );
}

void dawn::Compiler::compile_ref( RefNod const& nod, Vector<Instruction>& instrs )
{
    instrs.emplace_back( InstructionType::STORE, nod.value_ref );
}

void dawn::Compiler::compile_identifier( IdentifierNod const& nod, Vector<Instruction>& instrs )
{
    instrs.emplace_back( InstructionType::FINDV, nod.id );
}

void dawn::Compiler::compile_call( CallNod const& nod, Vector<Instruction>& instrs )
{
    compile_node( nod.left_expr.value(), instrs );
    instrs.emplace_back( InstructionType::PUSHC, 0 );

    for ( auto& arg : nod.args )
    {
        compile_node( arg, instrs );
        instrs.emplace_back( InstructionType::PUSHC, 0 );
    }

    instrs.emplace_back( InstructionType::CALL, 0 );
}

void dawn::Compiler::compile_index( IndexNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile index not impl" );
}

void dawn::Compiler::compile_enu( EnumNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile enum not impl" );
}

void dawn::Compiler::compile_struc( StructNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile struc not impl" );
}

void dawn::Compiler::compile_array( ArrayNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile array not impl" );
}

void dawn::Compiler::compile_unary( UnaryNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile unary not impl" );
}

void dawn::Compiler::compile_operator( OperatorNod const& nod, Vector<Instruction>& instrs )
{
    compile_node( nod.sides[0], instrs );
    compile_node( nod.sides[1], instrs );
    instrs.emplace_back( InstructionType::OP, (int32_t) nod.type );
}

void dawn::Compiler::compile_assign( AssignNod const& nod, Vector<Instruction>& instrs )
{
    PANIC( "compile assign not impl" );
}
