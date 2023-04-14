#include "lexer/Lexer.h"


dawn::Lexer::Lexer(Set<String>& typesRef) : typesRef(typesRef) {}

void dawn::Lexer::SaveStreamPart(StringStream& stream, Array<Token>& tokens, const Int lineID) const {
	const String result = stream.str();
	if (!dawn::IsDiscarded(result)) {
		Token token = {};
		token.value = result;
		token.line = lineID;
		tokens.push_back(token);
	}
	stream = {};
}

dawn::Array<dawn::Token> dawn::Lexer::Split(const String& source) const {
	Array<Token> tokens = {};
	StringStream valueStream = {};

	Bool readingComment = false;
	Bool readingSplitter = false;
	Bool readingString = false;
	Int lineID = 1;

	for (const Char& sourceChar : source) {
		if (sourceChar == L'\n') {
			lineID += 1;
			continue;
		}

		if (sourceChar == literal::comment.front()) {
			readingComment = !readingComment;
			continue;
		}

		if (!readingComment && !IsIgnored(sourceChar)) {
			if (sourceChar == literal::string.front()) {
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

dawn::Array<dawn::Token> dawn::Lexer::Generate(const String& source) const {
	auto tokens = Split(source);
	for (auto& token : tokens) {
		token.type = GetTokenType(typesRef, token.value);
	}
	return tokens;
}
