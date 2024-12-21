#include "parser.h"


dawn::Bool dawn::Module::contains_id( Int id ) const
{
    if ( std::find_if( variables.begin(), variables.end(), [&]( Variable const& var ) { return var.id == id; } ) != variables.end() )
        return true;

    if ( std::find_if( functions.begin(), functions.end(), [&]( Function const& func ) { return func.id == id; } ) != functions.end() )
        return true;

    if ( std::find_if( enums.begin(), enums.end(), [&]( Enum const& enu ) { return enu.id == id; } ) != enums.end() )
        return true;

    if ( std::find_if( structs.begin(), structs.end(), [&]( Struct const& struc ) { return struc.id == id; } ) != structs.end() )
        return true;

    return false;
}

void dawn::Parser::parse( Array<Token>& tokens, Module& module )
{
    for ( auto it = tokens.begin(); it != tokens.end(); )
    {
        if ( it->value == kw_import )
        {
            parse_import( it, tokens.end(), module );
        }
        else if ( it->value == kw_struct )
        {
            parse_global_struct( it, tokens.end(), module );
        }
        else if ( it->value == kw_enum )
        {
            parse_global_enum( it, tokens.end(), module );
        }
        else if ( it->value == kw_func )
        {
            parse_global_function( it, tokens.end(), module );
        }
        else if ( it->value == kw_let || it->value == kw_var || it->value == kw_ref )
        {
            parse_global_variable( it, tokens.end(), module );
        }
        else
            PARSER_PANIC( *it, "not allowed in global scope or allowed only 1 instance of" );
    }
}

void dawn::Parser::parse_import( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    if ( it->value != kw_import )
        PARSER_PANIC( *it, "expected import keyword" );
    ++it;

    if ( it->type != TokenType::STRING )
        PARSER_PANIC( *it, "expected import path" );
    module.imports.insert( fs::absolute( it->lit_val ).string() );
    ++it;
}

void dawn::Parser::parse_global_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Struct struc;
    parse_struct( it, end, struc );

    if ( module.contains_id( struc.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( struc.id ), "] already in use" );

    module.structs.push_back( struc );
}

void dawn::Parser::parse_global_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Enum enu;
    parse_enum( it, end, enu );

    if ( module.contains_id( enu.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( enu.id ), "] already in use" );

    module.enums.push_back( enu );
}

void dawn::Parser::parse_global_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Function function;
    parse_function( it, end, function );

    if ( module.contains_id( function.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( function.id ), "] already in use" );

    module.functions.push_back( function );
}

void dawn::Parser::parse_global_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Variable variable;
    parse_variable( it, end, variable );

    if ( module.contains_id( variable.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( variable.id ), "] already in use" );

    module.variables.push_back( variable );
}

void dawn::Parser::parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Struct& struc )
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
            Int name_id = IDSystem::get( it->value );

            auto find_it = std::find_if( struc.fields.begin(), struc.fields.end(), [&]( auto const& field ) { return field.id == name_id; } );
            if ( find_it != struc.fields.end() )
                PARSER_PANIC( *it, "field [", it->value, "] already defined" );

            auto& field = struc.fields.emplace_back();
            field.id = name_id;
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( ExtractType::NEW_LINE, it, end, field.expr );
            }
            else
                field.expr = make_nothing_node();
        }
        else if ( it->value == kw_func )
        {
            auto& method = struc.methods.emplace_back();
            parse_function( it, end, method );

            auto& self_var = *method.args.emplace( method.args.begin() );
            self_var.kind = VariableKind::REF;
            self_var.id = IDSystem::get( (String) kw_self );
        }
        else if ( it->value == kw_oper )
        {
            auto& op = struc.methods.emplace_back();
            parse_operator( it, end, op );

            auto& self_var = *op.args.emplace( op.args.begin() );
            self_var.kind = VariableKind::REF;
            self_var.id = IDSystem::get( (String) kw_self );
        }
        else
            PARSER_PANIC( *it, "expected field name or function" );
    }
    ++it;
}

void dawn::Parser::parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Enum& enu )
{
    if ( it->value != kw_enum )
        PARSER_PANIC( *it, "expected enum" );
    ++it;

    if ( !is_custom_type( it->value ) )
        PARSER_PANIC( *it, "expected enum name" );
    enu.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    while ( it->value != op_scope_cls )
    {
        if ( it->type == TokenType::NAME )
        {
            Int name_id = IDSystem::get( it->value );

            if ( enu.keys_expr.contains( name_id ) )
                PARSER_PANIC( *it, "key [", it->value, "] already in use" );

            auto& expr = enu.keys_expr[name_id];
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( ExtractType::NEW_LINE, it, end, expr );
            }
            else
                expr = make_nothing_node();
        }
        else
            PARSER_PANIC( *it, "expected key name" );
    }
    ++it;
}

void dawn::Parser::parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function )
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

    Set<Int> args;
    while ( it->value != op_expr_cls )
    {
        auto& arg = function.args.emplace_back();

        if ( it->value == kw_let )
            arg.kind = VariableKind::LET;
        else if ( it->value == kw_var )
            arg.kind = VariableKind::VAR;
        else if ( it->value == kw_ref )
            arg.kind = VariableKind::REF;
        else
            PARSER_PANIC( *it, "expected let, var or ref keywords" );
        ++it;

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

    parse_scope( it, end, std::get<Scope>( function.body ) );
}

void dawn::Parser::parse_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& operat )
{
    if ( it->value != kw_oper )
        PARSER_PANIC( *it, "expected opert" );
    ++it;

    if ( it->type != TokenType::OPERATOR )
        PARSER_PANIC( *it, "expected operator" );
    operat.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_expr_opn )
        PARSER_PANIC( *it, "expected open expr" );
    ++it;

    Set<Int> args;
    while ( it->value != op_expr_cls )
    {
        auto& arg = operat.args.emplace_back();

        if ( it->value == kw_let )
            arg.kind = VariableKind::LET;
        else if ( it->value == kw_var )
            arg.kind = VariableKind::VAR;
        else if ( it->value == kw_ref )
            arg.kind = VariableKind::REF;
        else
            PARSER_PANIC( *it, "expected let, var or ref keywords" );
        ++it;

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

    switch ( args.size() )
    {
    case 0:
        if ( operat.id != IDSystem::get( (String) op_add )
            && operat.id != IDSystem::get( (String) op_sub ) )
            PARSER_PANIC( *it, "operator [", IDSystem::get( operat.id ), "] can't be overloaded as unary" );
        break;

    case 1:
        if ( operat.id != IDSystem::get( (String) op_add )
            && operat.id != IDSystem::get( (String) op_sub )
            && operat.id != IDSystem::get( (String) op_mul )
            && operat.id != IDSystem::get( (String) op_div )
            && operat.id != IDSystem::get( (String) op_pow )
            && operat.id != IDSystem::get( (String) op_mod )
            && operat.id != IDSystem::get( (String) op_cmpr ) )
            PARSER_PANIC( *it, "operator [", IDSystem::get( operat.id ), "] can't be overloaded" );
        break;

    default:
        PARSER_PANIC( *it, "operator can have at most 1 argument" );
    }

    parse_scope( it, end, std::get<Scope>( operat.body ) );
}

void dawn::Parser::parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable )
{
    if ( it->value == kw_let )
        variable.kind = VariableKind::LET;
    else if ( it->value == kw_var )
        variable.kind = VariableKind::VAR;
    else if ( it->value == kw_ref )
        variable.kind = VariableKind::REF;
    else
        PARSER_PANIC( *it, "expected let, var or ref keywords" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected variable name" );
    variable.id = IDSystem::get( it->value );
    Int name_line = it->line_number;
    ++it;

    if ( it->value == op_assign )
    {
        ++it;
        parse_expression( ExtractType::NEW_LINE, it, end, variable.expr );
    }
    else
        variable.expr = make_nothing_node();
}

void dawn::Parser::parse_expression( ExtractType type, Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it == end )
    {
        tree = make_nothing_node();
        return;
    }

    Array<Token> expr_tokens;
    expression_extract( type, it, end, expr_tokens );

    Int least_prec_op = -1;
    Bool op_is_unary = false;
    expression_precedence( expr_tokens, least_prec_op, op_is_unary );

    if ( least_prec_op >= 0 )
    {
        if ( op_is_unary )
        {
            if ( expr_tokens.size() < 2 )
                PARSER_PANIC( {}, "unary expected expression" );

            create_unary_node( expr_tokens[least_prec_op], tree );
            auto& un_nod = tree.as<UnaryNod>();

            auto it = expr_tokens.begin() + 1;
            parse_expression( ExtractType::DEFAULT, it, expr_tokens.end(), un_nod.right );
        }
        else
        {
            if ( (Int) expr_tokens.size() < least_prec_op + 2 )
                PARSER_PANIC( {}, "operator expected expression" );

            Array<Token> left;
            left.insert( left.end(), expr_tokens.begin(), expr_tokens.begin() + least_prec_op );
            Array<Token> right;
            right.insert( right.end(), expr_tokens.begin() + least_prec_op + 1, expr_tokens.end() );

            auto& op = expr_tokens[least_prec_op];
            if ( op.value == op_expr_opn )
                expression_complex_expr( left, right, tree );

            else if ( op.value == op_scope_opn )
                expression_complex_scope( left, right, tree );

            else if ( op.value == op_array_opn )
                expression_complex_array( left, right, tree );

            else
                expression_complex_default( left, op, right, tree );
        }
    }
    else
        expression_pure( expr_tokens, tree );
}

void dawn::Parser::expression_extract( ExtractType type, Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens )
{
    Array<Token>::const_iterator first_it = it;
    Int last_line = it->line_number;
    Int expr_depth = 0;
    Bool in_lambda = false;

    for ( ; it != end; ++it )
    {
        if ( expr_depth == 0 )
        {
            if ( type == ExtractType::NEW_LINE )
            {
                if ( it->line_number != last_line )
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
                    auto it_before = it - 1;
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
        last_line = it->line_number;
    }

    if ( expr_depth > 0 )
        PARSER_PANIC( *it, "expected expression end" );
}

void dawn::Parser::expression_precedence( Array<Token>& tokens, Int& index, Bool& unary )
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
            Int prec = is_unary ? precedences.at( prec_unary ) : precedences.at( token.value );
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

void dawn::Parser::expression_complex_expr( Array<Token>& left, Array<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right expression expected" );

    if ( right.back().value != op_expr_cls )
        PARSER_PANIC( right.back(), "expected call close" );

    right.pop_back();

    if ( !left.empty() )
    {
        auto& nod = tree.store<CallNod>();

        auto left_it = left.begin();
        parse_expression( ExtractType::DEFAULT, left_it, left.end(), nod.left_expr );

        for ( auto it = right.begin(); it != right.end(); )
            parse_expression( ExtractType::SPLITTER, it, right.end(), nod.args.emplace_back() );
    }
    else
    {
        auto it = right.begin();
        parse_expression( ExtractType::DEFAULT, it, right.end(), tree );
    }
}

void dawn::Parser::expression_complex_scope( Array<Token>& left, Array<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right scope expression expected" );

    if ( right.back().value != op_scope_cls )
        PARSER_PANIC( right.back(), "expected scope close" );

    right.pop_back();

    if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        Array<Pair<Int, Node>> args;
        Opt<Int> key;
        for ( auto it = right.begin(); it != right.end(); )
        {
            if ( it->type != TokenType::NAME )
                PARSER_PANIC( *it, "expected field init name" );

            Int name_id = IDSystem::get( it->value );
            if ( std::find_if( args.begin(), args.end(), [&]( auto const& entry ) { return entry.first == name_id; } ) != args.end() )
                PARSER_PANIC( *it, "argument [", it->value, "] already passed" );
            ++it;

            if ( it == right.end() && args.empty() )
            {
                key = name_id;
                break;
            }

            if ( it->value != op_assign )
                PARSER_PANIC( *it, "expected assign operator" );
            ++it;

            auto& arg = args.emplace_back( name_id, Node{} );
            parse_expression( ExtractType::SPLITTER, it, right.end(), arg.second );
        }

        if ( key )
        {
            auto& node = tree.store<EnumNod>();
            node.type_id = IDSystem::get( left.front().value );
            node.key_id = *key;
        }
        else
        {
            auto& node = tree.store<StructNod>();
            node.type_id = IDSystem::get( left.front().value );
            node.args = args;
        }
    }
    else if ( left.size() >= 2 && left.front().value == op_lambda && left.back().value == op_lambda )
    {
        left.erase( left.begin() );
        left.pop_back();

        auto& nod = tree.store<RefNod>();
        nod.value_ref = ValueRef{ Function{} };
        auto& func = nod.value_ref.as<Function>();

        Set<Int> args;
        for ( auto it = left.begin(); it != left.end(); )
        {
            auto& arg = func.args.emplace_back();

            if ( it->value == kw_let )
                arg.kind = VariableKind::LET;
            else if ( it->value == kw_var )
                arg.kind = VariableKind::VAR;
            else if ( it->value == kw_ref )
                arg.kind = VariableKind::REF;
            else
                PARSER_PANIC( *it, "expected let, var or ref keywords" );
            ++it;

            if ( it->type != TokenType::NAME )
                PARSER_PANIC( *it, "expected arg name" );
            arg.id = IDSystem::get( it->value );

            if ( args.contains( arg.id ) )
                PARSER_PANIC( *it, "argument [", it->value, "] already defined" );
            args.insert( arg.id );
            ++it;

            if ( it != left.end() )
            {
                if ( it->value != op_split )
                    PARSER_PANIC( *it, "expected split or lambda close" );
                ++it;
            }
        }

        Token left_scope;
        left_scope.value = op_scope_opn;
        left_scope.type = TokenType::OPERATOR;
        left_scope.line_number = -1;
        right.insert( right.begin(), left_scope );

        Token right_scope;
        right_scope.value = op_scope_cls;
        right_scope.type = TokenType::OPERATOR;
        right_scope.line_number = -1;
        right.push_back( right_scope );

        auto right_it = right.begin();
        parse_scope( right_it, right.end(), std::get<Scope>( func.body ) );
    }
    else
        PARSER_PANIC( left.front(), "unknown scope expression" );
}

void dawn::Parser::expression_complex_array( Array<Token>& left, Array<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right array expression expected" );

    if ( right.back().value != op_array_cls )
        PARSER_PANIC( right.back(), "expected array close" );

    right.pop_back();

    if ( left.empty() )
    {
        auto& nod = tree.store<ArrayNod>();

        for ( auto it = right.begin(); it != right.end(); )
            parse_expression( ExtractType::SPLITTER, it, right.end(), nod.LIST_list.emplace_back() );

        nod.init_type = ArrayNod::InitType::LIST;
    }
    else if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        auto& node = tree.store<ArrayNod>();
        node.SIZE_value_expr = make_def_val( left.front().value );

        auto right_it = right.begin();
        parse_expression( ExtractType::DEFAULT, right_it, right.end(), node.SIZE_size_expr );
        node.init_type = ArrayNod::InitType::SIZE;
    }
    else
    {
        auto& nod = tree.store<IndexNod>();

        auto left_it = left.begin();
        parse_expression( ExtractType::DEFAULT, left_it, left.end(), nod.left_expr );

        auto right_it = right.begin();
        parse_expression( ExtractType::DEFAULT, right_it, right.end(), nod.expr );
    }
}

void dawn::Parser::expression_complex_default( Array<Token>& left, Token op, Array<Token>& right, Node& tree )
{
    auto it = left.begin();
    Node left_expr;
    parse_expression( ExtractType::DEFAULT, it, left.end(), left_expr );

    it = right.begin();
    Node right_expr;
    parse_expression( ExtractType::DEFAULT, it, right.end(), right_expr );

    try
    {
        create_operator_node( op, tree );
        auto& op_node = tree.as<OperatorNod>();
        op_node.left = left_expr;
        op_node.right = right_expr;
    }
    catch ( ... )
    {
        tree = {};
        create_assign_node( op, tree );
        auto& as_node = tree.as<AssignNod>();
        as_node.left = left_expr;
        as_node.right = right_expr;
    }
}

void dawn::Parser::expression_pure( Array<Token>& tokens, Node& tree )
{
    if ( tokens.empty() )
    {
        tree = make_nothing_node();
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
        PARSER_PANIC( token, "single type is not an expression" );

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
        PARSER_PANIC( token, "expected literal" );
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
        tree.store<IdentifierNod>().id = IDSystem::get( (String) kw_self );
    }
    else
        PARSER_PANIC( token, "keyword [", token.value, "] is not an expression" );
}

void dawn::Parser::expression_single_identifier( Token const& token, Node& tree )
{
    tree.store<IdentifierNod>().id = IDSystem::get( token.value );
}

void dawn::Parser::parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope )
{
    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    Set<Int> vars;
    while ( it->value != op_scope_cls )
    {
        if ( it->value == kw_let || it->value == kw_var || it->value == kw_ref )
        {
            auto& node = scope.instr.emplace_back().store<VariableNod>();
            parse_variable( it, end, node.var );

            if ( vars.contains( node.var.id ) )
                PARSER_PANIC( *it, "variable [", IDSystem::get( node.var.id ), "] already defined" );
            vars.insert( node.var.id );
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
        else if ( it->value == kw_throw )
        {
            auto& instr = scope.instr.emplace_back();
            scope_throw( it, end, instr );
        }
        else if ( it->value == kw_try )
        {
            auto& instr = scope.instr.emplace_back();
            scope_try( it, end, instr );
        }
        else if ( it->value == op_scope_opn )
        {
            auto& chld_scp = scope.instr.emplace_back().store<Scope>();
            parse_scope( it, end, chld_scp );
        }
        else
        {
            auto& expr = scope.instr.emplace_back();
            parse_expression( ExtractType::NEW_LINE, it, end, expr );
        }
    }
    ++it;
}

void dawn::Parser::scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_return )
        PARSER_PANIC( *it, "expected return" );
    Int return_line = it->line_number;
    ++it;

    auto& node = tree.store<ReturnNod>();
    if ( it->line_number == return_line )
        parse_expression( ExtractType::NEW_LINE, it, end, node.expr );
    else
        node.expr = make_nothing_node();
}

void dawn::Parser::scope_break( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_break )
        PARSER_PANIC( *it, "expected break" );
    ++it;

    tree.store<BreakNod>();
}

void dawn::Parser::scope_continue( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_continue )
        PARSER_PANIC( *it, "expected continue" );
    ++it;

    tree.store<ContinueNod>();
}

void dawn::Parser::scope_throw( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_throw )
        PARSER_PANIC( *it, "expected throw" );
    ++it;

    auto& node = tree.store<ThrowNod>();
    parse_expression( ExtractType::NEW_LINE, it, end, node.expr );
}

void dawn::Parser::scope_try( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_try )
        PARSER_PANIC( *it, "expected try" );
    ++it;

    auto& node = tree.store<TryNod>();

    parse_scope( it, end, node.try_scope );

    if ( it->value != kw_catch )
        PARSER_PANIC( *it, "expected catch" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected catch name" );
    node.catch_id = IDSystem::get( it->value );
    ++it;

    parse_scope( it, end, node.catch_scope );
}

void dawn::Parser::scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_if )
        PARSER_PANIC( *it, "expected if keyword" );
    ++it;

    auto& node = tree.store<IfNod>();

    parse_expression( ExtractType::SCOPE_START, it, end, node.if_part.expr );
    parse_scope( it, end, node.if_part.scope );

    while ( true )
    {
        if ( it->value == kw_elif )
        {
            ++it;
            auto& part = node.elif_parts.emplace_back();
            parse_expression( ExtractType::SCOPE_START, it, end, part.expr );
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
        PARSER_PANIC( *it, "expected switch keyword" );
    ++it;

    auto& node = tree.store<SwitchNod>();

    parse_expression( ExtractType::SCOPE_START, it, end, node.main_expr );

    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    while ( it->value != op_scope_cls )
    {
        if ( it->value == kw_case )
        {
            ++it;

            auto& casee = node.cases.emplace_back();

            Array<Token> case_tokens;
            expression_extract( ExtractType::SCOPE_START, it, end, case_tokens );

            for ( auto it = case_tokens.begin(); it != case_tokens.end(); )
                parse_expression( ExtractType::SPLITTER, it, case_tokens.end(), casee.exprs.emplace_back() );

            parse_scope( it, end, casee.scope );
        }
        else if ( it->value == kw_default )
        {
            ++it;

            if ( node.def_scope )
                PARSER_PANIC( *it, "default already defined" );

            auto& scope = node.def_scope.emplace();
            parse_scope( it, end, scope );
        }
        else
            PARSER_PANIC( *it, "expected case or default" );
    }
    ++it;
}

void dawn::Parser::scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_loop )
        PARSER_PANIC( *it, "expected loop keyword" );
    ++it;

    auto& node = tree.store<LoopNod>();
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_while )
        PARSER_PANIC( *it, "expected while keyword" );
    ++it;

    auto& node = tree.store<WhileNod>();
    parse_expression( ExtractType::SCOPE_START, it, end, node.expr );
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_for )
        PARSER_PANIC( *it, "expected for keyword" );
    ++it;

    auto& node = tree.store<ForNod>();

    if ( it->value == kw_let )
        node.var.kind = VariableKind::LET;
    else if ( it->value == kw_var )
        node.var.kind = VariableKind::VAR;
    else if ( it->value == kw_ref )
        node.var.kind = VariableKind::REF;
    else
        PARSER_PANIC( *it, "expected let, var or ref keywords" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected name" );
    node.var.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_link )
        PARSER_PANIC( *it, "expected link operator" );
    ++it;

    parse_expression( ExtractType::SCOPE_START, it, end, node.expr );
    parse_scope( it, end, node.scope );
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

void dawn::create_unary_node( Token const& token, Node& node )
{
    auto& un_nod = node.store<UnaryNod>();

    if ( token.value == op_add )
        un_nod.type = UnaryType::PLUS;

    else if ( token.value == op_sub )
        un_nod.type = UnaryType::MINUS;

    else if ( token.value == op_not )
        un_nod.type = UnaryType::NOT;

    else
        PARSER_PANIC( token, "unknown unary operator" );
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

    else if ( token.value == op_cmpr )
        op_nod.type = OperatorType::COMPARE;

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
        PARSER_PANIC( token, "unknown binary operator" );
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
        PARSER_PANIC( token, "unknown assign operator" );
}

dawn::Node dawn::make_def_val( StringRef const& type )
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
        return make_value_node( ValueRef{ RangeVal{} } );

    else if ( is_custom_type( type ) )
    {
        Node result;
        result.store<StructNod>().type_id = IDSystem::get( (String) type );
        return result;
    }

    else
        PARSER_PANIC( {}, "[", type, "] doesn't have a default value" );
}
