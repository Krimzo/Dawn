#include "Parser/Parser.h"


Faxdawn::Parser::Parser() {

}
Faxdawn::Parser::~Parser() {

}

std::vector<Faxdawn::Token> Faxdawn::Parser::validate_identifiers(const std::vector<Token>& tokens) const {
	std::vector<Token> updatedTokens;
	updatedTokens.reserve(tokens.size());

	int currentScopeID = 0;
	std::vector<int> scopeIdentifierCounts;
	std::list<std::string> identifierNames;
	int lastScopeSeparatorLine = -1;

	for (auto& token : tokens) {
		if (token.type == Token::Type::Separator) {
			if (token.value == "{") {
				currentScopeID += 1;
				lastScopeSeparatorLine = token.line;
			}
			else if (token.value == "}") {
				currentScopeID -= 1;
				Faxdawn::error(currentScopeID < 0, "Unexpected scope separator '}' at line ", token.line);
				lastScopeSeparatorLine = token.line;
			}
		}
		else if (token.type == Token::Type::Identifier) {

		}
		updatedTokens.push_back(token);
	}

	Faxdawn::error(currentScopeID != 0, "Unexpected scope separator '}' at line ", lastScopeSeparatorLine);

	return updatedTokens;
}

std::vector<Faxdawn::Token> Faxdawn::Parser::parse(const std::vector<Faxdawn::Token>& tokens) const {
	std::vector<Token> parsedTokens = validate_identifiers(tokens);
	return parsedTokens;
}
