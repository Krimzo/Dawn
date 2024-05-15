#include "lexer.h"
#include "parser.h"
#include "engine.h"

#include "helper.h"

static constexpr dawn::Bool TEST_FROM_FILE = false;
static constexpr dawn::Bool DEBUG_LEXER = true;
static constexpr dawn::Bool DEBUG_IGNORE_WHITESPACE = true;

int main()
{
	using namespace dawn;

	// Source
	String source = L"let a: int = 4 + 2 * -10 ^ 2;";
	if constexpr (TEST_FROM_FILE) {
		source = read_file(L"examples/min_example.dw");
	}

	// Lex
	Lexer lexer;
	lexer.load_defualt_dawn();

	Array<Token> tokens;
	if (Optional error = lexer.process(source, &tokens)) {
		std::wcout << error.value() << std::endl;
		return 1;
	}
	
	// Debug lex
	if constexpr (DEBUG_LEXER) {
		UInt counter = 0;
		for (const auto& token : tokens) {
			if constexpr (DEBUG_IGNORE_WHITESPACE) {
				if (token.type == TokenType::WHITESPACE) {
					continue;
				}
			}
			std::wcout << ++counter << ". " << token << std::endl;
		}
	}

	// Parse
	Parser parser;
	parser.assign_tokens(tokens);
	Object globals = parser.parse_global();
	if (!globals) {
		parser.print_errors();
	}

	// Engine
	//Engine engine;
}
