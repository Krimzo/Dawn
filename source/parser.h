#pragma once

#include "lexer.h"


namespace dawn {
	struct ASTNode
	{
		virtual ~ASTNode() = default;
	};
}

namespace dawn {
	struct Scope
	{

	};
}

namespace dawn {
	class Parser
	{
	public:
		std::vector<std::string> errors = {};

		Parser() = default;

		std::shared_ptr<ASTNode> parse_expression(const std::vector<Token>& tokens);

	private:
		template<typename... Args>
		std::shared_ptr<ASTNode> log_error(const Args&... args)
		{
			std::stringstream stream = {};
			(stream << ... << args);
			errors.push_back(stream.str());
			return nullptr;
		}

		std::shared_ptr<ASTNode> parse_expression_keyword(const std::vector<Token>& tokens);
		std::shared_ptr<ASTNode> parse_expression_identifier(const std::vector<Token>& tokens);

	};
}
