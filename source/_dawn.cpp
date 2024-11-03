#include "engine.h"
#include "util.h"

using namespace dawn;

int main()
{
	String source = read_file(L"examples/min_example.dw");
	
	Lexer lexer;
	Parser parser;
	Engine engine;

	Array<Token> tokens;
	if (auto error = lexer.split(source, tokens)) {
		std::wcout << error.value() << '\n';
		return 1;
	}
#if 0
	for (const auto& token : tokens) {
		std::wcout << token << '\n';
	}
#endif

	Tree tree;
	if (auto error = parser.parse(tokens, tree)) {
		std::wcout << error.value() << '\n';
		return 2;
	}

	Any result;
	if (auto error = engine.eval(tree, result)) {
		std::wcout << error.value() << '\n';
		return 3;
	}

	return 0;
}
