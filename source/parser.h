#pragma once

#include "lexer.h"
#include "ast.h"


namespace dawn {
	class Parser
	{
	public:
		Parser() = default;

		// Errors
		bool has_errors() const;
		void print_errors() const;
		const std::vector<std::string>& get_errors() const;

		// Prepare tokens
		void assign_tokens(const std::vector<Token>& tokens);

		// Parser
		std::shared_ptr<GlobalASTNode> parse_global();
		std::shared_ptr<ScopeASTNode> parse_scope();
		std::shared_ptr<ASTNode> parse_expression();

		// Parse specific
		std::shared_ptr<ModuleASTNode> parse_module();
		std::shared_ptr<InternalASTNode> parse_internal();
		std::shared_ptr<LetASTNode> parse_let();
		std::shared_ptr<VarASTNode> parse_var();
		std::shared_ptr<DefASTNode> parse_def();
		std::shared_ptr<EnumASTNode> parse_enum();
		std::shared_ptr<StructASTNode> parse_struct();
		std::shared_ptr<InterfaceASTNode> parse_interface();
		std::shared_ptr<ClassASTNode> parse_class();

	private:
		std::vector<std::string> m_errors = {};
		std::vector<Token> m_tokens = {};
		std::vector<Token>::iterator m_current = {};

		template<typename... Args>
		nullptr_t log_error(const Args&... args)
		{
			std::stringstream stream = {};
			(stream << ... << args);
			m_errors.push_back(stream.str());
			return nullptr;
		}

		template<TokenType Type>
		bool peek_token(const std::string& value = {})
		{
			if (m_current == m_tokens.end()) { return false; }
			if (m_current->type != Type) { return false; }
			if (!value.empty() && value != m_current->value) { return false; }
			return true;
		}

		template<TokenType Type>
		bool peek_token(const char value)
		{
			return peek_token<Type>(std::string(1, value));
		}

		inline Token consume_token()
		{
			return *m_current++;
		}
	};
}
