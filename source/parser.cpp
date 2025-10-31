#include "parser.h"
#include "pool.h"
#include "engine.h"


dawn::Bool dawn::Module::contains_id( ID id ) const
{
    if ( std::find_if( variables.begin(), variables.end(), [&]( Variable const& var ) { return var.id == id; } ) != variables.end() )
        return true;

    if ( std::find_if( functions.begin(), functions.end(), [&]( Function const& func ) { return func.id == id; } ) != functions.end() )
        return true;

    if ( std::find_if( enums.begin(), enums.end(), [&]( Enum const& en ) { return en.id == id; } ) != enums.end() )
        return true;

    if ( std::find_if( structs.begin(), structs.end(), [&]( Struct const& struc ) { return struc.id == id; } ) != structs.end() )
        return true;

    return false;
}

dawn::Bool dawn::TokenIterator::valid() const
{
    return m_ptr >= m_start && m_ptr < m_end;
}

dawn::Token const& dawn::TokenIterator::operator*() const
{
    if ( !valid() )
    {
        TokenIterator it_before = *this;
        --it_before;
        const Token token_before = it_before.valid() ? *it_before : Token{};
        PARSER_PANIC( token_before, "dereferencing non-existent token after" );
    }
    return *m_ptr;
}

dawn::Token const* dawn::TokenIterator::operator->() const
{
    if ( !valid() )
    {
        TokenIterator it_before = *this;
        --it_before;
        const Token token_before = it_before.valid() ? *it_before : Token{};
        PARSER_PANIC( token_before, "accessing non-existent token after" );
    }
    return m_ptr;
}

dawn::Bool dawn::TokenIterator::operator==( TokenIterator const& other ) const
{
    return m_ptr == other.m_ptr;
}

void dawn::TokenIterator::operator++()
{
    ++m_ptr;
}

void dawn::TokenIterator::operator--()
{
    --m_ptr;
}

void dawn::Parser::parse( Vector<Token> const& tokens, Module& module )
{
    parse( tokens.data(), (Int) tokens.size(), module );
}

void dawn::Parser::parse( Token const* token_ptr, Int token_count, Module& module )
{
    TokenIterator it{ token_ptr, token_ptr + token_count };
    while ( it.valid() )
    {
        if ( it->value == kw_import )
        {
            parse_import( it, module );
        }
        else if ( it->value == kw_struct )
        {
            parse_global_struct( it, module );
        }
        else if ( it->value == kw_enum )
        {
            parse_global_enum( it, module );
        }
        else if ( it->value == kw_func )
        {
            parse_global_function( it, module );
        }
        else if ( it->value == kw_oper )
        {
            parse_global_operator( it, module );
        }
        else if ( is_variable( it ) )
        {
            parse_global_variable( it, module );
        }
        else
            PARSER_PANIC( *it, "not allowed in global scope or allowed only 1 instance of" );
    }
}

dawn::Bool dawn::Parser::is_variable( TokenIterator const& it )
{
    if ( it->type != TokenType::TYPE )
        return false;
    TokenIterator next = it;
    ++next;
    return next->value == vr_variable
        || next->value == vr_reference
        || next->type == TokenType::NAME;
}

void dawn::Parser::parse_import( TokenIterator& it, Module& module )
{
    if ( it->value != kw_import )
        PARSER_PANIC( *it, "expected import keyword" );
    ++it;

    if ( it->type != TokenType::STRING )
        PARSER_PANIC( *it, "expected import path" );
    module.imports.insert( it->literal );
    ++it;
}

void dawn::Parser::parse_global_struct( TokenIterator& it, Module& module )
{
    const auto first_it = it;

    Struct struc;
    parse_struct( it, struc );

    if ( module.contains_id( struc.id ) )
        PARSER_PANIC( *first_it, "name [", IDSystem::get( struc.id ), "] already in use" );

    module.structs.push_back( struc );
}

void dawn::Parser::parse_global_enum( TokenIterator& it, Module& module )
{
    const auto first_it = it;

    Enum en;
    parse_enum( it, en );

    if ( module.contains_id( en.id ) )
        PARSER_PANIC( *first_it, "name [", IDSystem::get( en.id ), "] already in use" );

    module.enums.push_back( en );
}

void dawn::Parser::parse_global_function( TokenIterator& it, Module& module )
{
    const auto first_it = it;

    Function function;
    parse_function( it, function );

    if ( module.contains_id( function.id ) )
        PARSER_PANIC( *first_it, "name [", IDSystem::get( function.id ), "] already in use" );

    module.functions.push_back( function );
}

void dawn::Parser::parse_global_operator( TokenIterator& it, Module& module )
{
    const auto first_it = it;

    Operator op;
    parse_operator( it, op );

    module.operators.push_back( op );
}

void dawn::Parser::parse_global_variable( TokenIterator& it, Module& module )
{
    const auto first_it = it;

    Variable variable;
    parse_variable( it, variable );

    if ( variable.type.kind != VarKind::CONSTANT )
        PARSER_PANIC( *first_it, "global variables must be const" );

    if ( module.contains_id( variable.id ) )
        PARSER_PANIC( *first_it, "name [", IDSystem::get( variable.id ), "] already in use" );

    module.variables.push_back( variable );
}

void dawn::Parser::parse_struct( TokenIterator& it, Struct& struc )
{
    if ( it->value != kw_struct )
        PARSER_PANIC( *it, "expected struct" );
    ++it;

    if ( !is_custom_type( it->value ) )
        PARSER_PANIC( *it, "expected struct name" );
    struc.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    while ( it->value != op_scope_cls )
    {
        if ( it->type == TokenType::NAME )
        {
            ID name_id = IDSystem::get( it->value );
            if ( struc.contains( name_id ) )
                PARSER_PANIC( *it, "struct field [", IDSystem::get( name_id ), "] already defined" );
            ++it;

            if ( it->value != op_set )
                PARSER_PANIC( *it, "expected struct field type setter" );
            ++it;

            if ( it->type != TokenType::TYPE )
                PARSER_PANIC( *it, "expected field type" );
            const ID type_id = IDSystem::get( it->value );
            if ( type_id == struc.id )
                PARSER_PANIC( *it, "field type can not be the same as the parent struct" );
            struc.fields.emplace_back( name_id, type_id );
            ++it;
        }
        else if ( it->value == kw_func )
        {
            Function method;
            parse_function( it, method );
            if ( struc.contains( method.id ) )
                PARSER_PANIC( *it, "struct method [", IDSystem::get( method.id ), "] already defined" );

            auto& self_var = *method.args.emplace( method.args.begin() );
            self_var.type.type_id = struc.id;
            self_var.type.kind = VarKind::REFERENCE;
            self_var.id = IDSystem::get( kw_self );
            struc.methods.push_back( method );
        }
        else if ( it->value == kw_cast )
        {
            Function cast;
            parse_cast( it, cast );
            if ( struc.contains( cast.id ) )
                PARSER_PANIC( *it, "struct cast [", IDSystem::get( cast.id ), "] already defined" );

            auto& self_var = *cast.args.emplace( cast.args.begin() );
            self_var.type.type_id = struc.id;
            self_var.type.kind = VarKind::REFERENCE;
            self_var.id = IDSystem::get( kw_self );
            struc.methods.push_back( cast );
        }
        else
            PARSER_PANIC( *it, "expected field name, func or cast" );
    }
    ++it;
}

void dawn::Parser::parse_enum( TokenIterator& it, Enum& en )
{
    const auto first_it = it;

    if ( it->value != kw_enum )
        PARSER_PANIC( *it, "expected enum" );
    ++it;

    if ( !is_custom_type( it->value ) )
        PARSER_PANIC( *it, "expected enum name" );
    en.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    while ( it->value != op_scope_cls )
    {
        if ( it->type == TokenType::NAME )
        {
            ID name_id = IDSystem::get( it->value );
            if ( en.get( name_id ) )
                PARSER_PANIC( *it, "key [", it->value, "] already in use" );

            auto& entry = en.entries.emplace_back();
            entry.id = name_id;
            entry.expr = node_pool().new_register();
            auto& expr = *std::get<NodeRef>( entry.expr );
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( ExtractType::NEW_LINE, it, expr );
            }
            else
                expr.emplace<Value>();
        }
        else
            PARSER_PANIC( *it, "expected key name" );
    }
    ++it;

    if ( en.entries.empty() )
        PARSER_PANIC( *first_it, "enum [", IDSystem::get( en.id ), "] can not be empty" );
}

void dawn::Parser::parse_operator( TokenIterator& it, Operator& oper )
{
    if ( it->value != kw_oper )
        PARSER_PANIC( *it, "expected oper" );
    ++it;

    if ( it->type != TokenType::OPERATOR )
        PARSER_PANIC( *it, "expected operator" );
    const String op_val = it->value;
    oper.type = get_op( op_val );
    ++it;

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, "expected open expr" );
    ++it;

    Set<ID> arg_set;
    while ( it->value != op_expr_cls )
    {
        auto& arg = oper.args.emplace_back();

        if ( it->type != TokenType::TYPE )
            PARSER_PANIC( *it, "expected argument type" );
        arg.type.type_id = IDSystem::get( it->value );
        ++it;

        if ( it->value == vr_variable )
        {
            arg.type.kind = VarKind::VARIABLE;
            ++it;
        }
        else if ( it->value == vr_reference )
        {
            arg.type.kind = VarKind::REFERENCE;
            ++it;
        }
        else
            arg.type.kind = VarKind::CONSTANT;

        if ( it->type != TokenType::NAME )
            PARSER_PANIC( *it, "expected arg name" );
        arg.id = IDSystem::get( it->value );

        if ( arg_set.contains( arg.id ) )
            PARSER_PANIC( *it, "argument [", it->value, "] already defined" );
        arg_set.insert( arg.id );
        ++it;

        if ( it->value != op_expr_cls )
        {
            if ( it->value != op_split )
                PARSER_PANIC( *it, "expected split or expression close" );
            ++it;
        }
    }
    ++it;

    if ( oper.args.size() == 1 )
    {
        auto& arg = *oper.args.emplace( oper.args.begin() );
        arg.type = VarType{ .type_id = IDSystem::get( tp_void ), .kind = VarKind::CONSTANT };
    }
    else if ( oper.args.size() != 2 )
        PARSER_PANIC( *it, "operator must have either 1 or 2 arguments" );

    parse_scope( it, oper.body );
}

void dawn::Parser::parse_function( TokenIterator& it, Function& function )
{
    if ( it->value != kw_func )
        PARSER_PANIC( *it, "expected function" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected function name" );
    function.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, "expected expression open" );
    ++it;

    Set<ID> args;
    while ( it->value != op_expr_cls )
    {
        auto& arg = function.args.emplace_back();

        if ( it->type != TokenType::TYPE )
            PARSER_PANIC( *it, "expected argument type" );
        arg.type.type_id = IDSystem::get( it->value );
        ++it;

        if ( it->value == vr_variable )
        {
            arg.type.kind = VarKind::VARIABLE;
            ++it;
        }
        else if ( it->value == vr_reference )
        {
            arg.type.kind = VarKind::REFERENCE;
            ++it;
        }
        else
            arg.type.kind = VarKind::CONSTANT;

        if ( it->type != TokenType::NAME )
            PARSER_PANIC( *it, "expected arg name" );
        arg.id = IDSystem::get( it->value );

        if ( args.contains( arg.id ) )
            PARSER_PANIC( *it, "argument [", it->value, "] already defined" );
        args.insert( arg.id );
        ++it;

        if ( it->value != op_expr_cls )
        {
            if ( it->value != op_split )
                PARSER_PANIC( *it, "expected split or expression close" );
            ++it;
        }
    }
    ++it;

    parse_scope( it, function.body );
}

void dawn::Parser::parse_cast( TokenIterator& it, Function& function )
{
    if ( it->value != kw_cast )
        PARSER_PANIC( *it, "expected cast" );
    ++it;

    if ( it->value != tp_void &&
        it->value != tp_bool &&
        it->value != tp_int &&
        it->value != tp_float &&
        it->value != tp_char &&
        it->value != tp_string &&
        it->value != tp_range &&
        it->value != tp_function &&
        it->value != tp_array )
        PARSER_PANIC( *it, "expected cast type" );
    function.id = IDSystem::get( it->value );
    ++it;

    parse_scope( it, function.body );
}

void dawn::Parser::parse_variable( TokenIterator& it, Variable& variable )
{
    if ( it->type != TokenType::TYPE )
        PARSER_PANIC( *it, "expected variable type" );
    const Bool is_custom_type = dawn::is_custom_type( it->value );
    variable.type.type_id = IDSystem::get( it->value );
    ++it;

    if ( it->value == vr_variable )
    {
        variable.type.kind = VarKind::VARIABLE;
        ++it;
    }
    else if ( it->value == vr_reference )
    {
        variable.type.kind = VarKind::REFERENCE;
        ++it;
    }
    else
        variable.type.kind = VarKind::CONSTANT;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected variable name" );
    variable.id = IDSystem::get( it->value );
    const Location var_location = it->location;
    ++it;

    variable.expr = node_pool().new_register();
    if ( it->value == op_assign )
    {
        ++it;
        parse_expression( ExtractType::NEW_LINE, it, *variable.expr );
    }
    else if ( !is_custom_type )
        variable.expr->emplace<Value>( create_default_value( nullptr, variable.type.type_id, var_location ) );
    else
        PARSER_PANIC( *it, "custom type variable must be initialized" );
}

void dawn::Parser::parse_expression( ExtractType type, TokenIterator& it, Node& tree )
{
    if ( !it.valid() )
    {
        tree.emplace<Value>();
        return;
    }

    Vector<Token> expr_tokens;
    expression_extract( type, it, expr_tokens );

    Int least_prec_op = -1;
    Bool op_is_unary = false;
    expression_precedence( expr_tokens, least_prec_op, op_is_unary );

    if ( least_prec_op >= 0 )
    {
        if ( op_is_unary )
        {
            if ( expr_tokens.size() < 2 )
                PARSER_PANIC( expr_tokens[least_prec_op], "unary expected expression" );

            create_operator_node( expr_tokens[least_prec_op], tree );
            auto& op_node = std::get<OperatorNode>( tree );

            TokenIterator expr_it{ expr_tokens.begin()._Ptr + 1, expr_tokens.end()._Ptr };
            op_node.sides.resize( 2 );
            op_node.sides[0].emplace<Value>();
            parse_expression( ExtractType::DEFAULT, expr_it, op_node.sides[1] );
        }
        else
        {
            if ( (Int) expr_tokens.size() < least_prec_op + 2 )
                PARSER_PANIC( expr_tokens[least_prec_op], "operator expected expression" );

            Vector<Token> left;
            left.insert( left.end(), expr_tokens.begin(), expr_tokens.begin() + least_prec_op );
            Vector<Token> right;
            right.insert( right.end(), expr_tokens.begin() + ( least_prec_op + 1 ), expr_tokens.end() );

            auto& op = expr_tokens[least_prec_op];
            if ( op.value == op_expr_opn )
                expression_complex_expr( left, op, right, tree );

            else if ( op.value == op_scope_opn )
                expression_complex_scope( left, op, right, tree );

            else if ( op.value == op_array_opn )
                expression_complex_array( left, op, right, tree );

            else if ( op.value == op_link )
                expression_complex_link( left, op, right, tree );

            else
                expression_complex_default( left, op, right, tree );
        }
    }
    else
        expression_pure( expr_tokens, tree );
}

void dawn::Parser::expression_extract( ExtractType type, TokenIterator& it, Vector<Token>& tokens )
{
    const TokenIterator first_it = it;
    Int last_line = it->location.line;
    Int expr_depth = 0;
    Bool in_lambda = false;

    for ( ; it.valid(); ++it )
    {
        if ( expr_depth == 0 )
        {
            if ( type == ExtractType::NEW_LINE )
            {
                if ( it->location.line != last_line )
                    break;
            }
            else if ( type == ExtractType::SPLITTER )
            {
                if ( it->value == op_split )
                {
                    ++it;
                    break;
                }
            }
            else if ( type == ExtractType::SCOPE_START )
            {
                if ( it->value == op_scope_opn )
                {
                    if ( it == first_it )
                        break;

                    TokenIterator it_before = it;
                    --it_before;
                    if ( it_before->value != op_lambda &&
                        it_before->type != TokenType::TYPE )
                        break;
                }
            }
        }

        expr_depth += token_depth( *it, in_lambda );
        if ( expr_depth < 0 )
            PARSER_PANIC( *it, "unexpected expression end" );

        tokens.push_back( *it );
        last_line = it->location.line;
    }

    if ( expr_depth > 0 )
        PARSER_PANIC( *it, "expected expression end" );
}

void dawn::Parser::expression_precedence( Vector<Token>& tokens, Int& index, Bool& unary )
{
    Int least_precedence = -1;
    Bool was_op = true;
    Int expr_depth = 0;
    Bool in_lambda = false;

    for ( Int i = 0; i < (Int) tokens.size(); i++ )
    {
        auto& token = tokens[i];

        Bool is_op = precedences.contains( token.value );
        Bool is_unary = was_op && dawn::is_unary( token );

        if ( index < 0 && is_op )
        {
            index = i;
            unary = is_unary;
        }

        if ( expr_depth == 0 && is_op )
        {
            const Int prec = is_unary ? precedences.find( prec_unary )->second : precedences.at( token.value );
            if ( is_unary && prec > least_precedence || !is_unary && prec >= least_precedence )
            {
                least_precedence = prec;
                index = i;
                unary = is_unary;
            }
        }
        was_op = is_op;

        expr_depth += token_depth( token, in_lambda );
        if ( expr_depth < 0 )
            PARSER_PANIC( token, "unexpected expression end" );
    }
}

void dawn::Parser::expression_complex_expr( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( op, "right expression expected" );

    if ( right.back().value != op_expr_cls )
        PARSER_PANIC( right.back(), "expected call close" );

    right.pop_back();

    if ( left.size() == 1 && is_custom_type( left.front().value ) )
    {
        auto& node = tree.emplace<StructNode>( op.location );
        node.type_id = IDSystem::get( left.front().value );
        auto& args = node.init.emplace<StructNode::ListInit>().args;

        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        while ( right_it.valid() )
            parse_expression( ExtractType::SPLITTER, right_it, args.emplace_back() );
    }
    else if ( !left.empty() )
    {
        auto& node = tree.emplace<CallNode>( op.location );

        TokenIterator left_it{ left.begin()._Ptr, left.end()._Ptr };
        node.left_expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, left_it, *node.left_expr );

        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        while ( right_it.valid() )
            parse_expression( ExtractType::SPLITTER, right_it, node.args.emplace_back() );
    }
    else
    {
        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        parse_expression( ExtractType::DEFAULT, right_it, tree );
    }
}

void dawn::Parser::expression_complex_scope( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( op, "right scope expression expected" );

    if ( right.back().value != op_scope_cls )
        PARSER_PANIC( right.back(), "expected scope close" );

    right.pop_back();

    if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        Map<ID, Node> struct_args;
        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        while ( right_it.valid() )
        {
            if ( right_it->type != TokenType::NAME )
                PARSER_PANIC( *right_it, "expected field init name" );

            ID name_id = IDSystem::get( right_it->value );
            if ( struct_args.contains( name_id ) )
                PARSER_PANIC( *right_it, "argument [", right_it->value, "] already passed" );
            ++right_it;

            if ( right_it->value != op_assign )
                PARSER_PANIC( *right_it, "expected assign operator" );
            ++right_it;

            auto& arg = struct_args[name_id];
            parse_expression( ExtractType::SPLITTER, right_it, arg );
        }

        auto& node = tree.emplace<StructNode>( op.location );
        node.type_id = IDSystem::get( left.front().value );
        node.init.emplace<StructNode::NamedInit>().args = struct_args;
    }
    else if ( left.size() >= 2 && left.front().value == op_lambda && left.back().value == op_lambda )
    {
        left.erase( left.begin() );
        left.pop_back();

        auto& node = tree.emplace<LambdaNode>( op.location );
        auto& func = ( node.func_value = Value{ FunctionValue{}, op.location } ).as_function()
            .data.emplace<FunctionValue::AsLambda>()
            .func.emplace<DFunction>();

        Set<ID> args;
        TokenIterator left_it{ left.begin()._Ptr, left.end()._Ptr };
        while ( left_it.valid() )
        {
            auto& arg = func.args.emplace_back();

            if ( left_it->type != TokenType::TYPE )
                PARSER_PANIC( *left_it, "expected argument type" );
            arg.type.type_id = IDSystem::get( left_it->value );
            ++left_it;

            if ( left_it->value == vr_variable )
            {
                arg.type.kind = VarKind::VARIABLE;
                ++left_it;
            }
            else if ( left_it->value == vr_reference )
            {
                arg.type.kind = VarKind::REFERENCE;
                ++left_it;
            }
            else
                arg.type.kind = VarKind::CONSTANT;

            if ( left_it->type != TokenType::NAME )
                PARSER_PANIC( *left_it, "expected arg name" );
            arg.id = IDSystem::get( left_it->value );

            if ( args.contains( arg.id ) )
                PARSER_PANIC( *left_it, "argument [", left_it->value, "] already defined" );
            args.insert( arg.id );
            ++left_it;

            if ( left_it.valid() )
            {
                if ( left_it->value != op_split )
                    PARSER_PANIC( *left_it, "expected split or lambda close" );
                ++left_it;
            }
        }

        Token left_scope;
        left_scope.value = op_scope_opn;
        left_scope.type = TokenType::OPERATOR;
        left_scope.location = LOCATION_NONE;
        right.insert( right.begin(), left_scope );

        Token right_scope;
        right_scope.value = op_scope_cls;
        right_scope.type = TokenType::OPERATOR;
        right_scope.location = LOCATION_NONE;
        right.push_back( right_scope );

        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        parse_scope( right_it, func.body );
    }
    else
        PARSER_PANIC( op, "scope is not an expression" );
}

void dawn::Parser::expression_complex_array( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( op, "right array expression expected" );

    if ( right.back().value != op_array_cls )
        PARSER_PANIC( right.back(), "expected array close" );

    right.pop_back();

    if ( left.empty() )
    {
        ArrayNode::ListInit init{};
        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        while ( right_it.valid() )
            parse_expression( ExtractType::SPLITTER, right_it, init.elements.emplace_back() );

        auto& node = tree.emplace<ArrayNode>( op.location );
        node.init = init;
    }
    else if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        ArrayNode::SizedInit init{};
        init.type_id = IDSystem::get( left.front().value );
        init.size_expr = node_pool().new_register();

        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        parse_expression( ExtractType::DEFAULT, right_it, *init.size_expr );

        auto& node = tree.emplace<ArrayNode>( op.location );
        node.init = init;
    }
    else
    {
        auto& node = tree.emplace<IndexNode>( op.location );

        TokenIterator left_it{ left.begin()._Ptr, left.end()._Ptr };
        node.left_expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, left_it, *node.left_expr );

        TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
        node.expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, right_it, *node.expr );
    }
}

void dawn::Parser::expression_complex_link( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
{
    if ( left.size() != 1 )
        PARSER_PANIC( !left.empty() ? left.back() : Token{}, "enum name expected" );

    auto& enum_token = left.back();
    if ( enum_token.type != TokenType::TYPE )
        PARSER_PANIC( enum_token, "enum name expected but got [", enum_token.any_value(), "]" );

    if ( right.size() != 1 )
        PARSER_PANIC( !right.empty() ? right.back() : Token{}, "enum field name expected" );

    auto& enum_field = right.back();
    if ( enum_field.type != TokenType::NAME )
        PARSER_PANIC( enum_field, "enum field name expected but got [", enum_field.any_value(), "]" );

    auto& node = tree.emplace<EnumNode>( op.location );
    node.type_id = IDSystem::get( enum_token.value );
    node.key_id = IDSystem::get( enum_field.value );
}

void dawn::Parser::expression_complex_default( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
{
    TokenIterator left_it{ left.begin()._Ptr, left.end()._Ptr };
    Node left_expr;
    parse_expression( ExtractType::DEFAULT, left_it, left_expr );

    TokenIterator right_it{ right.begin()._Ptr, right.end()._Ptr };
    Node right_expr;
    parse_expression( ExtractType::DEFAULT, right_it, right_expr );

    if ( op.value == op_access )
    {
        if ( right.size() != 1 || right.front().type != TokenType::NAME )
            PARSER_PANIC( op, "access operator expects name on the right" );

        auto& ac_node = tree.emplace<AccessNode>( op.location );
        ac_node.left_expr = node_pool().new_register();
        *ac_node.left_expr = left_expr;
        ac_node.right_id = IDSystem::get( right.front().value );
    }
    else if ( is_op( op.value ) )
    {
        create_operator_node( op, tree );
        auto& op_node = std::get<OperatorNode>( tree );
        op_node.sides.emplace_back( left_expr );
        op_node.sides.emplace_back( right_expr );
    }
    else
    {
        create_assign_node( op, tree );
        auto& as_node = std::get<AssignNode>( tree );
        as_node.sides.emplace_back( left_expr );
        as_node.sides.emplace_back( right_expr );
    }
}

void dawn::Parser::expression_pure( Vector<Token>& tokens, Node& tree )
{
    if ( tokens.empty() )
    {
        tree.emplace<Value>();
    }
    else if ( tokens.size() == 1 )
    {
        expression_single( tokens[0], tree );
    }
    else
        PARSER_PANIC( tokens[0], "expected pure expression" );
}

void dawn::Parser::expression_single( Token const& token, Node& tree )
{
    switch ( token.type )
    {
    case TokenType::INTEGER:
    case TokenType::FLOAT:
    case TokenType::CHAR:
    case TokenType::STRING:
        expression_single_literal( token, tree );
        break;

    case TokenType::KEYWORD:
        expression_single_keyword( token, tree );
        break;

    case TokenType::TYPE:
        expression_single_type( token, tree );
        break;

    case TokenType::NAME:
        expression_single_identifier( token, tree );
        break;

    case TokenType::OPERATOR:
        PARSER_PANIC( token, "single operator is not an expression" );
    }
}

void dawn::Parser::expression_single_literal( Token const& token, Node& tree )
{
    if ( token.type == TokenType::INTEGER )
    {
        tree.emplace<Value>( (Int) std::stoll( token.literal ), token.location );
    }
    else if ( token.type == TokenType::FLOAT )
    {
        tree.emplace<Value>( (Float) std::stod( token.literal ), token.location );
    }
    else if ( token.type == TokenType::CHAR )
    {
        tree.emplace<Value>( (Char) token.literal[0], token.location );
    }
    else if ( token.type == TokenType::STRING )
    {
        tree.emplace<Value>( (String const&) token.literal, token.location );
    }
    else
        PARSER_PANIC( token, "expected literal" );
}

void dawn::Parser::expression_single_keyword( Token const& token, Node& tree )
{
    if ( token.value == kw_true )
    {
        tree.emplace<Value>( (Bool) true, token.location );
    }
    else if ( token.value == kw_false )
    {
        tree.emplace<Value>( (Bool) false, token.location );
    }
    else if ( token.value == kw_self )
    {
        static const ID self_id = IDSystem::get( kw_self );
        tree.emplace<IdentifierNode>( token.location ).id = self_id;
    }
    else
        PARSER_PANIC( token, "keyword [", token.value, "] is not an expression" );
}

void dawn::Parser::expression_single_type( Token const& token, Node& tree )
{
    if ( token.value == tp_void ||
        token.value == tp_bool ||
        token.value == tp_int ||
        token.value == tp_float ||
        token.value == tp_char ||
        token.value == tp_string ||
        token.value == tp_range ||
        token.value == tp_function ||
        token.value == tp_array )
        tree.emplace<IdentifierNode>( token.location ).id = IDSystem::get( token.value );
    else
        PARSER_PANIC( token, "type [", token.value, "] is not an expression" );
}

void dawn::Parser::expression_single_identifier( Token const& token, Node& tree )
{
    tree.emplace<IdentifierNode>( token.location ).id = IDSystem::get( token.value );
}

void dawn::Parser::parse_scope( TokenIterator& it, Scope& scope )
{
    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    Set<ID> vars;
    while ( it->value != op_scope_cls )
    {
        if ( is_variable( it ) )
        {
            auto& node = scope.instr.emplace_back().emplace<VariableNode>( it->location );
            parse_variable( it, node.var );

            if ( vars.contains( node.var.id ) )
                PARSER_PANIC( *it, "variable [", IDSystem::get( node.var.id ), "] already defined" );
            vars.insert( node.var.id );
        }
        else if ( it->value == kw_if )
        {
            auto& instr = scope.instr.emplace_back();
            scope_if( it, instr );
        }
        else if ( it->value == kw_switch )
        {
            auto& instr = scope.instr.emplace_back();
            scope_switch( it, instr );
        }
        else if ( it->value == kw_for )
        {
            auto& instr = scope.instr.emplace_back();
            scope_for( it, instr );
        }
        else if ( it->value == kw_while )
        {
            auto& instr = scope.instr.emplace_back();
            scope_while( it, instr );
        }
        else if ( it->value == kw_loop )
        {
            auto& instr = scope.instr.emplace_back();
            scope_loop( it, instr );
        }
        else if ( it->value == kw_return )
        {
            auto& instr = scope.instr.emplace_back();
            scope_return( it, instr );
        }
        else if ( it->value == kw_break )
        {
            auto& instr = scope.instr.emplace_back();
            scope_break( it, instr );
        }
        else if ( it->value == kw_continue )
        {
            auto& instr = scope.instr.emplace_back();
            scope_continue( it, instr );
        }
        else if ( it->value == kw_throw )
        {
            auto& instr = scope.instr.emplace_back();
            scope_throw( it, instr );
        }
        else if ( it->value == kw_try )
        {
            auto& instr = scope.instr.emplace_back();
            scope_try( it, instr );
        }
        else if ( it->value == op_scope_opn )
        {
            auto& chld_scp = scope.instr.emplace_back().emplace<Scope>();
            parse_scope( it, chld_scp );
        }
        else
        {
            auto& expr = scope.instr.emplace_back();
            parse_expression( ExtractType::NEW_LINE, it, expr );
        }
    }
    ++it;
}

void dawn::Parser::scope_return( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_return )
        PARSER_PANIC( *it, "expected return" );
    const Location return_location = it->location;
    ++it;

    auto& node = tree.emplace<ReturnNode>( it->location );
    node.expr = node_pool().new_register();
    if ( it->location.line == return_location.line )
        parse_expression( ExtractType::NEW_LINE, it, *node.expr );
    else
        node.expr->emplace<Value>();
}

void dawn::Parser::scope_break( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_break )
        PARSER_PANIC( *it, "expected break" );
    ++it;

    tree.emplace<BreakNode>( it->location );
}

void dawn::Parser::scope_continue( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_continue )
        PARSER_PANIC( *it, "expected continue" );
    ++it;

    tree.emplace<ContinueNode>( it->location );
}

void dawn::Parser::scope_throw( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_throw )
        PARSER_PANIC( *it, "expected throw" );
    ++it;

    auto& node = tree.emplace<ThrowNode>( it->location );
    node.expr = node_pool().new_register();
    parse_expression( ExtractType::NEW_LINE, it, *node.expr );
}

void dawn::Parser::scope_try( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_try )
        PARSER_PANIC( *it, "expected try" );
    ++it;

    auto& node = tree.emplace<TryNode>( it->location );

    parse_scope( it, node.try_scope );

    if ( it->value != kw_catch )
        PARSER_PANIC( *it, "expected catch" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected catch name" );
    node.catch_id = IDSystem::get( it->value );
    ++it;

    parse_scope( it, node.catch_scope );
}

void dawn::Parser::scope_if( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_if )
        PARSER_PANIC( *it, "expected if keyword" );
    ++it;

    auto& node = tree.emplace<IfNode>( it->location );

    parse_expression( ExtractType::SCOPE_START, it, node.parts.emplace_back().expr );
    parse_scope( it, node.parts.back().scope );

    while ( true )
    {
        if ( it->value == kw_elif )
        {
            ++it;
            auto& part = node.parts.emplace_back();
            parse_expression( ExtractType::SCOPE_START, it, part.expr );
            parse_scope( it, part.scope );
        }
        else if ( it->value == kw_else )
        {
            ++it;
            auto& part = node.parts.emplace_back();
            part.expr.emplace<Value>( (Bool) true, it->location );
            parse_scope( it, part.scope );
            break;
        }
        else
            break;
    }
}

void dawn::Parser::scope_switch( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_switch )
        PARSER_PANIC( *it, "expected switch keyword" );
    ++it;

    auto& node = tree.emplace<SwitchNode>( it->location );

    node.main_expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, *node.main_expr );

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    while ( it->value != op_scope_cls )
    {
        if ( it->value == kw_case )
        {
            ++it;
            auto& casee = node.cases.emplace_back();

            Vector<Token> case_tokens;
            expression_extract( ExtractType::SCOPE_START, it, case_tokens );

            TokenIterator case_it{ case_tokens.begin()._Ptr, case_tokens.end()._Ptr };
            while ( case_it.valid() )
                parse_expression( ExtractType::SPLITTER, case_it, casee.exprs.emplace_back() );

            parse_scope( it, casee.scope );
        }
        else if ( it->value == kw_default )
        {
            ++it;
            if ( node.def_scope )
                PARSER_PANIC( *it, "default already defined" );

            auto& scope = node.def_scope.emplace();
            parse_scope( it, scope );
        }
        else
            PARSER_PANIC( *it, "expected case or default" );
    }
    ++it;
}

void dawn::Parser::scope_loop( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_loop )
        PARSER_PANIC( *it, "expected loop keyword" );
    ++it;

    auto& node = tree.emplace<LoopNode>( it->location );
    parse_scope( it, node.scope );
}

void dawn::Parser::scope_while( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_while )
        PARSER_PANIC( *it, "expected while keyword" );
    ++it;

    auto& node = tree.emplace<WhileNode>( it->location );
    node.expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, *node.expr );
    parse_scope( it, node.scope );
}

void dawn::Parser::scope_for( TokenIterator& it, Node& tree )
{
    if ( it->value != kw_for )
        PARSER_PANIC( *it, "expected for keyword" );
    ++it;

    auto& node = tree.emplace<ForNode>( it->location );
    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected name" );
    node.var_id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_set )
        PARSER_PANIC( *it, "expected iterator operator" );
    ++it;

    node.expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, *node.expr );
    parse_scope( it, node.scope );
}

dawn::Bool dawn::is_unary( Token const& token )
{
    return token.value == op_add || token.value == op_sub || token.value == op_not;
}

dawn::Int dawn::token_depth( Token const& token, Bool& in_lambda )
{
    if ( token.value == op_lambda )
    {
        in_lambda = !in_lambda;
        return in_lambda ? 1 : -1;
    }

    if ( token.value == op_expr_opn || token.value == op_scope_opn || token.value == op_array_opn )
        return 1;
    else if ( token.value == op_expr_cls || token.value == op_scope_cls || token.value == op_array_cls )
        return -1;

    return 0;
}

void dawn::create_operator_node( Token const& token, Node& node )
{
    auto& op_node = node.emplace<OperatorNode>( token.location );
    op_node.type = get_op( token.value );
}

void dawn::create_assign_node( Token const& token, Node& node )
{
    auto& as_node = node.emplace<AssignNode>( token.location );

    if ( token.value == op_assign )
        as_node.type = AssignType::ASSIGN;

    else if ( token.value == op_addas )
        as_node.type = AssignType::ADD;

    else if ( token.value == op_subas )
        as_node.type = AssignType::SUB;

    else if ( token.value == op_mulas )
        as_node.type = AssignType::MUL;

    else if ( token.value == op_divas )
        as_node.type = AssignType::DIV;

    else if ( token.value == op_powas )
        as_node.type = AssignType::POW;

    else if ( token.value == op_modas )
        as_node.type = AssignType::MOD;

    else
        PARSER_PANIC( token, "unknown assign operator" );
}
