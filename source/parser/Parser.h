#pragma once

#include "lexer/Lexer.h"


namespace dawn {
	class Parser {
		Array<Token> FixClassTypes(const Array<Token>& tokens) const;
		Array<Token> FixIdentifierTypes(const Array<Token>& tokens) const;

	public:
		Set<String>& typesRef;

		Parser(Set<String>& types);

		Array<Token> Parse(const Array<Token>& tokens) const;
	};
}
