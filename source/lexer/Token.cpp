#include "lexer/Token.h"


dawn::String dawn::GetTokenDescription(TokenType type) {
	switch (type) {
	case Unknown:
		return L"Unknown";
	case Separator:
		return L"Separator";
	case Operator:
		return L"Operator";
	case Keyword:
		return L"Keyword";
	case Literal:
		return L"Literal";
	case Type:
		return L"Type";
	case Identifier:
		return L"Identifier";
	}
	return {};
}

std::wostream& dawn::operator<<(std::wostream& stream, const Token& token) {
	stream << "(" << GetTokenDescription(token.type) << " : " << token.value << ")";
	return stream;
}
