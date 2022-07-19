#include "Parser/Parser.h"


Faxdawn::Parser::Parser(std::vector<std::string>& types) : types(types) {

}
Faxdawn::Parser::~Parser() {

}

std::vector<Faxdawn::Token> Faxdawn::Parser::update_class_types(const std::vector<Token>& tokens) const {
	std::vector<Token> updatedTokens;
	updatedTokens.reserve(tokens.size());
	bool lastWasClass = false;
	for (auto& token : tokens) {
		Token updatedToken = token;
		if (lastWasClass) {
			Faxdawn::error(updatedToken.type != Token::Type::Identifier, updatedToken);
			Faxdawn::error(Faxdawn::IsType(types, updatedToken.value), updatedToken);
			types.push_back(updatedToken.value);
			updatedToken.type = Token::Type::Type;
			lastWasClass = false;
		}
		else if (updatedToken.value == "class") {
			lastWasClass = true;
		}
		updatedTokens.push_back(updatedToken);
	}
	return updatedTokens;
}
std::vector<Faxdawn::Token> Faxdawn::Parser::update_identifier_types(const std::vector<Token>& tokens) const {
	std::vector<Token> updatedTokens;
	updatedTokens.reserve(tokens.size());
	for (auto& token : tokens) {
		Token updatedToken = token;
		if (updatedToken.type == Token::Type::Identifier && Faxdawn::IsType(types, updatedToken.value)) {
			updatedToken.type = Token::Type::Type;
		}
		updatedTokens.push_back(updatedToken);
	}
	return updatedTokens;
}
std::vector<Faxdawn::Token> Faxdawn::Parser::update_array_types(const std::vector<Token>& tokens) const {
	std::vector<Token> updatedTokens;
	updatedTokens.reserve(tokens.size());
	int state = 0;
	for (auto& token : tokens) {
		Token updateToken = token;
		
		bool shouldSave = true;

		if (state == 0) {
			if (updateToken.type == Token::Type::Type) {
				state = 1;
			}
		}
		else if (state == 1) {
			if (updateToken.value == "[") {
				state = 2;
			}
			else {
				state = 0;
			}
		}
		else {
			if (updateToken.value == "]") {
				updatedTokens.pop_back();
				updatedTokens.back().value += "[]";
				updatedTokens.back().type = Token::Type::Array;
				shouldSave = false;
			}
			state = 0;
		}

		if (shouldSave) {
			updatedTokens.push_back(updateToken);
		}
	}
	return updatedTokens;
}

std::vector<Faxdawn::Token> Faxdawn::Parser::parse(const std::vector<Faxdawn::Token>& tokens) const {
	std::vector<Token> parsedTokens = tokens;
	parsedTokens = update_class_types(parsedTokens);
	parsedTokens = update_identifier_types(parsedTokens);
	parsedTokens = update_array_types(parsedTokens);
	return parsedTokens;
}
