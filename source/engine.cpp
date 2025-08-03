#include "engine.h"


dawn::Engine::Engine()
{
    load_standard_functions();
    load_standard_members();
}

void dawn::Engine::load_mod( Module const& module )
{
    for ( auto& entry : module.functions )
        load_function( entry );

    for ( auto& entry : module.enums )
        load_enum( entry );

    for ( auto& entry : module.structs )
        load_struct( entry );

    for ( auto& entry : module.variables )
        load_variable( entry );
}

void dawn::Engine::load_function( Function const& entry )
{
    if ( stack.root().get( entry.id ) )
        ENGINE_PANIC( "object [", IDSystem::get( entry.id ), "] already exists" );
    stack.root().set( entry.id, Value{ entry } );
}

void dawn::Engine::load_enum( Enum const& entry )
{
    enums[entry.id] = entry;
}

void dawn::Engine::load_struct( Struct const& entry )
{
    structs[entry.id] = entry;
}

void dawn::Engine::load_variable( Variable const& entry )
{
    add_var( entry.kind, entry.id, handle_expr( entry.expr.value() ) );
}

void dawn::Engine::bind_func( Int id, CFuncBody cfunc )
{
    Function func;
    func.id = id;
    func.body.emplace<CFuncBody>( std::move( cfunc ) );
    load_function( func );
}

dawn::Value dawn::Engine::call_func( Int id, Value* args, Int arg_count )
{
    Value* value = stack.root().get( id );
    if ( !value )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] doesn't exist" );

    if ( value->type() != ValueType::FUNCTION )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] can't be called" );

    return handle_func( value->as_function(), args, arg_count );
}

void dawn::Engine::add_var( VariableKind kind, Int id, Value const& value )
{
    if ( kind == VariableKind::CONST )
        stack.current().set( id, value.clone() );
    else if ( kind == VariableKind::VAR )
        stack.current().set( id, value.clone().unlock_const() );
    else
        stack.current().set( id, value );
}

dawn::Value* dawn::Engine::get_var( Int id )
{
    return stack.current().get( id );
}

void dawn::Engine::add_type_member( ValueType type, String const& name, Func<Value( Value& )> const& func )
{
    member_generators[(Int) type][IDSystem::get( name )] = [=]( Value const& self ) -> Value
        {
            return func( const_cast<Value&>( self ) );
        };
}

void dawn::Engine::add_type_method( ValueType type, String const& name, Bool is_const, Int expected_args, Func<Value( Value&, Value* )> const& body )
{
    const Int id = IDSystem::get( name );
    member_generators[(Int) type][id] = [=]( Value const& self ) -> Value
        {
            Function func;
            func.id = id;
            *func.METHOD_self = self;
            func.body = [=]( Value* args, Int arg_count ) -> Value
                {
                    if ( !is_const && self.is_const() )
                        ENGINE_PANIC( "can't call [", name, "] on a const value" );
                    if ( ( 1 + expected_args ) != arg_count )
                        ENGINE_PANIC( "method [", name, "] expects self + ", expected_args, " arguments" );
                    return body( args[0], args + 1 );
                };
            return (Value) func;
        };
}

dawn::Value dawn::Engine::handle_func( Function const& func, Value* args, Int arg_count )
{
    if ( func.body.index() == 0 )
    {
        if ( func.args.size() != arg_count )
        {
            if ( func.type() == FunctionType::LAMBDA )
                ENGINE_PANIC( "invalid argument count for lambda" );
            else if ( func.type() == FunctionType::METHOD )
                ENGINE_PANIC( "invalid argument count for method [", IDSystem::get( func.id ), "]" );
            else
                ENGINE_PANIC( "invalid argument count for function [", IDSystem::get( func.id ), "]" );
        }

        auto pop_handler = stack.push_from(
            func.type() == FunctionType::LAMBDA ? func.LAMBDA_frame : RegisterRef<Frame>{} );

        for ( Int i = 0; i < arg_count; i++ )
            add_var( func.args[i].kind, func.args[i].id, args[i] );

        Opt<Value> retval;
        handle_scope( std::get<Scope>( func.body ), retval, nullptr, nullptr );
        return retval ? *retval : Value{};
    }
    else
    {
        return std::get<CFuncBody>( func.body )( args, arg_count );
    }
}

void dawn::Engine::handle_scope( Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& instr : scope.instr )
    {
        if ( retval || ( didbrk && *didbrk ) || ( didcon && *didcon ) )
            break;

        handle_instr( instr, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_instr( Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
    {
        auto pop_handler = stack.push();
        handle_scope( node.as<Scope>(), retval, didbrk, didcon );
    }
    break;

    case NodeType::VARIABLE:
        handle_var_node( node.as<VariableNode>() );
        break;

    case NodeType::RETURN:
        handle_return_node( node.as<ReturnNode>(), retval );
        break;

    case NodeType::BREAK:
        handle_break_node( node.as<BreakNode>(), didbrk );
        break;

    case NodeType::CONTINUE:
        handle_continue_node( node.as<ContinueNode>(), didcon );
        break;

    case NodeType::THROW:
        handle_throw_node( node.as<ThrowNode>() );
        break;

    case NodeType::TRY:
        handle_try_node( node.as<TryNode>(), retval, didbrk, didcon );
        break;

    case NodeType::IF:
        handle_if_node( node.as<IfNode>(), retval, didbrk, didcon );
        break;

    case NodeType::SWITCH:
        handle_switch_node( node.as<SwitchNode>(), retval, didbrk, didcon );
        break;

    case NodeType::LOOP:
        handle_loop_node( node.as<LoopNode>(), retval );
        break;

    case NodeType::WHILE:
        handle_while_node( node.as<WhileNode>(), retval );
        break;

    case NodeType::FOR:
        handle_for_node( node.as<ForNode>(), retval );
        break;

    default:
        handle_expr( node );
    }
}

dawn::Value dawn::Engine::handle_expr( Node const& node )
{
    switch ( node.type() )
    {
    case NodeType::VALUE:
        return handle_value_node( node.as<ValueNode>() );

    case NodeType::IDENTIFIER:
        return handle_id_node( node.as<IdentifierNode>() );

    case NodeType::CALL:
        return handle_call_node( node.as<CallNode>() );

    case NodeType::INDEX:
        return handle_index_node( node.as<IndexNode>() );

    case NodeType::ENUM:
        return handle_enum_node( node.as<EnumNode>() );

    case NodeType::STRUCT:
        return handle_struct_node( node.as<StructNode>() );

    case NodeType::ARRAY:
        return handle_array_node( node.as<ArrayNode>() );

    case NodeType::UNARY:
        return handle_un_node( node.as<UnaryNode>() );

    case NodeType::OPERATOR:
        return handle_op_node( node.as<OperatorNode>() );

    case NodeType::ASSIGN:
        return handle_as_node( node.as<AssignNode>() );

    default:
        ENGINE_PANIC( "unknown expr node type: ", (Int) node.type() );
    }
}

dawn::Value dawn::Engine::handle_value_node( ValueNode const& node )
{
    if ( node.value.type() == ValueType::FUNCTION )
    {
        auto& func = node.value.as_function();
        if ( func.type() == FunctionType::LAMBDA )
            func.LAMBDA_frame = stack.peek();
    }
    return node.value;
}

void dawn::Engine::handle_var_node( VariableNode const& node )
{
    add_var( node.var.kind, node.var.id, handle_expr( node.var.expr.value() ) );
}

dawn::Value dawn::Engine::handle_id_node( IdentifierNode const& node )
{
    auto* ptr = get_var( node.id );
    if ( !ptr )
        ENGINE_PANIC( "object [", IDSystem::get( node.id ), "] doesn't exist" );
    return *ptr;
}

dawn::Value dawn::Engine::handle_call_node( CallNode const& node )
{
    Value left = handle_expr( node.left_expr.value() );
    if ( left.type() != ValueType::FUNCTION )
        ENGINE_PANIC( "can't call [", left.type(), "]" );

    auto& func = left.as_function();
    Int arg_count = ( func.type() == FunctionType::METHOD ) ? ( 1 + node.args.size() ) : node.args.size();

    Value* args_ptr = SALLOC( Value, arg_count );
    SAllocManager alloc_manager{ args_ptr, arg_count };

    if ( func.type() == FunctionType::METHOD )
    {
        args_ptr[0] = *func.METHOD_self;
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[1 + i] = handle_expr( node.args[i] );
    }
    else
    {
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[i] = handle_expr( node.args[i] );
    }

    return handle_func( func, args_ptr, arg_count );
}

dawn::Value dawn::Engine::handle_index_node( IndexNode const& node )
{
    Value left = handle_expr( node.left_expr.value() );
    Int index = handle_expr( node.expr.value() ).to_int( *this );

    if ( left.type() == ValueType::STRING )
    {
        auto& value = left.as_string();
        if ( index < 0 || index >= (Int) value.size() )
            ENGINE_PANIC( "string access [", index, "] out of bounds" );
        return Value{ value[index] };
    }
    else if ( left.type() == ValueType::ARRAY )
    {
        auto& value = left.as_array();
        if ( index < 0 || index >= (Int) value.data.size() )
            ENGINE_PANIC( "array access [", index, "] out of bounds" );
        return value.data[index];
    }
    else
        ENGINE_PANIC( "can't index type [", left.type(), "]" );
}

void dawn::Engine::handle_return_node( ReturnNode const& node, Opt<Value>& retval )
{
    retval = handle_expr( node.expr.value() );
}

void dawn::Engine::handle_break_node( BreakNode const& node, Bool* didbrk )
{
    if ( !didbrk )
        ENGINE_PANIC( "break statement outside of loop" );
    *didbrk = true;
}

void dawn::Engine::handle_continue_node( ContinueNode const& node, Bool* didcon )
{
    if ( !didcon )
        ENGINE_PANIC( "continue statement outside of loop" );
    *didcon = true;
}

void dawn::Engine::handle_throw_node( ThrowNode const& node )
{
    throw handle_expr( node.expr.value() );
}

void dawn::Engine::handle_try_node( TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    try
    {
        auto pop_handler = stack.push();
        handle_scope( node.try_scope, retval, didbrk, didcon );
    }
    catch ( Value const& value )
    {
        auto pop_handler = stack.push();
        add_var( VariableKind::REF, node.catch_id, value );
        handle_scope( node.catch_scope, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_if_node( IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& part : node.parts )
    {
        if ( !handle_expr( part.expr ).to_bool( *this ) )
            continue;

        auto pop_handler = stack.push();
        handle_scope( part.scope, retval, didbrk, didcon );
        break;
    }
}

void dawn::Engine::handle_switch_node( SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    Value check_value = handle_expr( node.main_expr.value() );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            if ( !check_value.op_eq( *this, handle_expr( expr ) ).to_bool( *this ) )
                continue;

            auto pop_handler = stack.push();
            handle_scope( case_part.scope, retval, didbrk, didcon );
            return;
        }
    }

    if ( node.def_scope )
    {
        auto pop_handler = stack.push();
        handle_scope( *node.def_scope, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_loop_node( LoopNode const& node, Opt<Value>& retval )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( retval || didbrk )
            break;
        didcon = false;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_while_node( WhileNode const& node, Opt<Value>& retval )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( retval || didbrk )
            break;
        didcon = false;

        if ( !handle_expr( node.expr.value() ).to_bool( *this ) )
            break;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNode const& node, Opt<Value>& retval )
{
    Value loop_value = handle_expr( node.expr.value() );

    if ( loop_value.type() == ValueType::RANGE )
    {
        auto& value_rng = loop_value.as_range();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, Value{ i } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_value.type() == ValueType::STRING )
    {
        auto& value_str = loop_value.as_string();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, Value{ c } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_value.type() == ValueType::ARRAY )
    {
        auto& value_arr = loop_value.as_array();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, value );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else
        ENGINE_PANIC( "can't for loop [", loop_value.type(), "]" );
}

dawn::Value dawn::Engine::handle_enum_node( EnumNode const& node )
{
    auto enum_it = enums.find( node.type_id );
    if ( enum_it == enums.end() )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't exist" );

    if ( !enum_it->second.contains( node.key_id ) )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't have key [", IDSystem::get( node.key_id ), "]" );

    EnumValue result{};
    result.parent = &enum_it->second;
    result.key_id = node.key_id;

    return Value{ result };
}

dawn::Value dawn::Engine::handle_struct_node( StructNode const& node )
{
    auto struct_it = structs.find( node.type_id );
    if ( struct_it == structs.end() )
        ENGINE_PANIC( "struct [", IDSystem::get( node.type_id ), "] doesn't exist" );
    auto& struc = struct_it->second;

    Value value{ StructValue{} };

    auto& struc_value = value.as_struct();
    struc_value.parent = &struc;

    // struct default init
    {
        static const Int self_id = IDSystem::get( (String) kw_self );
        auto pop_handler = stack.push_from( RegisterRef<Frame>{} );
        stack.current().set( self_id, value );
        for ( auto& field : struc.fields )
            struc_value.members[field.id] = handle_expr( field.expr.value() ).clone();
    }

    // struct {} init
    for ( auto& [id, arg_node] : node.args )
    {
        auto& member = struc_value.members[id];
        Value expr = handle_expr( arg_node ).clone();
        if ( member.type() != expr.type() )
            ENGINE_PANIC( "can't assign type [", expr.type(), "] to type [", member.type(), "]" );
        // must use = instead of assign() because member is const at this stage
        member = expr;
    }

    // methods
    for ( auto& method : struc.methods )
    {
        auto& func = ( struc_value.members[method.id] = Value{ method } ).as_function();
        *func.METHOD_self = value;
    }

    return value;
}

dawn::Value dawn::Engine::handle_array_node( ArrayNode const& node )
{
    ArrayValue result{};

    if ( node.type == ArrayType::SIZE )
    {
        Int size = handle_expr( node.SIZE_expr.value() ).to_int( *this );
        if ( size < 0 )
            ENGINE_PANIC( "array size can't be negative" );

        Value value = create_default_value( node.SIZE_typeid );
        result.data.reserve( size );
        for ( Int i = 0; i < size; i++ )
            result.data.push_back( value.clone() );
    }
    else
    {
        result.data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
            result.data.push_back( handle_expr( expr ).clone() );
    }

    return Value{ result };
}

dawn::Value dawn::Engine::handle_un_node( UnaryNode const& node )
{
    switch ( node.type )
    {
    case UnaryType::PLUS:
        return Value{ handle_expr( node.right.value() ).un_plus( *this ) };

    case UnaryType::MINUS:
        return Value{ handle_expr( node.right.value() ).un_minus( *this ) };

    case UnaryType::NOT:
        return Value{ handle_expr( node.right.value() ).un_not( *this ) };

    default:
        ENGINE_PANIC( "unknown unary node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_op_node( OperatorNode const& node )
{
    switch ( node.type )
    {
    case OperatorType::ACCESS:
        return handle_ac_node( node );

    case OperatorType::POW:
        return handle_expr( node.sides[0] ).op_pow( *this, handle_expr( node.sides[1] ) );

    case OperatorType::MOD:
        return handle_expr( node.sides[0] ).op_mod( *this, handle_expr( node.sides[1] ) );

    case OperatorType::MUL:
        return handle_expr( node.sides[0] ).op_mul( *this, handle_expr( node.sides[1] ) );

    case OperatorType::DIV:
        return handle_expr( node.sides[0] ).op_div( *this, handle_expr( node.sides[1] ) );

    case OperatorType::ADD:
        return handle_expr( node.sides[0] ).op_add( *this, handle_expr( node.sides[1] ) );

    case OperatorType::SUB:
        return handle_expr( node.sides[0] ).op_sub( *this, handle_expr( node.sides[1] ) );

    case OperatorType::COMPARE:
        return handle_expr( node.sides[0] ).op_cmpr( *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS:
        return handle_expr( node.sides[0] ).op_less( *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT:
        return handle_expr( node.sides[0] ).op_great( *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS_EQ:
        return handle_expr( node.sides[0] ).op_lesseq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT_EQ:
        return handle_expr( node.sides[0] ).op_greateq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::EQ:
        return handle_expr( node.sides[0] ).op_eq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::NOT_EQ:
        return handle_expr( node.sides[0] ).op_neq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::AND:
        return handle_expr( node.sides[0] ).op_and( *this, handle_expr( node.sides[1] ) );

    case OperatorType::OR:
        return handle_expr( node.sides[0] ).op_or( *this, handle_expr( node.sides[1] ) );

    case OperatorType::RANGE:
        return handle_expr( node.sides[0] ).op_range( *this, handle_expr( node.sides[1] ) );

    default:
        ENGINE_PANIC( "unknown operator node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_ac_node( OperatorNode const& node )
{
    Value left = handle_expr( node.sides[0] );

    if ( node.sides[1].type() != NodeType::IDENTIFIER )
        ENGINE_PANIC( "access must be an identifier" );

    Int right_id = node.sides[1].as<IdentifierNode>().id;
    if ( left.type() == ValueType::STRUCT )
        return handle_ac_struct_node( left, right_id );
    else
        return handle_ac_type_node( left, right_id );
}

dawn::Value dawn::Engine::handle_as_node( AssignNode const& node )
{
    Value left = handle_expr( node.sides[0] );

    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left.assign( handle_expr( node.sides[1] ) );
        return left;

    case AssignType::ADD:
        left.assign( left.op_add( *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::SUB:
        left.assign( left.op_sub( *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MUL:
        left.assign( left.op_mul( *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::DIV:
        left.assign( left.op_div( *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::POW:
        left.assign( left.op_pow( *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MOD:
        left.assign( left.op_mod( *this, handle_expr( node.sides[1] ) ) );
        return left;

    default:
        ENGINE_PANIC( "unknown assign node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_ac_struct_node( Value const& self, Int right_id )
{
    auto& left = self.as_struct();
    if ( !left.members.contains( right_id ) )
        ENGINE_PANIC( "struct [", IDSystem::get( left.parent->id ), "] doesn't have member [", IDSystem::get( right_id ), "]" );
    return left.members.at( right_id );
}

dawn::Value dawn::Engine::handle_ac_type_node( Value const& self, Int right_id )
{
    auto& generators = member_generators[(Int) self.type()];
    if ( !generators.contains( right_id ) )
        ENGINE_PANIC( "type [", self.type(), "] doesn't have member [", IDSystem::get( right_id ), "]" );
    return generators.at( right_id )( self );
}

dawn::Value dawn::Engine::create_default_value( Int typeid_ )
{
    static const Int _boolid = IDSystem::get( (String) tp_bool );
    static const Int _intid = IDSystem::get( (String) tp_int );
    static const Int _floatid = IDSystem::get( (String) tp_float );
    static const Int _charid = IDSystem::get( (String) tp_char );
    static const Int _stringid = IDSystem::get( (String) tp_string );

    if ( typeid_ == _boolid )
        return Value{ Bool{} };

    else if ( typeid_ == _intid )
        return Value{ Int{} };

    else if ( typeid_ == _floatid )
        return Value{ Float{} };

    else if ( typeid_ == _charid )
        return Value{ Char{} };

    else if ( typeid_ == _stringid )
        return Value{ String{} };

    else if ( enums.contains( typeid_ ) )
    {
        auto& entry = *enums.at( typeid_ ).entries.begin();
        EnumNode node;
        node.type_id = typeid_;
        node.key_id = entry.id;
        return handle_enum_node( node );
    }

    else if ( structs.contains( typeid_ ) )
    {
        StructNode node;
        node.type_id = typeid_;
        return handle_struct_node( node );
    }

    else
        ENGINE_PANIC( "type [", IDSystem::get( typeid_ ), "] doesn't exist" );
}
