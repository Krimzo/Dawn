#include "parser.h"


dawn::Bool dawn::Module::contains_id( StringRef const& id ) const
{
    if ( std::find_if( variables.begin(), variables.end(), [&]( Variable const& var ) { return var.name.str_id == id; } ) != variables.end() )
        return true;

    if ( std::find_if( functions.begin(), functions.end(), [&]( Function const& func ) { return func.name.str_id == id; } ) != functions.end() )
        return true;

    if ( std::find_if( enums.begin(), enums.end(), [&]( Enum const& enu ) { return enu.name.str_id == id; } ) != enums.end() )
        return true;

    if ( std::find_if( structs.begin(), structs.end(), [&]( Struct const& struc ) { return struc.name.str_id == id; } ) != structs.end() )
        return true;

    return false;
}

void dawn::Parser::parse( Array<Token>& tokens, Module& module )
{
    prepare_tokens( tokens );

    auto it = tokens.begin();
    auto end = tokens.end();

    while ( it != end )
    {
        if ( it->value == kw_enum )
        {
            parse_global_enum( it, end, module );
        }
        else if ( it->value == kw_struct )
        {
            parse_global_struct( it, end, module );
        }
        else if ( it->value == kw_func )
        {
            parse_global_function( it, end, module );
        }
        else if ( it->value == kw_let || it->value == kw_var || it->value == kw_ref )
        {
            parse_global_variable( it, end, module );
        }
        else
            PARSER_PANIC( *it, L"not allowed in global scope or allowed only 1 instance of" );
    }
}

void dawn::Parser::prepare_tokens( Array<Token>& tokens )
{
    for ( Int i = 0; i < (Int) tokens.size() - 1; i++ )
    {
        if ( tokens[i].type == TokenType::NAME && tokens[i + 1].value == op_expr_opn )
            tokens[i].type = TokenType::FUNCTION;
    }
}

void dawn::Parser::parse_global_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Struct struc;
    parse_struct( it, end, struc );

    if ( module.contains_id( struc.name.str_id ) )
        PARSER_PANIC( {}, L"name [", struc.name, L"] already in use" );

    module.structs.push_back( struc );
}

void dawn::Parser::parse_global_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Enum enu;
    parse_enum( it, end, enu );

    if ( module.contains_id( enu.name.str_id ) )
        PARSER_PANIC( {}, L"name [", enu.name, L"] already in use" );

    module.enums.push_back( enu );
}

void dawn::Parser::parse_global_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Function function;
    parse_function( it, end, function );

    if ( module.contains_id( function.name.str_id ) )
        PARSER_PANIC( {}, L"name [", function.name, L"] already in use" );

    module.functions.push_back( function );
}

void dawn::Parser::parse_global_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Variable variable;
    parse_variable( it, end, variable );

    if ( module.contains_id( variable.name.str_id ) )
        PARSER_PANIC( {}, L"name [", variable.name, L"] already in use" );

    module.variables.push_back( variable );
}

void dawn::Parser::parse_type( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, String& type )
{
    if ( it->value != tp_bool
        && it->value != tp_int
        && it->value != tp_float
        && it->value != tp_char
        && it->value != tp_string
        && it->value != op_range
        && it->type != TokenType::TYPE )
        PARSER_PANIC( *it, L"invalid type" );

    type = it->value;
    ++it;
}

void dawn::Parser::parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Struct& struc )
{
    if ( it->value != kw_struct )
        PARSER_PANIC( *it, L"expected struct" );
    ++it;

    if ( !it->is_custom_type() )
        PARSER_PANIC( *it, L"expected struct name" );
    struc.name = it->value;
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, L"expected scope open" );
    ++it;

    while ( true )
    {
        if ( it->value == op_scope_cls )
        {
            ++it;
            break;
        }

        if ( it->type == TokenType::NAME )
        {
            auto find_it = std::find_if( struc.fields.begin(), struc.fields.end(), [&]( auto const& field ) { return field.name.str_id == it->value; } );
            if ( find_it != struc.fields.end() )
                PARSER_PANIC( *it, L"field [", it->value, L"] already defined" );

            auto& field = struc.fields.emplace_back();
            field.name = it->value;
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( it, end, field.expr );
            }
            else
            {
                if ( it->value != op_split )
                    PARSER_PANIC( *it, L"expected split" );
                ++it;
                field.expr = make_nothing_node();
            }
        }
        else if ( it->value == kw_func )
        {
            auto& method = struc.methods.emplace_back();
            parse_function( it, end, method );

            auto& self_var = *method.args.emplace( method.args.begin() );
            self_var.kind = VariableKind::REF;
            self_var.expr = make_nothing_node();
            self_var.name.str_id = kw_self;
        }
        else
            PARSER_PANIC( *it, L"expected field name or function" );
    }
}

void dawn::Parser::parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Enum& enu )
{
    if ( it->value != kw_enum )
        PARSER_PANIC( *it, L"expected enum" );
    ++it;

    if ( !it->is_custom_type() )
        PARSER_PANIC( *it, L"expected enum name" );
    enu.name = it->value;
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, L"expected scope open" );
    ++it;

    while ( true )
    {
        if ( it->value == op_scope_cls )
        {
            ++it;
            break;
        }

        if ( it->type == TokenType::NAME )
        {
            if ( enu.keys_expr.contains( it->value ) )
                PARSER_PANIC( *it, L"key [", it->value, L"] already in use" );

            auto& key = enu.keys_expr[it->value];
            key.name = it->value;
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( it, end, key.expr );
            }
            else
            {
                if ( it->value != op_split )
                    PARSER_PANIC( *it, L"expected split" );
                ++it;
                key.expr = make_nothing_node();
            }
        }
        else
            PARSER_PANIC( *it, L"expected key name" );
    }
}

void dawn::Parser::parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function )
{
    if ( it->value != kw_func )
        PARSER_PANIC( *it, L"expected function" );
    ++it;

    if ( it->type != TokenType::FUNCTION )
        PARSER_PANIC( *it, L"expected function name" );
    function.name = it->value;
    ++it;

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, L"expected open expr" );
    ++it;

    Set<String> args;

    while ( true )
    {
        if ( it->value == op_expr_cls )
        {
            ++it;
            break;
        }

        auto& arg = function.args.emplace_back();

        if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
            PARSER_PANIC( *it, L"expected let, var or ref keywords" );
        if ( it->value == kw_let )
            arg.kind = VariableKind::LET;
        else if ( it->value == kw_var )
            arg.kind = VariableKind::VAR;
        else
            arg.kind = VariableKind::REF;
        ++it;

        if ( it->type != TokenType::NAME )
            PARSER_PANIC( *it, L"expected arg name" );
        arg.name = it->value;
        ++it;

        if ( args.contains( arg.name.str_id ) )
            PARSER_PANIC( *it, L"argument [", arg.name, L"] already defined" );
        args.insert( arg.name.str_id );

        if ( it->value == op_split )
            ++it;
    }

    parse_scope( it, end, std::get<Scope>( function.body ) );
}

void dawn::Parser::parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable )
{
    if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
        PARSER_PANIC( *it, L"expected let, var or ref keywords" );
    if ( it->value == kw_let )
        variable.kind = VariableKind::LET;
    else if ( it->value == kw_var )
        variable.kind = VariableKind::VAR;
    else
        variable.kind = VariableKind::REF;
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, L"expected variable name" );
    variable.name = it->value;
    Int name_line = it->line_number;
    ++it;

    if ( it->line_number == name_line )
    {
        if ( it->value != op_assign )
            PARSER_PANIC( *it, L"expected variable assignment" );
        ++it;

        parse_expression( it, end, variable.expr );
    }
    else
        variable.expr = make_nothing_node();
}

void dawn::Parser::parse_expression( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it == end )
    {
        tree = make_nothing_node();
        return;
    }

    Array<Token> expr_tokens;
    expression_extract( it, end, expr_tokens );

    Int least_prec_op = -1;
    expression_precedence( expr_tokens, least_prec_op );

    if ( least_prec_op == 0 )
    {
        if ( expr_tokens.size() < 2 )
            PARSER_PANIC( *it, L"unary expected expression" );

        create_unary_node( expr_tokens.front(), tree );
        auto& un_nod = tree.as<UnaryNod>();

        auto it = expr_tokens.begin() + 1;
        parse_expression( it, expr_tokens.end(), un_nod.right );
    }
    else if ( least_prec_op > 0 )
    {
        if ( (Int) expr_tokens.size() < least_prec_op + 2 )
            PARSER_PANIC( *it, L"unary expected expression" );

        try
        {
            tree = {};
            create_operator_node( expr_tokens[least_prec_op], tree );
            auto& op_node = tree.as<OperatorNod>();

            auto it = expr_tokens.begin();
            parse_expression( it, expr_tokens.begin() + least_prec_op, op_node.left );

            it = expr_tokens.begin() + (least_prec_op + 1);
            parse_expression( it, expr_tokens.end(), op_node.right );
        }
        catch ( ... )
        {
            tree = {};
            create_assign_node( expr_tokens[least_prec_op], tree );
            auto& as_node = tree.as<AssignNod>();

            auto it = expr_tokens.begin();
            parse_expression( it, expr_tokens.begin() + least_prec_op, as_node.left );

            it = expr_tokens.begin() + (least_prec_op + 1);
            parse_expression( it, expr_tokens.end(), as_node.right );
        }
    }
    else
        expression_pure( expr_tokens, tree );
}

void dawn::Parser::expression_extract( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens )
{
    Int expr_depth = 0;
    Array<Token>::const_iterator first_it = it;
    Int last_line = it->line_number;
    for ( ; it != end; ++it )
    {
        if ( it->line_number != last_line )
            break;

        if ( it->value == op_scope_opn )
        {
            if ( first_it == it )
                break;

            if ( (it - 1)->type != TokenType::TYPE )
                break;
        }

        if ( it->value == op_expr_opn || it->value == op_scope_opn || it->value == op_array_opn )
        {
            ++expr_depth;
        }
        else if ( it->value == op_expr_cls || it->value == op_scope_cls || it->value == op_array_cls )
        {
            --expr_depth;
            if ( expr_depth < 0 )
                break;
        }

        if ( expr_depth == 0 && it->value == op_split )
        {
            ++it;
            break;
        }

        tokens.push_back( *it );
        last_line = it->line_number;
    }
    if ( expr_depth > 0 )
        PARSER_PANIC( *it, L"expected expression end" );
}

void dawn::Parser::expression_precedence( Array<Token> const& tokens, Int& index )
{
    Int expr_depth = 0;
    Bool was_op = true;
    Int least_precedence = -1;
    for ( Int i = 0; i < (Int) tokens.size(); i++ )
    {
        auto& token = tokens[i];

        if ( token.value == op_expr_opn || token.value == op_scope_opn || token.value == op_array_opn )
        {
            ++expr_depth;
        }
        else if ( token.value == op_expr_cls || token.value == op_scope_cls || token.value == op_array_cls )
        {
            --expr_depth;
            if ( expr_depth < 0 )
                PARSER_PANIC( token, L"expected expression end" );
        }
        if ( expr_depth != 0 )
            continue;

        Bool is_op = precedences.contains( token.value );
        if ( is_op && index < 0 )
            index = i;

        if ( is_op && was_op )
            continue;

        was_op = is_op;
        if ( !is_op )
            continue;

        Int prec = precedences.at( token.value );
        if ( prec < least_precedence )
            continue;

        least_precedence = prec;
        index = i;
    }
}

void dawn::Parser::expression_pure( Array<Token> const& tokens, Node& tree )
{
    if ( tokens.empty() )
    {
        tree = make_nothing_node();
        return;
    }

    if ( tokens.size() == 1 )
    {
        expression_single( tokens[0], tree );
    }
    else if ( tokens.front().type == TokenType::TYPE )
    {
        expression_type( tokens, tree );
    }
    else if ( tokens.front().type == TokenType::FUNCTION )
    {
        expression_function( tokens, tree );
    }
    else if ( tokens.front().value == op_expr_opn )
    {
        if ( tokens.back().value != op_expr_cls )
            PARSER_PANIC( tokens.back(), L"expected expression close" );

        auto begin = tokens.begin() + 1;
        auto end = tokens.begin() + tokens.size() - 1;
        return parse_expression( begin, end, tree );
    }
    else if ( tokens.front().value == op_array_opn )
    {
        expression_type_array( tokens, tree );
    }
    else
        PARSER_PANIC( tokens[0], L"unknown expression token" );
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
        PARSER_PANIC( token, L"type is not an expression" );

    case TokenType::FUNCTION:
        PARSER_PANIC( token, L"function is not an expression" );

    case TokenType::NAME:
        expression_single_identifier( token, tree );
        break;

    case TokenType::OPERATOR:
        PARSER_PANIC( token, L"operator is not an expression" );
    }
}

void dawn::Parser::expression_single_literal( Token const& token, Node& tree )
{
    if ( token.type == TokenType::INTEGER )
    {
        tree = make_int_node( std::stoll( token.lit_val ) );
    }
    else if ( token.type == TokenType::FLOAT )
    {
        tree = make_float_node( std::stod( token.lit_val ) );
    }
    else if ( token.type == TokenType::CHAR )
    {
        tree = make_char_node( token.lit_val[0] );
    }
    else if ( token.type == TokenType::STRING )
    {
        tree = make_string_node( token.lit_val );
    }
    else
        PARSER_PANIC( token, L"expected literal" );
}

void dawn::Parser::expression_single_keyword( Token const& token, Node& tree )
{
    if ( token.value == kw_true )
    {
        tree = make_bool_node( true );
    }
    else if ( token.value == kw_false )
    {
        tree = make_bool_node( false );
    }
    else if ( token.value == kw_self )
    {
        tree.store<IdentifierNod>().name.str_id = kw_self;
    }
    else
        PARSER_PANIC( token, L"keyword [", token.value, L"] is not an expression" );
}

void dawn::Parser::expression_single_identifier( Token const& token, Node& tree )
{
    tree.store<IdentifierNod>().name = token.value;
}

void dawn::Parser::expression_type( Array<Token> const& tokens, Node& tree )
{
    auto& second_token = tokens[1];
    if ( second_token.value == op_expr_opn )
    {
        expression_type_cast( tokens, tree );
    }
    else if ( second_token.value == op_scope_opn )
    {
        expression_type_make( tokens, tree );
    }
    else if ( second_token.value == op_array_opn )
    {
        expression_type_array( tokens, tree );
    }
    else
        PARSER_PANIC( second_token, L"unknown type usage" );
}

void dawn::Parser::expression_type_cast( Array<Token> const& tokens, Node& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    auto& node = tree.store<CastNod>();

    parse_type( it, tokens.end(), node.type.str_id );

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, L"expected expression open" );
    ++it;

    parse_expression( it, tokens.end(), node.expr );

    if ( it->value != op_expr_cls )
        PARSER_PANIC( *it, L"expected expression close" );
    ++it;
}

void dawn::Parser::expression_type_make( Array<Token> const& tokens, Node& tree )
{
    Array<Token>::const_iterator it = tokens.begin();

    if ( !it->is_custom_type() )
        PARSER_PANIC( *it, L"expected custom type" );
    String type = it->value;
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, L"expected expression open" );
    ++it;

    Map<String, Node> args;
    Opt<String> key;
    while ( true )
    {
        if ( it->value == op_scope_cls )
        {
            ++it;
            break;
        }

        if ( it->type != TokenType::NAME )
            PARSER_PANIC( *it, L"expected field name" );

        String name = it->value;
        if ( args.contains( name ) )
            PARSER_PANIC( *it, L"argument [" + name + L"] already passed" );
        ++it;

        if ( args.empty() && it->value == op_scope_cls )
        {
            key = name;
            ++it;
            break;
        }

        if ( it->value != op_link )
            PARSER_PANIC( *it, L"expected bind operator" );
        ++it;

        parse_expression( it, tokens.end(), args[name] );
    }

    if ( key )
    {
        auto& node = tree.store<EnumNod>();
        node.type = type;
        node.key = *key;
    }
    else
    {
        auto& node = tree.store<StructNod>();
        node.type = type;
        node.args = args;
    }
}

void dawn::Parser::expression_type_array( Array<Token> const& tokens, Node& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    auto& node = tree.store<ArrayNod>();

    if ( it->value == op_array_opn )
    {
        ++it;

        while ( true )
        {
            if ( it->value == op_array_cls )
                break;

            parse_expression( it, tokens.end(), node.LIST_list.emplace_back() );
        }

        node.init_type = ArrayNod::InitType::LIST;
    }
    else
    {
        String val_type;
        parse_type( it, tokens.end(), val_type );

        node.SIZE_value_expr = make_def_type_expr( val_type );

        if ( it->value != op_array_opn )
            PARSER_PANIC( *it, L"expected array size expression" );
        ++it;

        parse_expression( it, tokens.end(), node.SIZE_size_expr );

        node.init_type = ArrayNod::InitType::SIZE;
    }

    if ( it->value != op_array_cls )
        PARSER_PANIC( *it, L"expected array end" );
}

void dawn::Parser::expression_function( Array<Token> const& tokens, Node& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    auto& node = tree.store<FunctionNod>();

    if ( it->type != TokenType::FUNCTION )
        PARSER_PANIC( *it, L"expected function name" );
    node.name = it->value;
    ++it;

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, L"expected function open" );
    ++it;

    while ( true )
    {
        if ( it->value == op_expr_cls )
            break;

        parse_expression( it, tokens.end(), node.args.emplace_back() );
    }

    if ( it->value != op_expr_cls )
        PARSER_PANIC( *it, L"expected function close" );
    ++it;
}

void dawn::Parser::parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope )
{
    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, L"expected scope open" );
    ++it;

    Set<String> vars;

    while ( true )
    {
        if ( it->value == op_scope_cls )
        {
            ++it;
            break;
        }

        if ( it->value == kw_let || it->value == kw_var || it->value == kw_ref )
        {
            auto& node = scope.instr.emplace_back().store<VariableNod>();
            parse_variable( it, end, node.var );

            if ( vars.contains( node.var.name.str_id ) )
                PARSER_PANIC( *it, L"variable [", node.var.name, L"] already defined" );
            vars.insert( node.var.name.str_id );
        }
        else if ( it->value == kw_if )
        {
            auto& instr = scope.instr.emplace_back();
            scope_if( it, end, instr );
        }
        else if ( it->value == kw_switch )
        {
            auto& instr = scope.instr.emplace_back();
            scope_switch( it, end, instr );
        }
        else if ( it->value == kw_for )
        {
            auto& instr = scope.instr.emplace_back();
            scope_for( it, end, instr );
        }
        else if ( it->value == kw_while )
        {
            auto& instr = scope.instr.emplace_back();
            scope_while( it, end, instr );
        }
        else if ( it->value == kw_loop )
        {
            auto& instr = scope.instr.emplace_back();
            scope_loop( it, end, instr );
        }
        else if ( it->value == kw_return )
        {
            auto& instr = scope.instr.emplace_back();
            scope_return( it, end, instr );
        }
        else if ( it->value == kw_break )
        {
            auto& instr = scope.instr.emplace_back();
            scope_break( it, end, instr );
        }
        else if ( it->value == kw_continue )
        {
            auto& instr = scope.instr.emplace_back();
            scope_continue( it, end, instr );
        }
        else if ( it->value == op_scope_opn )
        {
            auto& chld_scp = scope.instr.emplace_back().store<Scope>();
            parse_scope( it, end, chld_scp );
        }
        else
        {
            auto& expr = scope.instr.emplace_back();
            parse_expression( it, end, expr );
        }
    }
}

void dawn::Parser::scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_return )
        PARSER_PANIC( *it, L"expected return" );
    Int last_line = it->line_number;
    ++it;

    auto& node = tree.store<ReturnNod>();

    if ( it->line_number == last_line )
    {
        parse_expression( it, end, node.expr );
    }
    else
        node.expr = make_nothing_node();
}

void dawn::Parser::scope_break( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_break )
        PARSER_PANIC( *it, L"expected break" );
    ++it;

    tree.store<BreakNod>();
}

void dawn::Parser::scope_continue( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_continue )
        PARSER_PANIC( *it, L"expected continue" );
    ++it;

    tree.store<ContinueNod>();
}

void dawn::Parser::scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_if )
        PARSER_PANIC( *it, L"expected if keyword" );
    ++it;

    auto& node = tree.store<IfNod>();

    parse_expression( it, end, node.if_part.expr );
    parse_scope( it, end, node.if_part.scope );

    while ( true )
    {
        if ( it->value == kw_elif )
        {
            ++it;
            auto& part = node.elif_parts.emplace_back();
            parse_expression( it, end, part.expr );
            parse_scope( it, end, part.scope );
        }
        else if ( it->value == kw_else )
        {
            ++it;
            auto& part = node.else_part.emplace();
            parse_scope( it, end, part.scope );
            break;
        }
        else
            break;
    }
}

void dawn::Parser::scope_switch( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_switch )
        PARSER_PANIC( *it, L"expected switch keyword" );
    ++it;

    auto& node = tree.store<SwitchNod>();

    parse_expression( it, end, node.main_expr );

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, L"expected scope open" );
    ++it;

    while ( true )
    {
        if ( it->value == op_scope_cls )
        {
            ++it;
            break;
        }

        if ( it->value == kw_case )
        {
            ++it;

            auto& casee = node.cases.emplace_back();
            while ( true )
            {
                if ( it->value == op_scope_opn )
                    break;

                parse_expression( it, end, casee.exprs.emplace_back() );
            }

            parse_scope( it, end, casee.scope );
        }
        else if ( it->value == kw_default )
        {
            ++it;

            if ( node.def_scope )
                PARSER_PANIC( *it, L"default already defined" );

            auto& scope = node.def_scope.emplace();
            parse_scope( it, end, scope );
        }
        else
            PARSER_PANIC( *it, L"expected case or default" );
    }
}

void dawn::Parser::scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_loop )
        PARSER_PANIC( *it, L"expected loop keyword" );
    ++it;

    auto& node = tree.store<LoopNod>();
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_while )
        PARSER_PANIC( *it, L"expected while keyword" );
    ++it;

    auto& node = tree.store<WhileNod>();
    parse_expression( it, end, node.expr );
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_for )
        PARSER_PANIC( *it, L"expected for keyword" );
    ++it;

    auto& node = tree.store<ForNod>();

    if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
        PARSER_PANIC( *it, L"expected let, var or ref keywords" );
    if ( it->value == kw_let )
        node.var.kind = VariableKind::LET;
    else if ( it->value == kw_var )
        node.var.kind = VariableKind::VAR;
    else
        node.var.kind = VariableKind::REF;
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, L"expected name" );
    node.var.name = it->value;
    ++it;

    if ( it->value != op_link )
        PARSER_PANIC( *it, L"expected link operator" );
    ++it;

    parse_expression( it, end, node.expr );
    parse_scope( it, end, node.scope );
}

void dawn::create_unary_node( Token const& token, Node& node )
{
    auto& un_nod = node.store<UnaryNod>();

    if ( token.value == op_add )
        un_nod.type = UnaryType::PLUS;

    else if ( token.value == op_sub )
        un_nod.type = UnaryType::MINUS;

    else if ( token.value == op_not )
        un_nod.type = UnaryType::NOT;

    else if ( token.value == op_range )
        un_nod.type = UnaryType::RANGE;

    else
        PARSER_PANIC( token, L"unknown operator" );
}

void dawn::create_operator_node( Token const& token, Node& node )
{
    auto& op_nod = node.store<OperatorNod>();

    if ( token.value == op_access )
        op_nod.type = OperatorType::ACCESS;

    else if ( token.value == op_range )
        op_nod.type = OperatorType::RANGE;

    else if ( token.value == op_pow )
        op_nod.type = OperatorType::POW;

    else if ( token.value == op_mod )
        op_nod.type = OperatorType::MOD;

    else if ( token.value == op_mul )
        op_nod.type = OperatorType::MUL;

    else if ( token.value == op_div )
        op_nod.type = OperatorType::DIV;

    else if ( token.value == op_add )
        op_nod.type = OperatorType::ADD;

    else if ( token.value == op_sub )
        op_nod.type = OperatorType::SUB;

    else if ( token.value == op_less )
        op_nod.type = OperatorType::LESS;

    else if ( token.value == op_great )
        op_nod.type = OperatorType::GREAT;

    else if ( token.value == op_lesseq )
        op_nod.type = OperatorType::LESS_EQ;

    else if ( token.value == op_greateq )
        op_nod.type = OperatorType::GREAT_EQ;

    else if ( token.value == op_eq )
        op_nod.type = OperatorType::EQ;

    else if ( token.value == op_neq )
        op_nod.type = OperatorType::NOT_EQ;

    else if ( token.value == op_and )
        op_nod.type = OperatorType::AND;

    else if ( token.value == op_or )
        op_nod.type = OperatorType::OR;

    else
        PARSER_PANIC( token, L"unknown operator" );
}

void dawn::create_assign_node( Token const& token, Node& node )
{
    auto& as_nod = node.store<AssignNod>();
    if ( token.value == op_assign )
        as_nod.type = AssignType::ASSIGN;

    else if ( token.value == op_addas )
        as_nod.type = AssignType::ADD;

    else if ( token.value == op_subas )
        as_nod.type = AssignType::SUB;

    else if ( token.value == op_mulas )
        as_nod.type = AssignType::MUL;

    else if ( token.value == op_divas )
        as_nod.type = AssignType::DIV;

    else if ( token.value == op_powas )
        as_nod.type = AssignType::POW;

    else if ( token.value == op_modas )
        as_nod.type = AssignType::MOD;

    else
        PARSER_PANIC( token, L"unknown operator" );
}

dawn::Node dawn::make_def_type_expr( StringRef const& type )
{
    if ( type == tp_bool )
        return make_bool_node( {} );

    else if ( type == tp_int )
        return make_int_node( {} );

    else if ( type == tp_float )
        return make_float_node( {} );

    else if ( type == tp_char )
        return make_char_node( {} );

    else if ( type == tp_string )
        return make_string_node( {} );

    else if ( type == op_range )
        return make_value_node( Value{ RangeVal{} } );

    else
        return make_nothing_node();
}
