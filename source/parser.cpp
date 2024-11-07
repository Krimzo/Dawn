#include "parser.h"


std::wostream& dawn::operator<<(std::wostream& stream, const Error& error)
{
	stream << error.msg;
	return stream;
}

dawn::Opt<dawn::Error> dawn::Parser::parse(const Array<Token>& tokens, Module& module)
{
	auto it = tokens.begin();

	const auto end = tokens.end();
	if (auto error = parse_module_module(it, end, module))
		return error;

	while (it != end)
	{
		if (it->value == kw_internal) {
			if (auto error = parse_module_internal(it, end, module))
				return error;
		}
		else if (it->value == kw_enum) {
			if (auto error = parse_module_enum(it, end, module))
				return error;
		}
		else if (it->value == kw_struct) {
			if (auto error = parse_module_struct(it, end, module))
				return error;
		}
		else if (it->value == kw_interface) {
			if (auto error = parse_module_interface(it, end, module))
				return error;
		}
		else if (it->value == kw_class) {
			if (auto error = parse_module_class(it, end, module))
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
			return Error{ *it, L"not allowed in global scope or allowed only 1 instance of" };
		}
	}

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_module(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	if (it->value != kw_module)
		return Error{ *it, L"expected module keyword" };
	++it;

	if (it->type != TokenType::IDENTIFIER)
		return Error{ *it, L"expected module name" };
	module.name = it->value;
	++it;

	if (it->value != sep_end)
		return Error{ *it, L"expected expression end" };
	++it;

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_internal(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_enum(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_struct(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_interface(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_class(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_function(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	assert(false && "not implemented");
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_module_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module)
{
	Variable variable;
	if (auto error = parse_variable(it, end, variable))
		return error;
	if (m_is_module_internal) {
		module.global_internal.variables[variable.name] = variable;
	}
	else {
		module.global_public.variables[variable.name] = variable;
	}

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Variable& variable)
{
	if (it->value != kw_let && it->value != kw_var)
		return Error{ *it, L"expected let or var keyword" };
	variable.is_var = (it->value == kw_var);
	++it;

	if (it->type != TokenType::IDENTIFIER)
		return Error{ *it, L"expected variable name" };
	variable.name = it->value;
	++it;

	if (it->value == sep_static_access) {
		++it;
		if (auto error = parse_type(it, end, variable.type))
			return error;
	}

	if (it->value != sep_assign)
		return Error{ *it, L"expected variable assignment" };
	++it;

	if (auto error = parse_expression(it, end, variable.value))
		return error;

	if (it->value != sep_end)
		return Error{ *it, L"expected expression end" };
	++it;

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, String& type)
{
	if (it->type != TokenType::IDENTIFIER &&
		it->value != kw_bool &&
		it->value != kw_int &&
		it->value != kw_float &&
		it->value != kw_char &&
		it->value != kw_string)
		return Error{ *it, L"expected type" };
	type = it->value;
	++it;

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::parse_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Node>& tree)
{
	Array<Token> expr_tokens;
	if (auto error = extract_expression(it, end, expr_tokens))
		return error;

	Int least_prec_op = -1;
	if (auto error = find_least_precedence(expr_tokens, least_prec_op))
		return error;

	if (least_prec_op == 0) {
		if (expr_tokens.size() < 2)
			return Error{ *it, L"unary expected expression" };

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
			return Error{ *it, L"unary expected expression" };

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
		if (auto error = pure_expression(expr_tokens, tree))
			return error;
	}
	
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::pure_expression(const Array<Token>& tokens, Ref<Node>& tree)
{
	if (tokens.empty())
		return Error{ {}, L"expected pure expression" };

	if (tokens.size() == 1) {
		const auto& token = tokens[0];
		switch (token.type)
		{
			case TokenType::INT:
			case TokenType::FLOAT:
			case TokenType::CHAR:
			case TokenType::STRING:
				if (auto error = expression_literal(token, tree))
					return error;
				break;

			case TokenType::IDENTIFIER:
				if (auto error = expression_identifier(token, tree))
					return error;
				break;

			case TokenType::SEPARATOR:
				return Error{ token, L"separator is not an expression" };

			case TokenType::OPERATOR:
				return Error{ token, L"operator is not an expression" };
				 
			case TokenType::KEYWORD:
				if (auto error = expression_keyword(token, tree))
					return error;
				break;
		}
	}
	else if (tokens.front().value == sep_expr_open) {
		assert(false && "not implemented");
	}
	else if (tokens.front().value == sep_scope_open) {
		assert(false && "not implemented");
	}
	else if (tokens.front().value == sep_array_open) {
		assert(false && "not implemented");
	}
	else if (tokens.front().value == sep_yield_open) {
		assert(false && "not implemented");
	}
	else {
		return Error{ tokens[0], L"unknown expression token" };
	}

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::expression_literal(const Token& token, Ref<Node>& tree)
{
	if (token.type == TokenType::INT) {
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
		return Error{ token, L"expected literal" };
	}
	
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::expression_identifier(const Token& token, Ref<Node>& tree)
{
	auto node = std::make_shared<IdentifierNode>();
	node->name = token.value;
	tree = node;

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::expression_keyword(const Token& token, Ref<Node>& tree)
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
		auto value = std::make_shared<PointerValue>();
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
		return Error{ token, L"keyword is not an expression" };
	}
	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::extract_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Array<Token>& tokens)
{
	Int expr_depth = 0;
	const Array<Token>::const_iterator first_it = it;
	for (; it != end; ++it) {
		if (it->value == sep_expr_open || it->value == sep_scope_open || it->value == sep_array_open || it->value == sep_yield_open) {
			++expr_depth;
		}
		else if (it->value == sep_expr_close || it->value == sep_scope_close || it->value == sep_array_close || it->value == sep_yield_close) {
			--expr_depth;
			if (expr_depth < 0) {
				return Error{ *it, L"expected expression end" };
			}
		}
		if (expr_depth == 0) {
			if (it->value == sep_end) {
				break;
			}
			if (it->value == sep_scope_open) {
				if (first_it == it)
					break;
				Array<Token>::const_iterator temp_it = it;
				--temp_it;
				String ignored;
				if (auto error = parse_type(temp_it, temp_it + 1, ignored))
					break;
			}
		}
		tokens.push_back(*it);
	}
	if (expr_depth > 0)
		return Error{ *it, L"expected expression end" };

	return std::nullopt;
}

dawn::Opt<dawn::Error> dawn::Parser::find_least_precedence(const Array<Token>& tokens, Int& index)
{
	Int expr_depth = 0;
	bool was_op = false;
	Int least_precedence = -1;
	for (Int i = 0; i < (Int) tokens.size(); i++) {
		const auto& token = tokens[i];

		if (token.value == sep_expr_open || token.value == sep_scope_open || token.value == sep_array_open || token.value == sep_yield_open) {
			++expr_depth;
		}
		else if (token.value == sep_expr_close || token.value == sep_scope_close || token.value == sep_array_close || token.value == sep_yield_close) {
			--expr_depth;
			if (expr_depth < 0) {
				return Error{ token, L"expected expression end" };
			}
		}
		if (expr_depth != 0)
			continue;

		bool is_op = precedences.contains(token.value);
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

dawn::Ref<dawn::Node> dawn::UnaryNodeAddress::evaluate() const
{
	assert(false && "not implemented");
	return {};
}

dawn::Opt<dawn::Error> dawn::create_unary_node(const Token& token, Ref<UnaryNode>& node)
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
	else if (token.value == op_address) {
		node = std::make_shared<UnaryNodeAddress>();
	}
	else {
		return Error{ token, L"unknown unary operator" };
	}
	return std::nullopt;
}

dawn::Ref<dawn::Node> dawn::OperatorNodeStaticAccess::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAccess::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeAddress::evaluate()
{
	assert(false && "not implemented");
	return {};
}

dawn::Ref<dawn::Node> dawn::OperatorNodeNot::evaluate()
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

dawn::Opt<dawn::Error> dawn::create_operator_node(const Token& token, Ref<OperatorNode>& node)
{
	if (token.value == sep_static_access) {
		node = std::make_shared<OperatorNodeStaticAccess>();
	}
	else if (token.value == sep_access) {
		node = std::make_shared<OperatorNodeAccess>();
	}
	else if (token.value == op_address) {
		node = std::make_shared<OperatorNodeAddress>();
	}
	else if (token.value == op_not) {
		node = std::make_shared<OperatorNodeNot>();
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
	else if (token.value == op_less_eq) {
		node = std::make_shared<OperatorNodeLessEq>();
	}
	else if (token.value == op_great_eq) {
		node = std::make_shared<OperatorNodeGreatEq>();
	}
	else if (token.value == op_eq) {
		node = std::make_shared<OperatorNodeEq>();
	}
	else if (token.value == op_not_eq) {
		node = std::make_shared<OperatorNodeNotEq>();
	}
	else if (token.value == op_and) {
		node = std::make_shared<OperatorNodeAnd>();
	}
	else if (token.value == op_or) {
		node = std::make_shared<OperatorNodeOr>();
	}
	else if (token.value == sep_assign) {
		node = std::make_shared<OperatorNodeAssign>();
	}
	else if (token.value == op_add_as) {
		node = std::make_shared<OperatorNodeAddAs>();
	}
	else if (token.value == op_sub_as) {
		node = std::make_shared<OperatorNodeSubAs>();
	}
	else if (token.value == op_mul_as) {
		node = std::make_shared<OperatorNodeMulAs>();
	}
	else if (token.value == op_div_as) {
		node = std::make_shared<OperatorNodeDivAs>();
	}
	else if (token.value == op_pow_as) {
		node = std::make_shared<OperatorNodePowAs>();
	}
	else if (token.value == op_mod_as) {
		node = std::make_shared<OperatorNodeModAs>();
	}
	else if (token.value == sep_split) {
		node = std::make_shared<OperatorNodeSplit>();
	}
	else {
		return Error{ token, L"unknown operator" };
	}

	return std::nullopt;
}
