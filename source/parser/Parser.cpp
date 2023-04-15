#include "parser/Parser.h"


dawn::Parser::Parser(Set<String>& typesRef) : typesRef(typesRef) {}

dawn::Array<dawn::Token> dawn::Parser::Parse(const Array<Token>& tokens) const {
    Array<Token> parsedTokens = tokens;
    return parsedTokens;
}
