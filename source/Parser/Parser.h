#pragma once

#include "Faxdawn/Faxdawn.h"
#include "Lexer/Token.h"
#include <list>


namespace Faxdawn {
	class Parser {
	private:
		std::vector<Token> validate_identifiers(const std::vector<Token>& tokens) const;

	public:
		Parser();
		Parser(const Parser&) = delete;
		void operator=(const Parser&) = delete;
		~Parser();

		std::vector<Token> parse(const std::vector<Token>& tokens) const;
	};
}
