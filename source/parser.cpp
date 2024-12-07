#include "parser.h"


std::wostream& dawn::operator<<( std::wostream& stream, ParseError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Bool dawn::Module::contains_id( StringRef const& id ) const
{
    if ( std::find_if( variables.begin(), variables.end(), [&]( Variable const& var ) { return var.name == id; } ) != variables.end() )
        return true;

    if ( std::find_if( operators.begin(), operators.end(), [&]( Operator const& oper ) { return oper.name == id; } ) != operators.end() )
        return true;

    if ( std::find_if( functions.begin(), functions.end(), [&]( Function const& func ) { return func.name == id; } ) != functions.end() )
        return true;

    if ( std::find_if( enums.begin(), enums.end(), [&]( Enum const& enu ) { return enu.name == id; } ) != enums.end() )
        return true;

    if ( std::find_if( layers.begin(), layers.end(), [&]( Layer const& layer ) { return layer.name == id; } ) != layers.end() )
        return true;

    if ( std::find_if( structs.begin(), structs.end(), [&]( Struct const& struc ) { return struc.name == id; } ) != structs.end() )
        return true;

    return false;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse( Array<Token>& tokens, Module& module )
{
    prepare_tokens( tokens );

    auto it = tokens.begin();
    auto end = tokens.end();

    while ( it != end )
    {
        if ( it->value == kw_enum )
        {
            if ( auto error = parse_global_enum( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_layer )
        {
            if ( auto error = parse_global_layer( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_struct )
        {
            if ( auto error = parse_global_struct( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_func )
        {
            if ( auto error = parse_global_function( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_oper )
        {
            if ( auto error = parse_global_operator( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_let || it->value == kw_var || it->value == kw_ref )
        {
            if ( auto error = parse_global_variable( it, end, module ) )
                return error;
        }
        else
        {
            return ParseError{ *it, L"not allowed in global scope or allowed only 1 instance of" };
        }
    }

    return std::nullopt;
}

void dawn::Parser::prepare_tokens( Array<Token>& tokens )
{
    for ( Int i = 0; i < (Int) tokens.size() - 1; i++ )
    {
        if ( tokens[i].type == TokenType::NAME && tokens[i + 1].value == op_expr_opn )
            tokens[i].type = TokenType::FUNCTION;
    }
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Struct struc;
    if ( auto error = parse_struct( it, end, struc ) )
        return error;

    if ( module.contains_id( struc.name ) )
        return ParseError{ {}, L"name [" + struc.name + L"] already in use" };

    module.structs.push_back( struc );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Layer layer;
    if ( auto error = parse_layer( it, end, layer ) )
        return error;

    if ( module.contains_id( layer.name ) )
        return ParseError{ {}, L"name [" + layer.name + L"] already in use" };

    module.layers.push_back( layer );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Enum enu;
    if ( auto error = parse_enum( it, end, enu ) )
        return error;

    if ( module.contains_id( enu.name ) )
        return ParseError{ {}, L"name [" + enu.name + L"] already in use" };

    module.enums.push_back( enu );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Function function;
    if ( auto error = parse_function( it, end, function ) )
        return error;

    if ( module.contains_id( function.name ) )
        return ParseError{ {}, L"name [" + function.name + L"] already in use" };

    module.functions.push_back( function );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Operator operato;
    if ( auto error = parse_operator( it, end, operato ) )
        return error;

    module.operators.push_back( operato );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_global_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Variable variable;
    if ( auto error = parse_variable( it, end, variable ) )
        return error;

    if ( module.contains_id( variable.name ) )
        return ParseError{ {}, L"name [" + variable.name + L"] already in use" };

    module.variables.push_back( variable );

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Struct& struc )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Layer& layer )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Enum& enu )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function )
{
    if ( it->value != kw_func )
        return ParseError{ *it, L"expected function" };
    ++it;

    if ( it->type != TokenType::FUNCTION )
        return ParseError{ *it, L"expected function name" };
    function.name = it->value;
    ++it;

    if ( it->value != op_expr_opn )
        return ParseError{ *it, L"expected open expr" };
    ++it;

    Set<String> args;

    while ( true )
    {
        if ( it->value == op_expr_cls )
        {
            ++it;
            break;
        }

        Variable arg;

        if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
            return ParseError{ *it, L"expected let, var or ref keywords" };
        if ( it->value == kw_let )
            arg.kind = Variable::Kind::LET;
        else if ( it->value == kw_var )
            arg.kind = Variable::Kind::VAR;
        else
            arg.kind = Variable::Kind::REF;
        ++it;

        if ( it->type != TokenType::NAME )
            return ParseError{ *it, L"expected arg name" };
        arg.name = it->value;
        ++it;

        if ( args.contains( arg.name ) )
            return ParseError{ *it, L"argument [", arg.name, L"] already defined" };
        args.insert( arg.name );

        function.args.push_back( arg );

        if ( it->value == op_split )
            ++it;
    }

    if ( auto error = parse_scope( it, end, std::get<Scope>( function.body ) ) )
        return error;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Operator& operato )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable )
{
    if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
        return ParseError{ *it, L"expected let, var or ref keywords" };
    if ( it->value == kw_let )
        variable.kind = Variable::Kind::LET;
    else if ( it->value == kw_var )
        variable.kind = Variable::Kind::VAR;
    else
        variable.kind = Variable::Kind::REF;
    ++it;

    if ( it->type != TokenType::NAME )
        return ParseError{ *it, L"expected variable name" };
    variable.name = it->value;
    Int name_line = it->line_number;
    ++it;

    if ( it->line_number == name_line )
    {
        if ( it->value != op_assign )
            return ParseError{ *it, L"expected variable assignment" };
        ++it;

        if ( auto error = parse_expression( it, end, variable.expr ) )
            return error;
    }
    else
        variable.expr = std::make_shared<NothingNode>();

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::type_basic( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, String& type )
{
    if ( it->value != tp_bool
        && it->value != tp_int
        && it->value != tp_float
        && it->value != tp_char
        && it->value != tp_string
        && it->value != op_range
        && it->type != TokenType::TYPE )
        return ParseError{ *it, L"invalid type" };

    type = it->value;
    ++it;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_expression( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it == end )
    {
        tree = std::make_shared<NothingNode>();
        return std::nullopt;
    }

    Array<Token> expr_tokens;
    if ( auto error = expression_extract( it, end, expr_tokens ) )
        return error;

    Int least_prec_op = -1;
    if ( auto error = expression_precedence( expr_tokens, least_prec_op ) )
        return error;

    if ( least_prec_op == 0 )
    {
        if ( expr_tokens.size() < 2 )
            return ParseError{ *it, L"unary expected expression" };

        Ref<UnaryNode> node;
        if ( auto error = create_unary_node( expr_tokens.front(), node ) )
            return error;

        auto it = expr_tokens.begin() + 1;
        if ( auto error = parse_expression( it, expr_tokens.end(), node->right ) )
            return error;

        tree = node;
    }
    else if ( least_prec_op > 0 )
    {
        if ( (Int) expr_tokens.size() < least_prec_op + 2 )
            return ParseError{ *it, L"unary expected expression" };

        Ref<OperatorNode> node;
        if ( auto error = create_operator_node( expr_tokens[least_prec_op], node ) )
            return error;

        auto it = expr_tokens.begin();
        if ( auto error = parse_expression( it, expr_tokens.begin() + least_prec_op, node->left ) )
            return error;

        it = expr_tokens.begin() + (least_prec_op + 1);
        if ( auto error = parse_expression( it, expr_tokens.end(), node->right ) )
            return error;

        tree = node;
    }
    else
    {
        if ( auto error = expression_pure( expr_tokens, tree ) )
            return error;
    }

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_extract( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens )
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
        return ParseError{ *it, L"expected expression end" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_precedence( Array<Token> const& tokens, Int& index )
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
                return ParseError{ token, L"expected expression end" };
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

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_pure( Array<Token> const& tokens, Ref<Node>& tree )
{
    if ( tokens.empty() )
    {
        tree = std::make_shared<NothingNode>();
        return std::nullopt;
    }

    if ( tokens.size() == 1 )
    {
        if ( auto error = expression_single( tokens[0], tree ) )
            return error;
    }
    else if ( tokens.front().type == TokenType::TYPE )
    {
        if ( auto error = expression_type( tokens, tree ) )
            return error;
    }
    else if ( tokens.front().type == TokenType::FUNCTION )
    {
        if ( auto error = expression_function( tokens, tree ) )
            return error;
    }
    else if ( tokens.front().value == op_expr_opn )
    {
        if ( tokens.back().value != op_expr_cls )
            return ParseError{ tokens.back(), L"expected expression close" };

        auto begin = tokens.begin() + 1;
        auto end = tokens.begin() + tokens.size() - 1;
        return parse_expression( begin, end, tree );
    }
    else if ( tokens.front().value == op_array_opn )
    {
        if ( auto error = expression_type_array( tokens, tree ) )
            return error;
    }
    else
        return ParseError{ tokens[0], L"unknown expression token" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single( Token const& token, Ref<Node>& tree )
{
    switch ( token.type )
    {
    case TokenType::INTEGER:
    case TokenType::FLOAT:
    case TokenType::CHAR:
    case TokenType::STRING:
        if ( auto error = expression_single_literal( token, tree ) )
            return error;
        break;

    case TokenType::KEYWORD:
        if ( auto error = expression_single_keyword( token, tree ) )
            return error;
        break;

    case TokenType::TYPE:
        return ParseError{ token, L"type is not an expression" };

    case TokenType::FUNCTION:
        return ParseError{ token, L"function is not an expression" };

    case TokenType::NAME:
        if ( auto error = expression_single_identifier( token, tree ) )
            return error;
        break;

    case TokenType::OPERATOR:
        return ParseError{ token, L"operator is not an expression" };
    }

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_literal( Token const& token, Ref<Node>& tree )
{
    if ( token.type == TokenType::INTEGER )
    {
        auto value = IntValue::make();
        value->value = std::stoll( token.lit_val );
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::FLOAT )
    {
        auto value = FloatValue::make();
        value->value = std::stod( token.lit_val );
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::CHAR )
    {
        auto value = CharValue::make();
        value->value = token.lit_val[0];
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::STRING )
    {
        auto value = StringValue::make();
        value->value = token.lit_val;
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else
        return ParseError{ token, L"expected literal" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_keyword( Token const& token, Ref<Node>& tree )
{
    if ( token.value == kw_true )
    {
        auto value = BoolValue::make();
        value->value = true;
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.value == kw_false )
    {
        auto value = BoolValue::make();
        value->value = false;
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.value == kw_self )
    {
        auto node = std::make_shared<SelfNode>();
        tree = node;
    }
    else
        return ParseError{ token, L"keyword is not an expression" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_identifier( Token const& token, Ref<Node>& tree )
{
    auto node = std::make_shared<IdentifierNode>();
    node->name = token.value;
    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_type( Array<Token> const& tokens, Ref<Node>& tree )
{
    auto& second_token = tokens[1];
    if ( second_token.value == op_expr_opn )
    {
        if ( auto error = expression_type_cast( tokens, tree ) )
            return error;
    }
    else if ( second_token.value == op_scope_opn )
    {
        if ( auto error = expression_type_make( tokens, tree ) )
            return error;
    }
    else if ( second_token.value == op_array_opn )
    {
        if ( auto error = expression_type_array( tokens, tree ) )
            return error;
    }
    else
        return ParseError{ second_token, L"unknown type usage" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_type_cast( Array<Token> const& tokens, Ref<Node>& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    Ref node = std::make_shared<CastNode>();

    if ( auto error = type_basic( it, tokens.end(), node->type ) )
        return error;

    if ( it->value != op_expr_opn )
        return ParseError{ *it, L"expected expression open" };
    ++it;

    Array<Token> expr_tokens;
    if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
        return error;

    Array<Token>::const_iterator expr_it = expr_tokens.begin();
    if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->expr ) )
        return error;

    if ( it->value != op_expr_cls )
        return ParseError{ *it, L"expected expression close" };
    ++it;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_type_make( Array<Token> const& tokens, Ref<Node>& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    Ref node = std::make_shared<StructNode>();

    if ( auto error = type_basic( it, tokens.end(), node->type ) )
        return error;

    if ( it->value != op_scope_opn )
        return ParseError{ *it, L"expected expression open" };
    ++it;

    while ( true )
    {
        if ( it->value == op_scope_cls )
            break;

        if ( it->type != TokenType::NAME )
            return ParseError{ *it, L"expected field name" };

        String name = it->value;
        if ( node->args.contains( name ) )
            return ParseError{ *it, L"argument [" + name + L"] already passed" };
        ++it;

        if ( it->value != op_link )
            return ParseError{ *it, L"expected bind operator" };
        ++it;

        Array<Token> expr_tokens;
        if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
            return error;

        if ( expr_tokens.empty() )
            return ParseError{ *it, L"expected expression" };

        Array<Token>::const_iterator expr_it = expr_tokens.begin();
        if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->args[name] ) )
            return error;
    }
    ++it;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_type_array( Array<Token> const& tokens, Ref<Node>& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    Ref node = std::make_shared<ArrayNode>();

    if ( it->value == op_array_opn )
    {
        ++it;

        while ( true )
        {
            Array<Token> expr_tokens;
            if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
                return error;

            if ( expr_tokens.empty() )
                break;

            Array<Token>::const_iterator expr_it = expr_tokens.begin();
            if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->LIST_list.emplace_back() ) )
                return error;
        }

        node->init_type = ArrayNode::InitType::LIST;
    }
    else
    {
        String val_type;
        if ( auto error = type_basic( it, tokens.end(), val_type ) )
            return error;

        node->SIZE_value_expr = make_def_type_expr( val_type );

        if ( it->value != op_array_opn )
            return ParseError{ *it, L"expected array size expression" };
        ++it;

        Array<Token> expr_tokens;
        if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
            return error;

        Array<Token>::const_iterator expr_it = expr_tokens.begin();
        if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->SIZE_size_expr ) )
            return error;

        node->init_type = ArrayNode::InitType::SIZE;
    }

    if ( it->value != op_array_cls )
        return ParseError{ *it, L"expected array end" };

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_function( Array<Token> const& tokens, Ref<Node>& tree )
{
    Array<Token>::const_iterator it = tokens.begin();
    Ref node = std::make_shared<FunctionNode>();

    if ( it->type != TokenType::FUNCTION )
        return ParseError{ *it, L"expected function name" };
    node->name = it->value;
    ++it;

    if ( it->value != op_expr_opn )
        return ParseError{ *it, L"expected function open" };
    ++it;

    while ( true )
    {
        Array<Token> expr_tokens;
        if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
            return error;

        if ( expr_tokens.empty() )
            break;

        Array<Token>::const_iterator expr_it = expr_tokens.begin();
        if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->args.emplace_back() ) )
            return error;
    }

    if ( it->value != op_expr_cls )
        return ParseError{ *it, L"expected function close" };
    ++it;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope )
{
    if ( it->value != op_scope_opn )
        return ParseError{ *it, L"expected scope open" };
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
            Ref node = std::make_shared<VariableNode>();
            if ( auto error = parse_variable( it, end, node->var ) )
                return error;

            if ( vars.contains( node->var.name ) )
                return ParseError{ *it, L"variable [", node->var.name, L"] already defined" };
            vars.insert( node->var.name );

            scope.instr.push_back( node );
        }
        else if ( it->value == kw_if )
        {
            Ref<Node> instr;
            if ( auto error = scope_if( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_switch )
        {
            Ref<Node> instr;
            if ( auto error = scope_switch( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_for )
        {
            Ref<Node> instr;
            if ( auto error = scope_for( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_while )
        {
            Ref<Node> instr;
            if ( auto error = scope_while( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_loop )
        {
            Ref<Node> instr;
            if ( auto error = scope_loop( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_return )
        {
            Ref<Node> instr;
            if ( auto error = scope_return( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_break )
        {
            Ref<Node> instr;
            if ( auto error = scope_break( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == kw_continue )
        {
            Ref<Node> instr;
            if ( auto error = scope_continue( it, end, instr ) )
                return error;
            scope.instr.push_back( std::move( instr ) );
        }
        else if ( it->value == op_scope_opn )
        {
            Ref<Scope> chld_scp = std::make_shared<Scope>();
            if ( auto error = parse_scope( it, end, *chld_scp ) )
                return error;
            scope.instr.push_back( chld_scp );
        }
        else
        {
            Ref<Node> expr;
            if ( auto error = parse_expression( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
    }

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_return )
        return ParseError{ *it, L"expected return" };
    Int last_line = it->line_number;
    ++it;

    Ref node = std::make_shared<ReturnNode>();

    if ( it->line_number == last_line )
    {
        if ( auto error = parse_expression( it, end, node->expr ) )
            return error;
    }
    else
        node->expr = std::make_shared<NothingNode>();

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_break( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_break )
        return ParseError{ *it, L"expected break" };
    ++it;

    tree = std::make_shared<BreakNode>();

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_continue( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_continue )
        return ParseError{ *it, L"expected continue" };
    ++it;

    tree = std::make_shared<ContinueNode>();

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_if )
        return ParseError{ *it, L"expected if keyword" };
    ++it;

    auto node = std::make_shared<IfNode>();

    if ( auto error = parse_expression( it, end, node->if_part.expr ) )
        return error;
    if ( auto error = parse_scope( it, end, node->if_part.scope ) )
        return error;

    while ( true )
    {
        if ( it->value == kw_elif )
        {
            ++it;
            auto& part = node->elif_parts.emplace_back();
            if ( auto error = parse_expression( it, end, part.expr ) )
                return error;
            if ( auto error = parse_scope( it, end, part.scope ) )
                return error;
        }
        else if ( it->value == kw_else )
        {
            ++it;
            node->else_part.emplace();
            if ( auto error = parse_scope( it, end, node->else_part->scope ) )
                return error;
            break;
        }
        else
            break;
    }

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_switch( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_loop )
        return ParseError{ *it, L"expected loop keyword" };
    ++it;

    auto node = std::make_shared<LoopNode>();

    if ( auto error = parse_scope( it, end, node->scope ) )
        return error;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_while )
        return ParseError{ *it, L"expected while keyword" };
    ++it;

    auto node = std::make_shared<WhileNode>();

    if ( auto error = parse_expression( it, end, node->expr ) )
        return error;

    if ( auto error = parse_scope( it, end, node->scope ) )
        return error;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_for )
        return ParseError{ *it, L"expected for keyword" };
    ++it;

    auto node = std::make_shared<ForNode>();

    if ( it->value != kw_let && it->value != kw_var && it->value != kw_ref )
        return ParseError{ *it, L"expected let, var or ref keywords" };
    if ( it->value == kw_let )
        node->var.kind = Variable::Kind::LET;
    else if ( it->value == kw_var )
        node->var.kind = Variable::Kind::VAR;
    else
        node->var.kind = Variable::Kind::REF;
    ++it;

    if ( it->type != TokenType::NAME )
        return ParseError{ *it, L"expected name" };
    node->var.name = it->value;
    ++it;

    if ( it->value != op_link )
        return ParseError{ *it, L"expected link operator" };
    ++it;

    if ( auto error = parse_expression( it, end, node->expr ) )
        return error;

    if ( auto error = parse_scope( it, end, node->scope ) )
        return error;

    tree = node;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::create_unary_node( Token const& token, Ref<UnaryNode>& node )
{
    if ( token.value == op_add )
    {
        node = std::make_shared<UnaryNodePlus>();
    }
    else if ( token.value == op_sub )
    {
        node = std::make_shared<UnaryNodeMinus>();
    }
    else if ( token.value == op_not )
    {
        node = std::make_shared<UnaryNodeNot>();
    }
    else if ( token.value == op_range )
    {
        node = std::make_shared<UnaryNodeRange>();
    }
    else
        return ParseError{ token, L"unknown unary operator" };

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::create_operator_node( Token const& token, Ref<OperatorNode>& node )
{
    if ( token.value == op_link )
    {
        node = std::make_shared<OperatorNodeLink>();
    }
    else if ( token.value == op_access )
    {
        node = std::make_shared<OperatorNodeAccess>();
    }
    else if ( token.value == op_range )
    {
        node = std::make_shared<OperatorNodeRange>();
    }
    else if ( token.value == op_pow )
    {
        node = std::make_shared<OperatorNodePow>();
    }
    else if ( token.value == op_mod )
    {
        node = std::make_shared<OperatorNodeMod>();
    }
    else if ( token.value == op_mul )
    {
        node = std::make_shared<OperatorNodeMul>();
    }
    else if ( token.value == op_div )
    {
        node = std::make_shared<OperatorNodeDiv>();
    }
    else if ( token.value == op_add )
    {
        node = std::make_shared<OperatorNodeAdd>();
    }
    else if ( token.value == op_sub )
    {
        node = std::make_shared<OperatorNodeSub>();
    }
    else if ( token.value == op_less )
    {
        node = std::make_shared<OperatorNodeLess>();
    }
    else if ( token.value == op_great )
    {
        node = std::make_shared<OperatorNodeGreat>();
    }
    else if ( token.value == op_lesseq )
    {
        node = std::make_shared<OperatorNodeLessEq>();
    }
    else if ( token.value == op_greateq )
    {
        node = std::make_shared<OperatorNodeGreatEq>();
    }
    else if ( token.value == op_eq )
    {
        node = std::make_shared<OperatorNodeEq>();
    }
    else if ( token.value == op_neq )
    {
        node = std::make_shared<OperatorNodeNotEq>();
    }
    else if ( token.value == op_and )
    {
        node = std::make_shared<OperatorNodeAnd>();
    }
    else if ( token.value == op_or )
    {
        node = std::make_shared<OperatorNodeOr>();
    }
    else if ( token.value == op_assign )
    {
        node = std::make_shared<AssignNode>();
    }
    else if ( token.value == op_addas )
    {
        node = std::make_shared<AssignNodeAdd>();
    }
    else if ( token.value == op_subas )
    {
        node = std::make_shared<AssignNodeSub>();
    }
    else if ( token.value == op_mulas )
    {
        node = std::make_shared<AssignNodeMul>();
    }
    else if ( token.value == op_divas )
    {
        node = std::make_shared<AssignNodeDiv>();
    }
    else if ( token.value == op_powas )
    {
        node = std::make_shared<AssignNodePow>();
    }
    else if ( token.value == op_modas )
    {
        node = std::make_shared<AssignNodeMod>();
    }
    else
        return ParseError{ token, L"unknown operator" };

    return std::nullopt;
}

dawn::Ref<dawn::Node> dawn::make_def_type_expr( StringRef const& type )
{
    if ( type == tp_bool )
        return make_bool_literal( {} );

    if ( type == tp_int )
        return make_int_literal( {} );

    if ( type == tp_float )
        return make_float_literal( {} );

    if ( type == tp_char )
        return make_char_literal( {} );

    if ( type == tp_string )
        return make_string_literal( {} );

    if ( type == op_range )
        return make_value_literal( RangeValue::make() );

    return make_value_literal( NothingValue::make() );
}
