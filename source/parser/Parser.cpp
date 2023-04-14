#include "parser/Parser.h"

#include "dawn/IO.h"


dawn::Parser::Parser(Set<String>& typesRef) : typesRef(typesRef) {}

dawn::Array<dawn::Token> dawn::Parser::Parse(const Array<Token>& tokens) const {
    Array<Token> parsedTokens = tokens;
	parsedTokens = FixClassTypes(parsedTokens);
	parsedTokens = FixIdentifierTypes(parsedTokens);
    return parsedTokens;
}

dawn::Array<dawn::Token> dawn::Parser::FixClassTypes(const Array<Token>& tokens) const {
	Array<Token> updatedTokens = {};
	updatedTokens.reserve(tokens.size());

	bool lastTokenWasTypeDecl = false;
	for (const auto& token : tokens) {
		Token updatedToken = token;

		if (lastTokenWasTypeDecl) {
			Assert(updatedToken.type != Identifier, updatedToken);
			typesRef.insert(updatedToken.value);
			updatedToken.type = Type;
			lastTokenWasTypeDecl = false;
		}
		else if (updatedToken.value == keyword::type_declaration) {
			lastTokenWasTypeDecl = true;
		}

		updatedTokens.push_back(updatedToken);
	}
	return updatedTokens;
}

dawn::Array<dawn::Token> dawn::Parser::FixIdentifierTypes(const Array<Token>& tokens) const {
	Array<Token> updated_tokens = {};
	updated_tokens.reserve(tokens.size());

	for (const auto& token : tokens) {
		Token updatedToken = token;
		if (updatedToken.type == Identifier && typesRef.contains(updatedToken.value)) {
			updatedToken.type = Type;
		}
		updated_tokens.push_back(updatedToken);
	}
	return updated_tokens;
}
