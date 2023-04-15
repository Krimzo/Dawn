#pragma once

#include "lexer/Lexer.h"


namespace dawn {
	class Parser {
	public:
		Set<String>& typesRef;

		Parser(Set<String>& types);

		Array<Token> Parse(const Array<Token>& tokens) const;
	};
}
