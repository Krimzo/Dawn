#pragma once

#include "faxdawn/syntax.h"
#include "lexer/token.h"


namespace faxdawn {
	class lexer {
		std::vector<std::string>& types_;

		[[nodiscard]] std::vector<token> split(const std::string& source) const;

	public:
		explicit lexer(std::vector<std::string>& types);

		[[nodiscard]] std::vector<token> generate(const std::string& source) const;
	};
}