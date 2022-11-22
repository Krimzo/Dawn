#pragma once

#include "faxdawn/syntax.h"


namespace faxdawn {
	class lexer
	{
		std::unordered_set<std::string>& types_;

		[[nodiscard]] std::vector<token> split(const std::string& source) const;

	public:
		explicit lexer(std::unordered_set<std::string>& types);

		[[nodiscard]] std::vector<token> generate(const std::string& source) const;
	};
}
