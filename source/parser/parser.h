#pragma once

#include "faxdawn/syntax.h"
#include "lexer/lexer.h"


namespace faxdawn {
	class parser
	{
		std::unordered_set<std::string>& types_;

		std::vector<token> fix_class_types(const std::vector<token>& tokens) const;
		std::vector<token> fix_identifier_types(const std::vector<token>& tokens) const;
		
		static std::vector<token> fix_array_types(const std::vector<token>& tokens);

	public:
		parser(std::unordered_set<std::string>& types);

		std::vector<token> parse(const std::vector<token>& tokens) const;
	};
}
