#include "lexer/Lexer.h"
#include "dawn/IO.h"


dawn::Lexer::Lexer(Set<String>& typesRef)
	: typesRef(typesRef)
{}

dawn::Array<dawn::Token> dawn::Lexer::Tokenize(const String& source) const
{
	auto tokens = Split(source);
	for (auto& token : tokens) {
		token.type = GetTokenType(typesRef, token.value);
	}
	tokens = FixClassTypes(tokens);
	tokens = FixIdentifierTypes(tokens);
	return tokens;
}

dawn::Array<dawn::Token> dawn::Lexer::Split(const String& source) const
{
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

		if (readingComment || IsIgnored(sourceChar)) {
			continue;
		}

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

	if (valueStream.rdbuf()->in_avail()) {
		SaveStreamPart(valueStream, tokens, lineID);
	}

	return tokens;
}

void dawn::Lexer::SaveStreamPart(StringStream& stream, Array<Token>& tokens, const Int lineID) const
{
	const String result = stream.str();
	if (!dawn::IsDiscarded(result)) {
		Token token = {};
		token.value = result;
		token.line = lineID;
		tokens.push_back(token);
	}
	stream = {};
}

dawn::Array<dawn::Token> dawn::Lexer::FixClassTypes(const Array<Token>& tokens) const
{
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
		else if (updatedToken.value == keyword::type_decl) {
			lastTokenWasTypeDecl = true;
		}

		updatedTokens.push_back(updatedToken);
	}
	return updatedTokens;
}

dawn::Array<dawn::Token> dawn::Lexer::FixIdentifierTypes(const Array<Token>& tokens) const
{
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
