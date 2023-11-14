#include "parser.h"


// Errors
bool dawn::Parser::has_errors() const
{
	return !m_errors.empty();
}

void dawn::Parser::print_errors() const
{
	for (const auto& error : m_errors) {
		std::cout << error << std::endl;
	}
}

const std::vector<std::string>& dawn::Parser::get_errors() const
{
	return m_errors;
}

// Prepare tokens
void dawn::Parser::assign_tokens(const std::vector<Token>& tokens)
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
std::shared_ptr<dawn::GlobalASTNode> dawn::Parser::parse_global()
{
	std::shared_ptr node = std::make_shared<GlobalASTNode>();
	while (m_current != m_tokens.end()) {
		// Handle module
		if (peek_token<TokenType::KEYWORD>(kw_module)) {
			// Only one module can be defined per file/source
			if (node->module) {
				return log_error("Module already defined (", node->module->module_name, ")");
			}

			// Parse module
			std::shared_ptr module_ast = parse_module();
			if (!module_ast) {
				return log_error("Failed to parse (", kw_module, ")");
			}
			node->module = module_ast;
			continue;
		}

		// Handle internal scope
		if (peek_token<TokenType::KEYWORD>(kw_internal)) {
			std::shared_ptr internal_ast = parse_internal();
			if (!internal_ast) {
				return log_error("Failed to parse (", kw_internal, ")");
			}
			node->internals.push_back(internal_ast);
			continue;
		}

		// Handle global let
		if (peek_token<TokenType::KEYWORD>(kw_let)) {
			std::shared_ptr let_ast = parse_let();
			if (!let_ast) {
				return log_error("Failed to parse (", kw_let, ")");
			}
			node->lets.push_back(let_ast);
			continue;
		}

		// Handle global var
		if (peek_token<TokenType::KEYWORD>(kw_var)) {
			std::shared_ptr var_ast = parse_var();
			if (!var_ast) {
				return log_error("Failed to parse (", kw_var, ")");
			}
			node->vars.push_back(var_ast);
			continue;
		}

		// Handle function
		if (peek_token<TokenType::KEYWORD>(kw_def)) {
			std::shared_ptr def_ast = parse_def();
			if (!def_ast) {
				return log_error("Failed to parse (", kw_def, ")");
			}
			node->defs.push_back(def_ast);
			continue;
		}

		// Handle enum
		if (peek_token<TokenType::KEYWORD>(kw_enum)) {
			std::shared_ptr enum_ast = parse_enum();
			if (!enum_ast) {
				return log_error("Failed to parse (", kw_enum, ")");
			}
			node->enums.push_back(enum_ast);
			continue;
		}

		// Handle struct
		if (peek_token<TokenType::KEYWORD>(kw_struct)) {
			std::shared_ptr struct_ast = parse_struct();
			if (!struct_ast) {
				return log_error("Failed to parse (", kw_struct, ")");
			}
			node->structs.push_back(struct_ast);
			continue;
		}

		// Handle interface
		if (peek_token<TokenType::KEYWORD>(kw_interface)) {
			std::shared_ptr interface_ast = parse_interface();
			if (!interface_ast) {
				return log_error("Failed to parse (", kw_interface, ")");
			}
			node->interfaces.push_back(interface_ast);
			continue;
		}

		// Handle class
		if (peek_token<TokenType::KEYWORD>(kw_class)) {
			std::shared_ptr class_ast = parse_class();
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

std::shared_ptr<dawn::ScopeASTNode> dawn::Parser::parse_scope()
{
	return log_error("parse SCOPE not implemented");
}

std::shared_ptr<dawn::ASTNode> dawn::Parser::parse_expression()
{
	return log_error("parse EXPRESSION not implemented");;
}

// Parser helpers
std::shared_ptr<dawn::ModuleASTNode> dawn::Parser::parse_module()
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

	std::shared_ptr node = std::make_shared<ModuleASTNode>();
	node->module_name = identifier_token.value;
	return node;
}

std::shared_ptr<dawn::InternalASTNode> dawn::Parser::parse_internal()
{
	// Internal
	if (!peek_token<TokenType::KEYWORD>(kw_internal)) {
		return log_error("Expected a value (", kw_internal, ")");
	}
	consume_token();

	// Internal scope
	return log_error("parse INTERNAL not implemented");
}

std::shared_ptr<dawn::LetASTNode> dawn::Parser::parse_let()
{
	return log_error("parse LET not implemented");
}

std::shared_ptr<dawn::VarASTNode> dawn::Parser::parse_var()
{
	return log_error("parse VAR not implemented");
}

std::shared_ptr<dawn::DefASTNode> dawn::Parser::parse_def()
{
	return log_error("parse DEF not implemented");
}

std::shared_ptr<dawn::EnumASTNode> dawn::Parser::parse_enum()
{
	return log_error("parse ENUM not implemented");
}

std::shared_ptr<dawn::StructASTNode> dawn::Parser::parse_struct()
{
	return log_error("parse STRUCT not implemented");
}

std::shared_ptr<dawn::InterfaceASTNode> dawn::Parser::parse_interface()
{
	return log_error("parse INTERFACE not implemented");
}

std::shared_ptr<dawn::ClassASTNode> dawn::Parser::parse_class()
{
	return log_error("parse CLASS not implemented");
}
