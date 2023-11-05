#pragma once

#include "lexer.h"


namespace dawn {
	struct ASTNode
	{
		virtual ~ASTNode() = default;
	};
}

namespace dawn {
	class Parser
	{
	public:
		Parser() = default;

		const std::vector<std::string>& get_errors() const;
		void print_errors() const;

		void assign_tokens(const std::vector<Token>& tokens);
		std::shared_ptr<ASTNode> parse();

	private:
		std::vector<Token> m_tokens = {};
		std::vector<std::string> m_errors = {};

		template<typename... Args>
		std::shared_ptr<ASTNode> log_error(const Args&... args)
		{
			std::stringstream stream = {};
			(stream << ... << args);
			m_errors.push_back(stream.str());
			return nullptr;
		}
	};
}
