#pragma once

#include "faxdawn/syntax.h"
#include "lexer/lexer.h"


namespace faxdawn {
	class parser {
		std::unordered_set<std::string>& types_;

		[[nodiscard]] std::vector<token> fix_class_types(const std::vector<token>& tokens) const;
		[[nodiscard]] std::vector<token> fix_identifier_types(const std::vector<token>& tokens) const;
		
		static std::vector<token> fix_array_types(const std::vector<token>& tokens);

	public:
		explicit parser(std::unordered_set<std::string>& types);

		[[nodiscard]] std::vector<token> parse(const std::vector<token>& tokens) const;
	};
}
