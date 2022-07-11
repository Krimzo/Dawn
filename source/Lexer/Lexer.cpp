#include "Lexer/Lexer.h"

#include <sstream>


Faxdawn::Lexer::Lexer() {

}
Faxdawn::Lexer::~Lexer() {

}

// Source splitter
static void SaveStreamPart(std::stringstream& stream, std::vector<std::string>& parts) {
	const std::string result = stream.str();
	if (!Faxdawn::IsDiscarded(result)) {
		parts.push_back(result);
	}
	stream = {};
}
std::vector<std::string> Faxdawn::Lexer::split(const std::string& source) const {
	std::vector<std::string> parts;

	bool readingComment = false;
	bool readingSplitter = false;
	bool readingString = false;
	std::stringstream partStream = {};

	for (auto& sourceChar : source) {
		if (sourceChar == Faxdawn::comment) {
			readingComment = !readingComment;
		}
		else if (!readingComment && !IsIgnored(sourceChar)) {
			if (sourceChar == Faxdawn::string) {
				readingString = !readingString;
			}
			if (!readingString && (IsSeparator(sourceChar) || IsOperator(sourceChar))) {
				if (readingSplitter) {
					if (IsOperator(partStream.str() + sourceChar)) {
						partStream << sourceChar;
						SaveStreamPart(partStream, parts);
						readingSplitter = false;
					}
					else {
						SaveStreamPart(partStream, parts);
						partStream << sourceChar;
					}
				}
				else {
					SaveStreamPart(partStream, parts);
					partStream << sourceChar;
					readingSplitter = true;
				}
			}
			else {
				if (readingSplitter) {
					SaveStreamPart(partStream, parts);
					readingSplitter = false;
				}
				partStream << sourceChar;
			}
		}
	}
	return parts;
}

// Find types and generate tokens
static Faxdawn::Token::Type GetTokenType(const std::string& value) {
	if (Faxdawn::IsSeparator(value)) {
		return Faxdawn::Token::Type::Separator;
	}
	if (Faxdawn::IsOperator(value)) {
		return Faxdawn::Token::Type::Operator;
	}
	if (Faxdawn::IsKeyword(value)) {
		return Faxdawn::Token::Type::Keyword;
	}
	if (Faxdawn::IsType(value)) {
		return Faxdawn::Token::Type::Type;
	}
	if (Faxdawn::IsLiteral(value)) {
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

// Utility
static std::string ToString(char value) {
	std::string result;
	result.push_back(value);
	return result;
}
static bool ContainerContains(const std::vector<std::string>& container, const std::string& value) {
	for (auto& containerValue : container) {
		if (containerValue == value) {
			return true;
		}
	}
	return false;
}

// Don' care
bool Faxdawn::IsIgnored(char value) {
	return IsIgnored(ToString(value));
}
bool Faxdawn::IsIgnored(const std::string& value) {
	return ContainerContains(Faxdawn::ignored, value);
}
bool Faxdawn::IsDiscarded(const std::string& value) {
	return ContainerContains(Faxdawn::discarded, value);
}

// Separators/operators
bool Faxdawn::IsSeparator(char value) {
	return IsSeparator(ToString(value));
}
bool Faxdawn::IsOperator(char value) {
	return IsSeparator(ToString(value));
}
bool Faxdawn::IsSeparator(const std::string& value) {
	return ContainerContains(Faxdawn::separators, value);
}
bool Faxdawn::IsOperator(const std::string& value) {
	return ContainerContains(Faxdawn::operators, value);
}

// Keywords/types
bool Faxdawn::IsKeyword(const std::string& value) {
	return ContainerContains(Faxdawn::keywords, value);
}
bool Faxdawn::IsType(const std::string& value) {
	return ContainerContains(Faxdawn::types, value);
}

// Literals
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
bool Faxdawn::IsLiteral(const std::string& value) {
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
