#include "parser.h"
#include "pool.h"


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

void dawn::Parser::parse( Vector<Token>& tokens, Module& module )
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
        else if ( it->value == kw_const || it->value == kw_var || it->value == kw_ref )
        {
            parse_global_variable( it, tokens.end(), module );
        }
        else
            PARSER_PANIC( *it, "not allowed in global scope or allowed only 1 instance of" );
    }
}

void dawn::Parser::parse_import( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module )
{
    if ( it->value != kw_import )
        PARSER_PANIC( *it, "expected import keyword" );
    ++it;

    if ( it->type != TokenType::STRING )
        PARSER_PANIC( *it, "expected import path" );
    module.imports.insert( it->lit_val );
    ++it;
}

void dawn::Parser::parse_global_struct( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module )
{
    Struct struc;
    parse_struct( it, end, struc );

    if ( module.contains_id( struc.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( struc.id ), "] already in use" );

    module.structs.push_back( struc );
}

void dawn::Parser::parse_global_enum( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module )
{
    Enum enu;
    parse_enum( it, end, enu );

    if ( module.contains_id( enu.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( enu.id ), "] already in use" );

    module.enums.push_back( enu );
}

void dawn::Parser::parse_global_function( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module )
{
    Function function;
    parse_function( it, end, function );

    if ( module.contains_id( function.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( function.id ), "] already in use" );

    module.functions.push_back( function );
}

void dawn::Parser::parse_global_variable( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module )
{
    Variable variable;
    parse_variable( it, end, variable );

    if ( module.contains_id( variable.id ) )
        PARSER_PANIC( {}, "name [", IDSystem::get( variable.id ), "] already in use" );

    module.variables.push_back( variable );
}

void dawn::Parser::parse_struct( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Struct& struc )
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
            if ( struc.contains( name_id ) )
                PARSER_PANIC( *it, "struct field [", IDSystem::get( name_id ), "] already defined" );

            auto& field_exp = struc.fields.emplace_back( name_id, NodeRef{} ).expr;
            field_exp = node_pool().new_register();
            ++it;

            if ( it->value == op_assign )
            {
                ++it;
                parse_expression( ExtractType::NEW_LINE, it, end, field_exp.value() );
            }
            else
                field_exp.value() = make_nothing_node();
        }
        else if ( it->value == kw_func )
        {
            Function method;
            parse_function( it, end, method );
            if ( struc.contains( method.id ) )
                PARSER_PANIC( *it, "struct method [", IDSystem::get( method.id ), "] already defined" );

            auto& self_var = *method.args.emplace( method.args.begin() );
            self_var.kind = VariableKind::REF;
            self_var.id = IDSystem::get( (String) kw_self );
            struc.methods.push_back( method );
        }
        else if ( it->value == kw_oper )
        {
            Function op;
            parse_operator( it, end, op );
            if ( struc.contains( op.id ) )
                PARSER_PANIC( *it, "struct operator [", IDSystem::get( op.id ), "] already defined" );

            auto& self_var = *op.args.emplace( op.args.begin() );
            self_var.kind = VariableKind::REF;
            self_var.id = IDSystem::get( (String) kw_self );
            struc.methods.push_back( op );
        }
        else
            PARSER_PANIC( *it, "expected field name or function" );
    }
    ++it;
}

void dawn::Parser::parse_enum( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Enum& enu )
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
            if ( enu.contains( name_id ) )
                PARSER_PANIC( *it, "key [", it->value, "] already in use" );

            auto& entry = enu.entries.emplace_back();
            entry.id = name_id;
            entry.expr = node_pool().new_register();
            auto& expr = entry.expr.value();
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

    if ( enu.entries.empty() )
        PARSER_PANIC( {}, "enum [", IDSystem::get( enu.id ), "] cannot be empty" );
}

void dawn::Parser::parse_function( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Function& function )
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

        if ( it->value == kw_const )
            arg.kind = VariableKind::CONST;
        else if ( it->value == kw_var )
            arg.kind = VariableKind::VAR;
        else if ( it->value == kw_ref )
            arg.kind = VariableKind::REF;
        else
            PARSER_PANIC( *it, "expected const, var or ref keyword" );
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

void dawn::Parser::parse_operator( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Function& operat )
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

        if ( it->value == kw_const )
            arg.kind = VariableKind::CONST;
        else if ( it->value == kw_var )
            arg.kind = VariableKind::VAR;
        else if ( it->value == kw_ref )
            arg.kind = VariableKind::REF;
        else
            PARSER_PANIC( *it, "expected const, var or ref keyword" );
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

void dawn::Parser::parse_variable( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Variable& variable )
{
    if ( it->value == kw_const )
        variable.kind = VariableKind::CONST;
    else if ( it->value == kw_var )
        variable.kind = VariableKind::VAR;
    else if ( it->value == kw_ref )
        variable.kind = VariableKind::REF;
    else
        PARSER_PANIC( *it, "expected const, var or ref keyword" );
    ++it;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected variable name" );
    variable.id = IDSystem::get( it->value );
    Int name_line = it->line_number;
    ++it;

    variable.expr = node_pool().new_register();
    if ( it->value == op_assign )
    {
        ++it;
        parse_expression( ExtractType::NEW_LINE, it, end, variable.expr.value() );
    }
    else
        variable.expr.value() = make_nothing_node();
}

void dawn::Parser::parse_expression( ExtractType type, Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it == end )
    {
        tree = make_nothing_node();
        return;
    }

    Vector<Token> expr_tokens;
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
            un_nod.right = node_pool().new_register();
            parse_expression( ExtractType::DEFAULT, it, expr_tokens.end(), un_nod.right.value() );
        }
        else
        {
            if ( (Int) expr_tokens.size() < least_prec_op + 2 )
                PARSER_PANIC( {}, "operator expected expression" );

            Vector<Token> left;
            left.insert( left.end(), expr_tokens.begin(), expr_tokens.begin() + least_prec_op );
            Vector<Token> right;
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

void dawn::Parser::expression_extract( ExtractType type, Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Vector<Token>& tokens )
{
    Vector<Token>::const_iterator first_it = it;
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

void dawn::Parser::expression_complex_expr( Vector<Token>& left, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right expression expected" );

    if ( right.back().value != op_expr_cls )
        PARSER_PANIC( right.back(), "expected call close" );

    right.pop_back();

    if ( !left.empty() )
    {
        auto& nod = tree.emplace<CallNod>();

        auto left_it = left.begin();
        nod.left_expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, left_it, left.end(), nod.left_expr.value() );

        for ( auto it = right.begin(); it != right.end(); )
            parse_expression( ExtractType::SPLITTER, it, right.end(), nod.args.emplace_back() );
    }
    else
    {
        auto it = right.begin();
        parse_expression( ExtractType::DEFAULT, it, right.end(), tree );
    }
}

void dawn::Parser::expression_complex_scope( Vector<Token>& left, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right scope expression expected" );

    if ( right.back().value != op_scope_cls )
        PARSER_PANIC( right.back(), "expected scope close" );

    right.pop_back();

    if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        OMap<Int, Node> args;
        Opt<Int> key;
        for ( auto it = right.begin(); it != right.end(); )
        {
            if ( it->type != TokenType::NAME )
                PARSER_PANIC( *it, "expected field init name" );

            Int name_id = IDSystem::get( it->value );
            if ( args.contains( name_id ) )
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

            auto& arg = args[name_id];
            parse_expression( ExtractType::SPLITTER, it, right.end(), arg );
        }

        if ( key )
        {
            auto& node = tree.emplace<EnumNod>();
            node.type_id = IDSystem::get( left.front().value );
            node.key_id = *key;
        }
        else
        {
            auto& node = tree.emplace<StructNod>();
            node.type_id = IDSystem::get( left.front().value );
            node.args = args;
        }
    }
    else if ( left.size() >= 2 && left.front().value == op_lambda && left.back().value == op_lambda )
    {
        left.erase( left.begin() );
        left.pop_back();

        auto& nod = tree.emplace<RefNod>();
        nod.value_ref = Value{ Function{} };
        auto& func = nod.value_ref.as_function();

        Set<Int> args;
        for ( auto it = left.begin(); it != left.end(); )
        {
            auto& arg = func.args.emplace_back();

            if ( it->value == kw_const )
                arg.kind = VariableKind::CONST;
            else if ( it->value == kw_var )
                arg.kind = VariableKind::VAR;
            else if ( it->value == kw_ref )
                arg.kind = VariableKind::REF;
            else
                PARSER_PANIC( *it, "expected const, var or ref keyword" );
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

void dawn::Parser::expression_complex_array( Vector<Token>& left, Vector<Token>& right, Node& tree )
{
    if ( right.empty() )
        PARSER_PANIC( {}, "right array expression expected" );

    if ( right.back().value != op_array_cls )
        PARSER_PANIC( right.back(), "expected array close" );

    right.pop_back();

    if ( left.empty() )
    {
        auto& nod = tree.emplace<ArrayNod>();
        nod.init_type = ArrayNod::InitType::LIST;

        for ( auto it = right.begin(); it != right.end(); )
            parse_expression( ExtractType::SPLITTER, it, right.end(), nod.LIST_list.emplace_back() );
    }
    else if ( left.size() == 1 && left.front().type == TokenType::TYPE )
    {
        auto& node = tree.emplace<ArrayNod>();
        node.init_type = ArrayNod::InitType::SIZE;

        auto right_it = right.begin();
        node.SIZE_typeid = IDSystem::get( left.front().value );
        node.SIZE_expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, right_it, right.end(), node.SIZE_expr.value() );
    }
    else
    {
        auto& nod = tree.emplace<IndexNod>();

        auto left_it = left.begin();
        nod.left_expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, left_it, left.end(), nod.left_expr.value() );

        auto right_it = right.begin();
        nod.expr = node_pool().new_register();
        parse_expression( ExtractType::DEFAULT, right_it, right.end(), nod.expr.value() );
    }
}

void dawn::Parser::expression_complex_default( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree )
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
        op_node.sides.emplace_back( left_expr );
        op_node.sides.emplace_back( right_expr );
    }
    catch ( ... )
    {
        tree = {};
        create_assign_node( op, tree );
        auto& as_node = tree.as<AssignNod>();
        as_node.sides.emplace_back( left_expr );
        as_node.sides.emplace_back( right_expr );
    }
}

void dawn::Parser::expression_pure( Vector<Token>& tokens, Node& tree )
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
        tree.emplace<IdentifierNod>().id = IDSystem::get( (String) kw_self );
    }
    else
        PARSER_PANIC( token, "keyword [", token.value, "] is not an expression" );
}

void dawn::Parser::expression_single_identifier( Token const& token, Node& tree )
{
    tree.emplace<IdentifierNod>().id = IDSystem::get( token.value );
}

void dawn::Parser::parse_scope( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Scope& scope )
{
    if ( it->value != op_scope_opn )
        PARSER_PANIC( *it, "expected scope open" );
    ++it;

    Set<Int> vars;
    while ( it->value != op_scope_cls )
    {
        if ( it->value == kw_const || it->value == kw_var || it->value == kw_ref )
        {
            auto& node = scope.instr.emplace_back().emplace<VariableNod>();
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
            auto& chld_scp = scope.instr.emplace_back().emplace<Scope>();
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

void dawn::Parser::scope_return( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_return )
        PARSER_PANIC( *it, "expected return" );
    Int return_line = it->line_number;
    ++it;

    auto& node = tree.emplace<ReturnNod>();
    node.expr = node_pool().new_register();
    if ( it->line_number == return_line )
        parse_expression( ExtractType::NEW_LINE, it, end, node.expr.value() );
    else
        node.expr.value() = make_nothing_node();
}

void dawn::Parser::scope_break( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_break )
        PARSER_PANIC( *it, "expected break" );
    ++it;

    tree.emplace<BreakNod>();
}

void dawn::Parser::scope_continue( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_continue )
        PARSER_PANIC( *it, "expected continue" );
    ++it;

    tree.emplace<ContinueNod>();
}

void dawn::Parser::scope_throw( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_throw )
        PARSER_PANIC( *it, "expected throw" );
    ++it;

    auto& node = tree.emplace<ThrowNod>();
    node.expr = node_pool().new_register();
    parse_expression( ExtractType::NEW_LINE, it, end, node.expr.value() );
}

void dawn::Parser::scope_try( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_try )
        PARSER_PANIC( *it, "expected try" );
    ++it;

    auto& node = tree.emplace<TryNod>();

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

void dawn::Parser::scope_if( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_if )
        PARSER_PANIC( *it, "expected if keyword" );
    ++it;

    auto& node = tree.emplace<IfNod>();

    parse_expression( ExtractType::SCOPE_START, it, end, node.parts.emplace_back().expr );
    parse_scope( it, end, node.parts.back().scope );

    while ( true )
    {
        if ( it->value == kw_elif )
        {
            ++it;
            auto& part = node.parts.emplace_back();
            parse_expression( ExtractType::SCOPE_START, it, end, part.expr );
            parse_scope( it, end, part.scope );
        }
        else if ( it->value == kw_else )
        {
            ++it;
            auto& part = node.parts.emplace_back();
            part.expr = make_bool_node( true );
            parse_scope( it, end, part.scope );
            break;
        }
        else
            break;
    }
}

void dawn::Parser::scope_switch( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_switch )
        PARSER_PANIC( *it, "expected switch keyword" );
    ++it;

    auto& node = tree.emplace<SwitchNod>();

    node.main_expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, end, node.main_expr.value() );

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

void dawn::Parser::scope_loop( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_loop )
        PARSER_PANIC( *it, "expected loop keyword" );
    ++it;

    auto& node = tree.emplace<LoopNod>();
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_while( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_while )
        PARSER_PANIC( *it, "expected while keyword" );
    ++it;

    auto& node = tree.emplace<WhileNod>();
    node.expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, end, node.expr.value() );
    parse_scope( it, end, node.scope );
}

void dawn::Parser::scope_for( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree )
{
    if ( it->value != kw_for )
        PARSER_PANIC( *it, "expected for keyword" );
    ++it;

    auto& node = tree.emplace<ForNod>();
    node.var.kind = VariableKind::REF;

    if ( it->type != TokenType::NAME )
        PARSER_PANIC( *it, "expected name" );
    node.var.id = IDSystem::get( it->value );
    ++it;

    if ( it->value != op_link )
        PARSER_PANIC( *it, "expected link operator" );
    ++it;

    node.expr = node_pool().new_register();
    parse_expression( ExtractType::SCOPE_START, it, end, node.expr.value() );
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
    auto& un_nod = node.emplace<UnaryNod>();

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
    auto& op_nod = node.emplace<OperatorNod>();

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
    auto& as_nod = node.emplace<AssignNod>();
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
