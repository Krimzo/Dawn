#include "Lexer/Lexer.h"

#include <sstream>


Faxdawn::Lexer::Lexer() {

}
Faxdawn::Lexer::~Lexer() {

}

template<typename T> static std::string ToString(const T& value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}
static bool IsIgnored(char value) {
	for (auto& ignoredValue : Faxdawn::ignored) {
		if (ignoredValue == value) {
			return true;
		}
	}
	return false;
}
static bool IsSeparator(char value) {
	for (auto& separatorValue : Faxdawn::separators) {
		if (separatorValue == value) {
			return true;
		}
	}
	return false;
}
static bool IsOperator(const std::string& value) {
	for (auto& operatorValue : Faxdawn::operators) {
		if (operatorValue == value) {
			return true;
		}
	}
	return false;
}

std::vector<std::string> Faxdawn::Lexer::split(const std::string& source) const {
	std::vector<std::string> parts;

	bool readingComment = false;
	bool readingSplitter = false;
	std::stringstream partStream = {};

	for (auto& sourceChar : source) {
		if (sourceChar == Faxdawn::comment) {
			readingComment = !readingComment;
		}
		else if (!readingComment && !IsIgnored(sourceChar)) {
			if (IsSeparator(sourceChar) || IsOperator(ToString(sourceChar))) {
				if (!readingSplitter) {
					readingSplitter = true;
					std::string part = partStream.str();
					if (part != "") {
						parts.push_back(part);
					}
					partStream = {};
					partStream << sourceChar;
				}
				else {
					std::string possibleOperator = partStream.str() + sourceChar;
					if (IsOperator(possibleOperator)) {
						parts.push_back(possibleOperator);
						partStream = {};
						readingSplitter = false;
					}
					else {
						std::string splitter = partStream.str();
						if (splitter != " ") {
							parts.push_back(splitter);
						}
						partStream = {};
						partStream << sourceChar;
					}
				}
			}
			else {
				if (readingSplitter) {
					readingSplitter = false;
					std::string splitter = partStream.str();
					if (splitter != " ") {
						parts.push_back(splitter);
					}
					partStream = {};
				}
				partStream << sourceChar;
			}
		}
	}
	return parts;
}

static bool IsInteger(const std::string& value) {
	if (value.empty()) {
		return false;
	}
	char* res = nullptr;
	long iHateWarnings = strtol(value.data(), &res, 10);
	return !*res;
}
static bool IsDouble(const std::string& value) {
	if (value.empty()) {
		return false;
	}
	char* res = nullptr;
	double iHateWarnings = strtod(value.data(), &res);
	return !*res;
}
static bool IsKeyword(const std::string& value) {
	for (auto& keywordValue : Faxdawn::keywords) {
		if (keywordValue == value) {
			return true;
		}
	}
	return false;
}
static bool IsType(const std::string& value) {
	for (auto& typeValue : Faxdawn::types) {
		if (typeValue == value) {
			return true;
		}
	}
	return false;
}
static bool IsLiteral(const std::string& value) {
	if (value.find("'") != std::string::npos) {
		return true;
	}
	if (value.find("\"") != std::string::npos) {
		return true;
	}
	if (value == "true" || value == "false") {
		return true;
	}
	if (IsInteger(value) || IsDouble(value)) {
		return true;
	}
	return false;
}

static Faxdawn::Token::Type GetTokenType(const std::string& value) {
	if (value.size() == 1 && IsSeparator(value.front())) {
		return Faxdawn::Token::Type::Separator;
	}
	if (IsOperator(value)) {
		return Faxdawn::Token::Type::Operator;
	}
	if (IsKeyword(value)) {
		return Faxdawn::Token::Type::Keyword;
	}
	if (IsType(value)) {
		return Faxdawn::Token::Type::Type;
	}
	if (IsLiteral(value)) {
		return Faxdawn::Token::Type::Literal;
	}
	return Faxdawn::Token::Type::Identifier;
}

std::vector<Faxdawn::Token> Faxdawn::Lexer::generate(const std::string& source) const {
	std::vector<Faxdawn::Token> tokens;
	for (auto& part : split(source)) {
		Faxdawn::Token token;
		token.value = part;
		token.type = GetTokenType(token.value);
		tokens.push_back(token);
	}
	return tokens;
}
