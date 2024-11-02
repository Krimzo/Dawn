#include "lexer.h"
#include "util.h"

using namespace dawn;

int main()
{
	String source = read_file(L"examples/min_example.dw");
	Lexer lexer;

	Array<Token> tokens;
	if (auto error = lexer.tokenize(source, tokens)) {
		std::wcout << error.value() << '\n';
		return 1;
	}

	for (const auto& token : tokens) {
		std::wcout << token << '\n';
	}
	return 0;
}
