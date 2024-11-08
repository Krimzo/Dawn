#include "parser.h"


std::wostream& dawn::operator<<(std::wostream& stream, const ParseError& error)
{
	stream << error.msg;
	return stream;
}

dawn::Bool dawn::Space::contains_id(const StringRef& id) const
{
	const String id_str{ id };
	return variables.contains(id_str) ||
		functions.contains(id_str) ||
		enums.contains(id_str) ||
		layers.contains(id_str) ||
		structs.contains(id_str);
}

dawn::Bool dawn::Module::contains_id(const StringRef& id) const
{
	return space_public.contains_id(id) || space_internal.contains_id(id);
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse(Array<Token>& tokens, Module& module)
{
	prepare_tokens(tokens);
	auto it = tokens.begin();

	const auto end = tokens.end();
	if (auto error = parse_module_module(it, end, module))
		return error;

	while (it != end) {
		if (it->value == kw_internal) {
			if (auto error = parse_module_internal(it, end, module))
				return error;
		}
		else if (it->value == kw_enum) {
			if (auto error = parse_module_enum(it, end, module))
				return error;
		}
		else if (it->value == kw_layer) {
			if (auto error = parse_module_layer(it, end, module))
				return error;
		}
		else if (it->value == kw_struct) {
			if (auto error = parse_module_struct(it, end, module))
				return error;
		}
		else if (it->value == kw_func) {
			if (auto error = parse_module_function(it, end, module))
				return error;
		}
		else if (it->value == kw_let || it->value == kw_var) {
			if (auto error = parse_module_variable(it, end, module))
				return error;
		}
		else {
			return ParseError{ *it, L"not allowed in global scope or allowed only 1 instance of" };
		}
	}

	return std::nullopt;
}

void dawn::Parser::prepare_tokens(Array<Token>& tokens)
{
	for (Int i = 0; i < (Int) tokens.size() - 1; i++) {
		if (tokens[i].type == TokenType::NAME && tokens[i + 1].value == op_expr_opn) {
			tokens[i].type = TokenType::FUNCTION;
		}
	}
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_module(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	if (it->value != kw_module)
		return ParseError{ *it, L"expected module keyword" };
	++it;

	if (it->type != TokenType::NAME)
		return ParseError{ *it, L"expected module name" };
	module.name = it->value;
	++it;

	if (it->value != op_expr_end)
		return ParseError{ *it, L"expected expression end" };
	++it;

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_internal(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_enum(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_layer(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_struct(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_function(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_module_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	Variable variable;
	if (auto error = parse_variable(it, end, variable))
		return error;

	if (m_is_module_internal) {
		module.space_internal.variables[variable.name] = variable;
	}
	else {
		module.space_public.variables[variable.name] = variable;
	}

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Variable& variable)
{
	if (it->value != kw_let && it->value != kw_var)
		return ParseError{ *it, L"expected let or var keyword" };
	variable.is_var = (it->value == kw_var);
	++it;

	if (it->type != TokenType::NAME)
		return ParseError{ *it, L"expected variable name" };
	variable.name = it->value;
	++it;

	if (it->value == op_link) {
		++it;
		if (auto error = parse_type(it, end, variable.type))
			return error;
	}

	if (it->value != op_assign)
		return ParseError{ *it, L"expected variable assignment" };
	++it;

	if (auto error = parse_expression(it, end, variable.value))
		return error;

	if (it->value != op_expr_end)
		return ParseError{ *it, L"expected expression end" };
	++it;

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Type>& type)
{
	Ref<Type> child_type;
	if (it->value == tp_bool) {
		child_type = std::make_shared<BoolType>();
	}
	else if (it->value == tp_int) {
		child_type = std::make_shared<IntType>();
	}
	else if (it->value == tp_float) {
		child_type = std::make_shared<FloatType>();
	}
	else if (it->value == tp_char) {
		child_type = std::make_shared<CharType>();
	}
	else if (it->value == tp_string) {
		child_type = std::make_shared<StringType>();
	}
	else if (it->type == TokenType::TYPE) {
		child_type = std::make_shared<Type>();
		child_type->name = it->value;
	}
	else if (it->value == op_range) {
		child_type = std::make_shared<RangeType>();
	}
	else {
		return ParseError{ *it, L"invalid type" };
	}
	++it;

	if (it->value == op_array_opn) {
		++it;

		if (it->value != op_array_cls)
			return ParseError{ *it, L"expected array end" };
		++it;

		auto array_type = std::make_shared<ArrayType>();
		array_type->type = child_type;
		child_type = array_type;
	}

	Ref<RefType> ref_type;
	if (auto error = parse_reference_type(it, end, ref_type))
		return error;

	if (ref_type) {
		Ref<RefType> p = ref_type;
		while (p->type) {
			p = std::dynamic_pointer_cast<RefType>(p->type);
		}
		p->type = child_type;
		type = ref_type;
	}
	else {
		type = child_type;
	}
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_reference_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<RefType>& type)
{
	if (it->value != kw_let && it->value != kw_var)
		return std::nullopt;

	Ref<RefType> child_type;
	if (it->value == kw_var) {
		child_type = std::make_shared<VarRefType>();
	}
	else {
		child_type = std::make_shared<LetRefType>();
	}
	++it;

	if (it->value != op_ref)
		return ParseError{ *it, L"expected reference after let or var" };
	++it;

	Ref<RefType> parent_type;
	if (auto error = parse_reference_type(it, end, parent_type))
		return error;

	if (parent_type) {
		Ref<RefType> p = parent_type;
		while (p->type) {
			p = std::dynamic_pointer_cast<RefType>(p->type);
		}
		p->type = child_type;
		type = parent_type;
	}
	else {
		type = child_type;
	}
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::parse_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Node>& tree)
{
	Array<Token> expr_tokens;
	if (auto error = expression_extract(it, end, expr_tokens))
		return error;

	Int least_prec_op = -1;
	if (auto error = expression_precedence(expr_tokens, least_prec_op))
		return error;

	if (least_prec_op == 0) {
		if (expr_tokens.size() < 2)
			return ParseError{ *it, L"unary expected expression" };

		Ref<UnaryNode> node;
		if (auto error = create_unary_node(expr_tokens.front(), node))
			return error;

		auto it = expr_tokens.begin() + 1;
		if (auto error = parse_expression(it, expr_tokens.end(), node->right))
			return error;

		tree = node;
	}
	else if (least_prec_op > 0) {
		if ((Int) expr_tokens.size() < least_prec_op + 2)
			return ParseError{ *it, L"unary expected expression" };

		Ref<OperatorNode> node;
		if (auto error = create_operator_node(expr_tokens[least_prec_op], node))
			return error;

		auto it = expr_tokens.begin();
		if (auto error = parse_expression(it, expr_tokens.begin() + least_prec_op, node->left))
			return error;

		it = expr_tokens.begin() + (least_prec_op + 1);
		if (auto error = parse_expression(it, expr_tokens.end(), node->right))
			return error;

		tree = node;
	}
	else {
		if (auto error = expression_pure(expr_tokens, tree))
			return error;
	}
	
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_extract(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Array<Token>& tokens)
{
	Int expr_depth = 0;
	const Array<Token>::const_iterator first_it = it;
	for (; it != end; ++it) {
		if (it->value == op_expr_opn || it->value == op_scope_opn || it->value == op_array_opn || it->value == op_yield_opn) {
			++expr_depth;
		}
		else if (it->value == op_expr_cls || it->value == op_scope_cls || it->value == op_array_cls || it->value == op_yield_cls) {
			--expr_depth;
			if (expr_depth < 0)
				return ParseError{ *it, L"expected expression end" };
		}
		if (expr_depth == 0) {
			if (it->value == op_expr_end)
				break;

			if (it->value == op_scope_opn) {
				if (first_it == it)
					break;

				Array<Token>::const_iterator temp_it = it;
				--temp_it;
				Ref<Type> _type;
				if (auto error = parse_type(temp_it, temp_it + 1, _type))
					break;
			}
		}
		tokens.push_back(*it);
	}
	if (expr_depth > 0)
		return ParseError{ *it, L"expected expression end" };

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_precedence(const Array<Token>& tokens, Int& index)
{
	Int expr_depth = 0;
	Bool was_op = false;
	Int least_precedence = -1;
	for (Int i = 0; i < (Int) tokens.size(); i++) {
		const auto& token = tokens[i];

		if (token.value == op_expr_opn || token.value == op_scope_opn || token.value == op_array_opn || token.value == op_yield_opn) {
			++expr_depth;
		}
		else if (token.value == op_expr_cls || token.value == op_scope_cls || token.value == op_array_cls || token.value == op_yield_cls) {
			--expr_depth;
			if (expr_depth < 0)
				return ParseError{ token, L"expected expression end" };
		}
		if (expr_depth != 0)
			continue;

		Bool is_op = precedences.contains(token.value);
		if (is_op && was_op)
			continue;

		was_op = is_op;
		if (!is_op)
			continue;

		Int prec = precedences.at(token.value);
		if (prec <= least_precedence)
			continue;

		least_precedence = prec;
		index = i;
	}

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_pure(const Array<Token>& tokens, Ref<Node>& tree)
{
	if (tokens.empty())
		return ParseError{ {}, L"expected pure expression" };

	if (tokens.size() == 1) {
		if (auto error = expression_single(tokens[0], tree))
			return error;
	}
	else if (tokens.front().type == TokenType::TYPE) {
		assert(false && "not implemented");
	}
	else if (tokens.front().type == TokenType::FUNCTION) {
		assert(false && "not implemented");
	}
	else if (tokens.front().type == TokenType::NAME) {
		assert(false && "not implemented");
	}
	else if (tokens.front().value == op_expr_opn) {
		if (tokens.back().value != op_expr_cls)
			return ParseError{ tokens.back(), L"expected expression close" };

		auto begin = tokens.begin() + 1;
		auto end = tokens.begin() + tokens.size() - 1;
		return parse_expression(begin, end, tree);
	}
	else if (tokens.front().value == op_yield_opn) {
		assert(false && "not implemented");
	}
	else {
		return ParseError{ tokens[0], L"unknown expression token" };
	}

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single(const Token& token, Ref<Node>& tree)
{
	switch (token.type)
	{
	case TokenType::INTEGER:
	case TokenType::FLOAT:
	case TokenType::CHAR:
	case TokenType::STRING:
		if (auto error = expression_single_literal(token, tree))
			return error;
		break;

	case TokenType::KEYWORD:
		if (auto error = expression_single_keyword(token, tree))
			return error;
		break;

	case TokenType::TYPE:
		return ParseError{ token, L"type is not an expression" };

	case TokenType::FUNCTION:
		return ParseError{ token, L"function is not an expression" };

	case TokenType::NAME:
		if (auto error = expression_single_identifier(token, tree))
			return error;
		break;

	case TokenType::OPERATOR:
		return ParseError{ token, L"operator is not an expression" };
	}

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_literal(const Token& token, Ref<Node>& tree)
{
	if (token.type == TokenType::INTEGER) {
		auto value = std::make_shared<IntValue>();
		value->value = std::stoll(token.value);
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.type == TokenType::FLOAT) {
		auto value = std::make_shared<FloatValue>();
		value->value = std::stod(token.value);
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.type == TokenType::CHAR) {
		auto value = std::make_shared<CharValue>();
		value->value = token.value[0];
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.type == TokenType::STRING) {
		auto value = std::make_shared<StringValue>();
		value->value = token.value;
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else {
		return ParseError{ token, L"expected literal" };
	}
	
	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_keyword(const Token& token, Ref<Node>& tree)
{
	if (token.value == kw_true) {
		auto value = std::make_shared<BoolValue>();
		value->value = true;
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.value == kw_false) {
		auto value = std::make_shared<BoolValue>();
		value->value = false;
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.value == kw_null) {
		auto value = std::make_shared<ReferenceValue>();
		value->value = nullptr;
		auto node = std::make_shared<ValueNode>();
		node->value = value;
		tree = node;
	}
	else if (token.value == kw_self) {
		auto node = std::make_shared<SelfNode>();
		tree = node;
	}
	else {
		return ParseError{ token, L"keyword is not an expression" };
	}

	return std::nullopt;
}

dawn::Opt<dawn::ParseError> dawn::Parser::expression_single_identifier(const Token& token, Ref<Node>& tree)
{
	auto node = std::make_shared<IdentifierNode>();
	node->name = token.value;
	tree = node;

	return std::nullopt;
}

dawn::Ref<dawn::Node> dawn::UnaryNodePlus::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::UnaryNodeMinus::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::UnaryNodeNot::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::UnaryNodeRef::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::UnaryNodeRange::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Opt<dawn::ParseError> dawn::create_unary_node(const Token& token, Ref<UnaryNode>& node)
{
	if (token.value == op_add) {
		node = std::make_shared<UnaryNodePlus>();
	}
	else if (token.value == op_sub) {
		node = std::make_shared<UnaryNodeMinus>();
	}
	else if (token.value == op_not) {
		node = std::make_shared<UnaryNodeNot>();
	}
	else if (token.value == op_ref) {
		node = std::make_shared<UnaryNodeRef>();
	}
	else if (token.value == op_range) {
		node = std::make_shared<UnaryNodeRange>();
	}
	else {
		return ParseError{ token, L"unknown unary operator" };
	}

	return std::nullopt;
}

dawn::Ref<dawn::Node> dawn::OperatorNodeLink::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAccess::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeRange::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodePow::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeMod::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeMul::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeDiv::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAdd::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeSub::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeLess::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeGreat::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeLessEq::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeGreatEq::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeEq::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeNotEq::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAnd::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeOr::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAssign::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAddAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeSubAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeMulAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeDivAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodePowAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeModAs::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeSplit::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Opt<dawn::ParseError> dawn::create_operator_node(const Token& token, Ref<OperatorNode>& node)
{
	if (token.value == op_link) {
		node = std::make_shared<OperatorNodeLink>();
	}
	else if (token.value == op_access) {
		node = std::make_shared<OperatorNodeAccess>();
	}
	else if (token.value == op_range) {
		node = std::make_shared<OperatorNodeRange>();
	}
	else if (token.value == op_pow) {
		node = std::make_shared<OperatorNodePow>();
	}
	else if (token.value == op_mod) {
		node = std::make_shared<OperatorNodeMod>();
	}
	else if (token.value == op_mul) {
		node = std::make_shared<OperatorNodeMul>();
	}
	else if (token.value == op_div) {
		node = std::make_shared<OperatorNodeDiv>();
	}
	else if (token.value == op_add) {
		node = std::make_shared<OperatorNodeAdd>();
	}
	else if (token.value == op_sub) {
		node = std::make_shared<OperatorNodeSub>();
	}
	else if (token.value == op_less) {
		node = std::make_shared<OperatorNodeLess>();
	}
	else if (token.value == op_great) {
		node = std::make_shared<OperatorNodeGreat>();
	}
	else if (token.value == op_lesseq) {
		node = std::make_shared<OperatorNodeLessEq>();
	}
	else if (token.value == op_greateq) {
		node = std::make_shared<OperatorNodeGreatEq>();
	}
	else if (token.value == op_eq) {
		node = std::make_shared<OperatorNodeEq>();
	}
	else if (token.value == op_neq) {
		node = std::make_shared<OperatorNodeNotEq>();
	}
	else if (token.value == op_and) {
		node = std::make_shared<OperatorNodeAnd>();
	}
	else if (token.value == op_or) {
		node = std::make_shared<OperatorNodeOr>();
	}
	else if (token.value == op_assign) {
		node = std::make_shared<OperatorNodeAssign>();
	}
	else if (token.value == op_addas) {
		node = std::make_shared<OperatorNodeAddAs>();
	}
	else if (token.value == op_subas) {
		node = std::make_shared<OperatorNodeSubAs>();
	}
	else if (token.value == op_mulas) {
		node = std::make_shared<OperatorNodeMulAs>();
	}
	else if (token.value == op_divas) {
		node = std::make_shared<OperatorNodeDivAs>();
	}
	else if (token.value == op_powas) {
		node = std::make_shared<OperatorNodePowAs>();
	}
	else if (token.value == op_modas) {
		node = std::make_shared<OperatorNodeModAs>();
	}
	else if (token.value == op_split) {
		node = std::make_shared<OperatorNodeSplit>();
	}
	else {
		return ParseError{ token, L"unknown operator" };
	}

	return std::nullopt;
}
