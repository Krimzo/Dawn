#pragma once

#include "dawn/Syntax.h"


namespace dawn {
	class Lexer {
		void SaveStreamPart(StringStream& stream, Array<Token>& tokens, Int lineID) const;

	public:
		Set<String>& typesRef;

		Lexer(Set<String>& typesRef);

		Array<Token> Split(const String& source) const;
		Array<Token> Generate(const String& source) const;
	};
}
