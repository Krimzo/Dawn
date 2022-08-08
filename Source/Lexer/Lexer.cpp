#include "Lexer/Lexer.h"


Faxdawn::Lexer::Lexer(std::vector<std::string>& types) : types(types) {

}

Faxdawn::Lexer::~Lexer() {

}

// Source splitter
static void SaveStreamPart(std::stringstream& stream, std::vector<Faxdawn::Token>& tokens, int lineID) {
	const std::string result = stream.str();
	if (!Faxdawn::IsDiscarded(result)) {
		Faxdawn::Token token;
		token.value = result;
		token.line = lineID;
		tokens.push_back(token);
	}
	stream = {};
}

std::vector<Faxdawn::Token> Faxdawn::Lexer::split(const std::string& source) const {
	std::vector<Token> tokens;

	bool readingComment = false;
	bool readingSplitter = false;
	bool readingString = false;
	std::stringstream valueStream = {};
	int lineID = 1;

	for (auto& sourceChar : source) {
		if (sourceChar == '\n') {
			lineID += 1;
		}
		if (sourceChar == Faxdawn::comment) {
			readingComment = !readingComment;
		}
		else if (!readingComment && !IsIgnored(sourceChar)) {
			if (sourceChar == Faxdawn::string) {
				readingString = !readingString;
			}
			if (!readingString && (IsSeparator(sourceChar) || IsOperator(sourceChar))) {
				if (readingSplitter) {
					if (IsOperator(valueStream.str() + sourceChar)) {
						valueStream << sourceChar;
						SaveStreamPart(valueStream, tokens, lineID);
						readingSplitter = false;
					}
					else {
						SaveStreamPart(valueStream, tokens, lineID);
						valueStream << sourceChar;
					}
				}
				else {
					SaveStreamPart(valueStream, tokens, lineID);
					valueStream << sourceChar;
					readingSplitter = true;
				}
			}
			else {
				if (readingSplitter) {
					SaveStreamPart(valueStream, tokens, lineID);
					readingSplitter = false;
				}
				valueStream << sourceChar;
			}
		}
	}
	return tokens;
}

// Find types and generate tokens
static Faxdawn::Token::Type GetTokenType(const std::vector<std::string>& types, const std::string& value) {
	if (Faxdawn::IsSeparator(value)) {
		return Faxdawn::Token::Type::Separator;
	}
	if (Faxdawn::IsOperator(value)) {
		return Faxdawn::Token::Type::Operator;
	}
	if (Faxdawn::IsKeyword(value)) {
		return Faxdawn::Token::Type::Keyword;
	}
	if (Faxdawn::IsType(types, value)) {
		return Faxdawn::Token::Type::Type;
	}
	if (Faxdawn::IsLiteral(value)) {
		return Faxdawn::Token::Type::Literal;
	}
	return Faxdawn::Token::Type::Identifier;
}

std::vector<Faxdawn::Token> Faxdawn::Lexer::generate(const std::string& source) const {
	auto tokens = split(source);
	for (auto& token : tokens) {
		token.type = GetTokenType(types, token.value);
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

// Don't care
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
	return IsOperator(ToString(value));
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

bool Faxdawn::IsType(const std::vector<std::string>& types, const std::string& value) {
	return ContainerContains(types, value);
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
