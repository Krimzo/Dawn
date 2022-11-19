#pragma once

#include "faxdawn/syntax.h"


namespace faxdawn {
	class lexer
	{
		std::unordered_set<std::string>& types_;

		std::vector<token> split(const std::string& source) const;

	public:
		lexer(std::unordered_set<std::string>& types);

		std::vector<token> generate(const std::string& source) const;
	};
}
