#pragma once

#include "dawn/Syntax.h"


namespace dawn {
	class Lexer {
		Array<Token> Split(const String& source) const;
		void SaveStreamPart(StringStream& stream, Array<Token>& tokens, Int lineID) const;

		Array<Token> FixClassTypes(const Array<Token>& tokens) const;
		Array<Token> FixIdentifierTypes(const Array<Token>& tokens) const;

	public:
		Set<String>& typesRef;

		Lexer(Set<String>& typesRef);

		Array<Token> Generate(const String& source) const;
	};
}
