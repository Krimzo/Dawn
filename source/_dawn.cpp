#include "util.h"
#include "lexer.h"
#include "parser.h"

using namespace dawn;

int main()
{
	String source = read_file(L"examples/min_example.dw");
	
	Lexer lexer;
	Parser parser;

	Array<Token> tokens;
	if (auto error = lexer.tokenize(source, tokens)) {
		std::wcout << error.value() << '\n';
		return 1;
	}
#if 0
	for (const auto& token : tokens) {
		std::wcout << token << '\n';
	}
	return 0;
#endif

	Module module;
	if (auto error = parser.parse(tokens, module)) {
		std::wcout << error.value() << '\n';
		return 2;
	}

	return 0;
}
