#include "parser.h"


// Errors
dawn::Bool dawn::Parser::has_errors() const
{
	return !m_errors.empty();
}

const dawn::Array<dawn::String>& dawn::Parser::get_errors() const
{
	return m_errors;
}

void dawn::Parser::print_errors() const
{
	for (const auto& error : m_errors) {
		std::wcout << error << std::endl;
	}
}

// Prepare tokens
void dawn::Parser::assign_tokens(const Array<Token>& tokens)
{
	m_errors.clear();
	m_tokens.clear();
	m_tokens.reserve(tokens.size() / 2);
	for (const auto& token : tokens) {
		if (token.type != TokenType::WHITESPACE) {
			m_tokens.push_back(token);
		}
	}
	m_current = m_tokens.begin();
}

// Global parser
dawn::Object<dawn::GlobalASTNode> dawn::Parser::parse_global()
{
	Object node = std::make_shared<GlobalASTNode>();
	while (m_current != m_tokens.end()) {
		// Handle module
		if (peek_token<TokenType::KEYWORD>(kw_module)) {
			// Only one module can be defined per file/source
			if (node->module) {
				return log_error("Module already defined (", node->module->name, ")");
			}

			// Parse module
			Object module_ast = parse_module();
			if (!module_ast) {
				return log_error("Failed to parse (", kw_module, ")");
			}
			node->module = module_ast;
			continue;
		}

		// Handle internal scope
		if (peek_token<TokenType::KEYWORD>(kw_internal)) {
			if (node->internal) {
				return log_error("Internal scope already exists");
			}
			Object internal_ast = parse_internal();
			if (!internal_ast) {
				return log_error("Failed to parse (", kw_internal, ")");
			}
			node->internal = internal_ast;
			continue;
		}

		// Handle global let
		if (peek_token<TokenType::KEYWORD>(kw_let)) {
			Object let_ast = parse_let();
			if (!let_ast) {
				return log_error("Failed to parse (", kw_let, ")");
			}
			node->lets.push_back(let_ast);
			continue;
		}

		// Handle global var
		if (peek_token<TokenType::KEYWORD>(kw_var)) {
			Object var_ast = parse_var();
			if (!var_ast) {
				return log_error("Failed to parse (", kw_var, ")");
			}
			node->vars.push_back(var_ast);
			continue;
		}

		// Handle function
		if (peek_token<TokenType::KEYWORD>(kw_def)) {
			Object def_ast = parse_def();
			if (!def_ast) {
				return log_error("Failed to parse (", kw_def, ")");
			}
			node->defs.push_back(def_ast);
			continue;
		}

		// Handle enum
		if (peek_token<TokenType::KEYWORD>(kw_enum)) {
			Object enum_ast = parse_enum();
			if (!enum_ast) {
				return log_error("Failed to parse (", kw_enum, ")");
			}
			node->enums.push_back(enum_ast);
			continue;
		}

		// Handle struct
		if (peek_token<TokenType::KEYWORD>(kw_struct)) {
			Object struct_ast = parse_struct();
			if (!struct_ast) {
				return log_error("Failed to parse (", kw_struct, ")");
			}
			node->structs.push_back(struct_ast);
			continue;
		}

		// Handle interface
		if (peek_token<TokenType::KEYWORD>(kw_interface)) {
			Object interface_ast = parse_interface();
			if (!interface_ast) {
				return log_error("Failed to parse (", kw_interface, ")");
			}
			node->interfaces.push_back(interface_ast);
			continue;
		}

		// Handle class
		if (peek_token<TokenType::KEYWORD>(kw_class)) {
			Object class_ast = parse_class();
			if (!class_ast) {
				return log_error("Failed to parse (", kw_class, ")");
			}
			node->classes.push_back(class_ast);
			continue;
		}

		// Error
		if (m_current != m_tokens.end()) {
			return log_error("Unknown global (", m_current->value, ") at line ", m_current->line_number);
		}
	}
	return node;
}

dawn::Object<dawn::ASTNode> dawn::Parser::parse_scope()
{
	return log_error("parse SCOPE not implemented");
}

// Parser helpers
dawn::Object<dawn::ModuleASTNode> dawn::Parser::parse_module()
{
	// Module
	if (!peek_token<TokenType::KEYWORD>(kw_module)) {
		return log_error("Expected a value (", kw_module, ")");
	}
	consume_token();

	// Identifier
	if (!peek_token<TokenType::IDENTIFIER>()) {
		return log_error("Expected a value [IDENTIFIER]");
	}
	const Token identifier_token = consume_token();

	// Separator
	if (!peek_token<TokenType::SEPARATOR>(sep_express)) {
		return log_error("Expected a value (", sep_express, ")");
	}
	consume_token();

	Object node = std::make_shared<ModuleASTNode>();
	node->name = identifier_token.value;
	return node;
}

dawn::Object<dawn::InternalASTNode> dawn::Parser::parse_internal()
{
	// Internal
	if (!peek_token<TokenType::KEYWORD>(kw_internal)) {
		return log_error("Expected a value (", kw_internal, ")");
	}
	consume_token();

	// Internal scope
	return log_error("parse INTERNAL not implemented");
}

dawn::Object<dawn::InterfaceASTNode> dawn::Parser::parse_interface()
{
	return log_error("parse INTERFACE not implemented");
}

dawn::Object<dawn::ClassASTNode> dawn::Parser::parse_class()
{
	return log_error("parse CLASS not implemented");
}

dawn::Object<dawn::StructASTNode> dawn::Parser::parse_struct()
{
	return log_error("parse STRUCT not implemented");
}

dawn::Object<dawn::EnumASTNode> dawn::Parser::parse_enum()
{
	return log_error("parse ENUM not implemented");
}

dawn::Object<dawn::DefASTNode> dawn::Parser::parse_def()
{
	return log_error("parse DEF not implemented");
}

dawn::Object<dawn::LetASTNode> dawn::Parser::parse_let()
{
	// Let
	if (!peek_token<TokenType::KEYWORD>(kw_let)) {
		return log_error("Expected a value (", kw_let, ")");
	}
	consume_token();

	// Identifier
	if (!peek_token<TokenType::IDENTIFIER>()) {
		return log_error("Expected a value [IDENTIFIER]");
	}
	const Token identifier_token = consume_token();

	// Type declaration
	Object<TypeASTNode> type;
	if (peek_token<TokenType::SEPARATOR>(sep_static_access)) {
		consume_token();
		type = parse_type();
	}

	// Separator
	if (!peek_token<TokenType::SEPARATOR>(sep_assign)) {
		return log_error("Expected a value (", sep_assign, ")");
	}
	consume_token();

	// Expression
	Array<Token> expression_tokens;
	while (!peek_token<TokenType::SEPARATOR>(sep_express)) {
		expression_tokens.push_back(consume_token());
	}
	const Object expr_node = parse_expression(expression_tokens);

	// Separator
	if (!peek_token<TokenType::SEPARATOR>(sep_express)) {
		return log_error("Expected a value (", sep_express, ")");
	}

	Object node = std::make_shared<LetASTNode>();
	node->name = identifier_token.value;
	node->type = type;
	node->value = expr_node;
	return node;
}

dawn::Object<dawn::VarASTNode> dawn::Parser::parse_var()
{
	// Var
	if (!peek_token<TokenType::KEYWORD>(kw_var)) {
		return log_error("Expected a value (", kw_var, ")");
	}
	consume_token();

	// Identifier
	if (!peek_token<TokenType::IDENTIFIER>()) {
		return log_error("Expected a value [IDENTIFIER]");
	}
	const Token identifier_token = consume_token();

	// Type declaration
	Object<TypeASTNode> type;
	if (peek_token<TokenType::SEPARATOR>(sep_static_access)) {
		consume_token();
		type = parse_type();
	}

	// Separator
	if (!peek_token<TokenType::SEPARATOR>(sep_assign)) {
		return log_error("Expected a value (", sep_assign, ")");
	}
	consume_token();

	// Expression
	Array<Token> expression_tokens;
	while (!peek_token<TokenType::SEPARATOR>(sep_express)) {
		expression_tokens.push_back(consume_token());
	}
	const Object expr_node = parse_expression(expression_tokens);

	// Separator
	if (!peek_token<TokenType::SEPARATOR>(sep_express)) {
		return log_error("Expected a value (", sep_express, ")");
	}

	Object node = std::make_shared<VarASTNode>();
	node->name = identifier_token.value;
	node->type = type;
	node->value = expr_node;
	return node;
}

dawn::Object<dawn::TypeASTNode> dawn::Parser::parse_type()
{
	if (!peek_token<TokenType::KEYWORD>() && peek_token<TokenType::IDENTIFIER>()) {
		return log_error("Expected a value [KEYWORD or IDENTIFIER]");
	}
	const Token type = consume_token();

	PointerType pointer_type = PointerType::NONE;
	if (peek_token<TokenType::KEYWORD>(kw_let)) {
		consume_token();
		if (!peek_token<TokenType::OPERATOR>(op_mul)) {
			return log_error("Expected a {*} after a pointer type");
		}
		consume_token();
		pointer_type = PointerType::LET;
	}
	else if (peek_token<TokenType::KEYWORD>(kw_var)) {
		consume_token();
		if (!peek_token<TokenType::OPERATOR>(op_mul)) {
			return log_error("Expected a {*} after a pointer type");
		}
		consume_token();
		pointer_type = PointerType::VAR;
	}

	Object node = std::make_shared<TypeASTNode>();
	node->name = type.value;
	node->pointer_type = pointer_type;
	return node;
}

// Expressions
dawn::Object<dawn::ASTNode> dawn::Parser::parse_expression(const Array<Token>& tokens)
{
	Int op_index = -1;
	Int max_prec = 0;
	for (Int i = 0; i < (Int) tokens.size(); i++) {
		auto& token = tokens[i];
		if (precedences.contains(token.value)) {
			const Int prec = precedences.at(token.value);
			if (prec > max_prec) {
				max_prec = prec;
				op_index = i;
			}
		}
	}
	if (op_index > 0) {

	}
	

	/*
	let a: int = b = call(2 * a)->haha * 2;
	*/

	/*
	if call() {

	}
	*/

	return log_error("parse EXPRESSION not implemented");
}
