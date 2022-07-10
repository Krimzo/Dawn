#include "Lexer/Token.h"


Faxdawn::Token::Token() {

}
Faxdawn::Token::~Token() {

}

std::ostream& Faxdawn::operator<<(std::ostream& stream, const Token& token) {
	stream << "[";
	switch (token.type) {
	case Token::Type::Unknown:
		stream << " Unknown : ";
		break;
	case Token::Type::Separator:
		stream << " Separator : ";
		break;
	case Token::Type::Operator:
		stream << " Operator : ";
		break;
	case Token::Type::Keyword:
		stream << " Keyword : ";
		break;
	case Token::Type::Type:
		stream << " Type : ";
		break;
	case Token::Type::Literal:
		stream << " Literal : ";
		break;
	case Token::Type::Identifier:
		stream << " Identifier : ";
		break;
	}
	stream << "\"" << token.value << "\" ]";
	return stream;
}
