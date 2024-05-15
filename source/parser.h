#pragma once

#include "lexer.h"
#include "ast.h"


namespace dawn {
	class Parser
	{
	public:
		Parser() = default;

		// Errors
		Bool has_errors() const;
		const Array<String>& get_errors() const;
		void print_errors() const;

		// Prepare tokens
		void assign_tokens(const Array<Token>& tokens);

		// Parser
		Object<GlobalASTNode> parse_global();
		Object<ASTNode> parse_scope();

		// Parse specific
		Object<ModuleASTNode> parse_module();
		Object<InternalASTNode> parse_internal();
		Object<InterfaceASTNode> parse_interface();
		Object<ClassASTNode> parse_class();
		Object<StructASTNode> parse_struct();
		Object<EnumASTNode> parse_enum();
		Object<DefASTNode> parse_def();
		Object<LetASTNode> parse_let();
		Object<VarASTNode> parse_var();
		Object<TypeASTNode> parse_type();

		// Expressions
		Object<ASTNode> parse_expression(const Array<Token>& tokens);

	private:
		Array<String> m_errors = {};
		Array<Token> m_tokens = {};
		Array<Token>::iterator m_current = {};

		template<typename... Args>
		nullptr_t log_error(const Args&... args)
		{
			StringStream stream;
			(stream << ... << args);
			m_errors.push_back(stream.str());
			return nullptr;
		}

		template<TokenType Type>
		Bool peek_token(const String& value = {})
		{
			if (m_current == m_tokens.end()) { return false; }
			if (m_current->type != Type) { return false; }
			if (!value.empty() && value != m_current->value) { return false; }
			return true;
		}

		template<TokenType Type>
		Bool peek_token(const Char value)
		{
			return peek_token<Type>(String(1, value));
		}

		inline Token consume_token()
		{
			return *m_current++;
		}
	};
}
