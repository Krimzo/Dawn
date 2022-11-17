#pragma once

#include "lexer/lexer.h"
#include "parser/parser.h"


namespace faxdawn {
	class machine {
		std::unordered_set<std::string> types_ = syntax::types;
		lexer lexer_ = lexer(types_);
		parser parser_ = parser(types_);

	public:
		machine() = default;

		bool compile(const std::string& source) const;
		bool compile_file(const std::string& filepath) const;

		bool run(const std::vector<token>& tokens);
		bool run_file(const std::string& filepath);
	};
}
