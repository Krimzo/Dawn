#include "parser.h"


std::wostream& dawn::operator<<( std::wostream& stream, ParseError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Bool dawn::Module::contains_id( StringRef const& id ) const
{
    String id_str{ id };
    return variables.contains( id_str ) ||
        functions.contains( id_str ) ||
        enums.contains( id_str ) ||
        layers.contains( id_str ) ||
        structs.contains( id_str );
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse( Array<Token>& tokens, Module& module )
{
    prepare_tokens( tokens );

    auto it = tokens.begin();
    auto end = tokens.end();
    if ( auto error = parse_module_module( it, end, module ) )
        return error;

    while ( it != end )
    {
        if ( it->value == kw_enum )
        {
            if ( auto error = parse_module_enum( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_layer )
        {
            if ( auto error = parse_module_layer( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_struct )
        {
            if ( auto error = parse_module_struct( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_func )
        {
            if ( auto error = parse_module_function( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_oper )
        {
            if ( auto error = parse_module_operator( it, end, module ) )
                return error;
        }
        else if ( it->value == kw_let || it->value == kw_var )
        {
            if ( auto error = parse_module_variable( it, end, module ) )
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

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_module( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    if ( it->value != kw_module )
        return ParseError{ *it, L"expected module keyword" };
    ++it;

    if ( it->type != TokenType::NAME )
        return ParseError{ *it, L"expected module name" };
    module.name = it->value;
    ++it;

    if ( it->value != op_expr_end )
        return ParseError{ *it, L"expected expression end" };
    ++it;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    StructType struct_type;
    if ( auto error = parse_struct( it, end, struct_type ) )
        return error;

    if ( module.contains_id( struct_type.name ) )
        return ParseError{ {}, L"name [" + struct_type.name + L"] already in use" };

    module.structs[struct_type.name] = struct_type;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    LayerType layer_type;
    if ( auto error = parse_layer( it, end, layer_type ) )
        return error;

    if ( module.contains_id( layer_type.name ) )
        return ParseError{ {}, L"name [" + layer_type.name + L"] already in use" };

    module.layers[layer_type.name] = layer_type;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    EnumType enum_type;
    if ( auto error = parse_enum( it, end, enum_type ) )
        return error;

    if ( module.contains_id( enum_type.name ) )
        return ParseError{ {}, L"name [" + enum_type.name + L"] already in use" };

    module.enums[enum_type.name] = enum_type;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Function function;
    if ( auto error = parse_function( it, end, function ) )
        return error;

    if ( module.contains_id( function.name ) )
        return ParseError{ {}, L"name [" + function.name + L"] already in use" };

    module.functions[function.name] = function;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Operator operato;
    if ( auto error = parse_operator( it, end, operato ) )
        return error;

    module.operators[operato.name] = operato;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module )
{
    Variable variable;
    if ( auto error = parse_variable( it, end, variable ) )
        return error;

    if ( module.contains_id( variable.name ) )
        return ParseError{ {}, L"name [" + variable.name + L"] already in use" };

    module.variables[variable.name] = variable;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, StructType& struct_type )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, LayerType& layer_type )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, EnumType& enum_type )
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

        if ( it->type != TokenType::NAME )
            return ParseError{ *it, L"expected arg name" };
        String name = it->value;
        ++it;

        if ( it->value != op_link )
            return ParseError{ *it, L"expected link op" };
        ++it;

        Ref<Type> type;
        if ( auto error = parse_type( it, end, type ) )
            return error;

        if ( args.contains( name ) )
            return ParseError{ *it, L"argument [", name, L"] already defined" };
        args.insert( name );

        function.args.emplace_back( std::move( name ), std::move( type ) );

        if ( it->value == op_split )
            ++it;
    }

    if ( it->value == op_link )
    {
        ++it;
        if ( auto error = parse_type( it, end, function.type ) )
            return error;
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
    if ( it->value != kw_let && it->value != kw_var )
        return ParseError{ *it, L"expected let or var keyword" };
    variable.is_var = (it->value == kw_var);
    ++it;

    if ( it->type != TokenType::NAME )
        return ParseError{ *it, L"expected variable name" };
    variable.name = it->value;
    ++it;

    if ( it->value == op_link )
    {
        ++it;
        if ( auto error = parse_type( it, end, variable.type ) )
            return error;
    }

    if ( it->value != op_assign )
        return ParseError{ *it, L"expected variable assignment" };
    ++it;

    if ( auto error = parse_expression( it, end, variable.expr ) )
        return error;

    if ( it->value != op_expr_end )
        return ParseError{ *it, L"expected expression end" };
    ++it;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_type( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type )
{
    Ref<Type> child_type;
    if ( auto error = type_basic( it, end, child_type ) )
        return error;

    if ( it->value == op_array_opn )
    {
        ++it;
        if ( it->value != op_array_cls )
            return ParseError{ *it, L"expected array end" };
        ++it;

        auto array_type = std::make_shared<ArrayType>();
        array_type->type = child_type;
        child_type = array_type;
    }

    Ref<RefType> ref_type;
    if ( auto error = type_reference( it, end, ref_type ) )
        return error;

    if ( ref_type )
    {
        Ref<RefType> p = ref_type;
        while ( p->type )
            p = std::dynamic_pointer_cast<RefType>(p->type);

        p->type = child_type;
        type = ref_type;
    }
    else
    {
        type = child_type;
    }

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::type_basic( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type )
{
    if ( it->value == tp_bool )
    {
        type = std::make_shared<BoolType>();
    }
    else if ( it->value == tp_int )
    {
        type = std::make_shared<IntType>();
    }
    else if ( it->value == tp_float )
    {
        type = std::make_shared<FloatType>();
    }
    else if ( it->value == tp_char )
    {
        type = std::make_shared<CharType>();
    }
    else if ( it->value == tp_string )
    {
        type = std::make_shared<StringType>();
    }
    else if ( it->type == TokenType::TYPE )
    {
        type = std::make_shared<Type>();
        type->name = it->value;
    }
    else if ( it->value == op_range )
    {
        type = std::make_shared<RangeType>();
    }
    else
    {
        return ParseError{ *it, L"invalid type" };
    }
    ++it;

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::type_reference( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<RefType>& type )
{
    if ( it->value != kw_let && it->value != kw_var )
        return std::nullopt;

    Ref<RefType> child_type;
    if ( it->value == kw_var )
    {
        child_type = std::make_shared<VarRefType>();
    }
    else
    {
        child_type = std::make_shared<LetRefType>();
    }
    ++it;

    if ( it->value != op_ref )
        return ParseError{ *it, L"expected reference after let or var" };
    ++it;

    Ref<RefType> parent_type;
    if ( auto error = type_reference( it, end, parent_type ) )
        return error;

    if ( parent_type )
    {
        Ref<RefType> p = parent_type;
        while ( p->type )
            p = std::dynamic_pointer_cast<RefType>(p->type);

        p->type = child_type;
        type = parent_type;
    }
    else
    {
        type = child_type;
    }

    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_expression( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
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
    for ( ; it != end; ++it )
    {
        if ( it->value == op_scope_opn )
        {
            if ( first_it == it )
                break;

            if ( (it - 1)->type != TokenType::TYPE )
                break;
        }

        if ( it->value == op_expr_opn || it->value == op_scope_opn || it->value == op_array_opn || it->value == op_yield_opn )
        {
            ++expr_depth;
        }
        else if ( it->value == op_expr_cls || it->value == op_scope_cls || it->value == op_array_cls || it->value == op_yield_cls )
        {
            --expr_depth;
            if ( expr_depth < 0 )
                break;
        }

        if ( expr_depth == 0 )
        {
            if ( it->value == op_expr_end )
                break;

            if ( it->value == op_split )
                break;
        }
        tokens.push_back( *it );
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

        if ( token.value == op_expr_opn || token.value == op_scope_opn || token.value == op_array_opn || token.value == op_yield_opn )
        {
            ++expr_depth;
        }
        else if ( token.value == op_expr_cls || token.value == op_scope_cls || token.value == op_array_cls || token.value == op_yield_cls )
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
    else if ( tokens.front().value == op_yield_opn )
    {
        if ( auto error = expression_yield( tokens, tree ) )
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
    else
    {
        return ParseError{ tokens[0], L"unknown expression token" };
    }

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
        value->value = std::stoll( token.value );
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::FLOAT )
    {
        auto value = FloatValue::make();
        value->value = std::stod( token.value );
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::CHAR )
    {
        auto value = CharValue::make();
        value->value = token.value[0];
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else if ( token.type == TokenType::STRING )
    {
        auto value = StringValue::make();
        value->value = token.value;
        auto node = std::make_shared<ValueNode>();
        node->value = value;
        tree = node;
    }
    else
    {
        return ParseError{ token, L"expected literal" };
    }

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
    {
        return ParseError{ token, L"keyword is not an expression" };
    }

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
    {
        return ParseError{ second_token, L"unknown type usage" };
    }

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

        if ( it->value == op_split )
            ++it;

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

    if ( auto error = type_basic( it, tokens.end(), node->type ) )
        return error;

    if ( it->value != op_array_opn )
        return ParseError{ *it, L"expected array size expression" };
    ++it;

    Array<Token> expr_tokens;
    if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
        return error;

    if ( expr_tokens.empty() )
    {
        if ( it->value != op_expr_end )
            return ParseError{ *it, L"expected expression end" };
        ++it;

        while ( true )
        {
            Array<Token> expr_tokens;
            if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
                return error;

            if ( expr_tokens.empty() )
                break;

            if ( it->value == op_split )
                ++it;

            Array<Token>::const_iterator expr_it = expr_tokens.begin();
            if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->_list.emplace_back() ) )
                return error;
        }

        node->init_type = ArrayNode::InitType::LIST;
    }
    else
    {
        {
            Array<Token>::const_iterator expr_it = expr_tokens.begin();
            if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->size ) )
                return error;
        }

        if ( it->value == op_expr_end )
        {
            ++it;

            Array<Token> expr_tokens;
            if ( auto error = expression_extract( it, tokens.end(), expr_tokens ) )
                return error;

            Array<Token>::const_iterator expr_it = expr_tokens.begin();
            if ( auto error = parse_expression( expr_it, expr_tokens.end(), node->_val ) )
                return error;

            node->init_type = ArrayNode::InitType::SIZE_VAL;
        }
        else
        {
            node->init_type = ArrayNode::InitType::SIZE;
        }
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

        if ( it->value == op_split )
            ++it;

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

dawn::Opt<dawn::ParseError> dawn::Parser::expression_yield( Array<Token> const& tokens, Ref<Node>& tree )
{
    assert( false && L"not impl" );
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

        if ( it->value == kw_let || it->value == kw_var )
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
            Ref<Node> expr;
            if ( auto error = scope_if( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
        else if ( it->value == kw_switch )
        {
            Ref<Node> expr;
            if ( auto error = scope_switch( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
        else if ( it->value == kw_for )
        {
            Ref<Node> expr;
            if ( auto error = scope_for( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
        else if ( it->value == kw_while )
        {
            Ref<Node> expr;
            if ( auto error = scope_while( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
        else if ( it->value == kw_loop )
        {
            Ref<Node> expr;
            if ( auto error = scope_loop( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
        }
        else if ( it->value == kw_return )
        {
            Ref<Node> expr;
            if ( auto error = scope_return( it, end, expr ) )
                return error;
            scope.instr.push_back( std::move( expr ) );
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

            if ( it->value != op_expr_end )
                return ParseError{ *it, L"expected expression end" };
            ++it;

            scope.instr.push_back( std::move( expr ) );
        }
    }

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

dawn::Opt<dawn::ParseError> dawn::Parser::scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    assert( false && L"not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree )
{
    if ( it->value != kw_return )
        return ParseError{ *it, L"expected return" };
    ++it;

    Ref node = std::make_shared<ReturnNode>();
    if ( auto error = parse_expression( it, end, node->expr ) )
        return error;

    if ( it->value != op_expr_end )
        return ParseError{ *it, L"expected expression end" };
    ++it;

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
    else if ( token.value == op_ref )
    {
        node = std::make_shared<UnaryNodeRef>();
    }
    else if ( token.value == op_range )
    {
        node = std::make_shared<UnaryNodeRange>();
    }
    else
    {
        return ParseError{ token, L"unknown unary operator" };
    }

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
    {
        return ParseError{ token, L"unknown operator" };
    }

    return std::nullopt;
}
