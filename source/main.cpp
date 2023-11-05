#include "lexer.h"
#include "parser.h"

#include "file_helper.h"
#include "display_helper.h"


static constexpr bool TEST_FROM_FILE = false;
static constexpr bool DEBUG_LEXER = true;
static constexpr bool DEBUG_IGNORE_WHITESPACE = true;

int main()
{
	using namespace dawn;

	// Source
	std::string source = "let a: int = (2 + 3.0) * 4;";
	if constexpr (TEST_FROM_FILE) {
		source = read_file("examples/full_example.dw");
	}

	// Lex
	Lexer lexer = {};
	lexer.load_defualt_dawn();

	std::vector<Token> tokens = {};
	if (std::optional error = lexer.process(source, &tokens)) {
		std::cout << error.value() << std::endl;
		return 1;
	}
	
	// Debug lex
	if constexpr (DEBUG_LEXER) {
		for (size_t i = 0; i < tokens.size(); i++) {
			if (DEBUG_IGNORE_WHITESPACE && tokens[i].type == TokenType::WHITESPACE) {
				continue;
			}
			std::cout << (i + 1) << ". " << tokens[i] << std::endl;
		}
	}

	// Parse
	Parser parser = {};
	parser.assign_tokens(tokens);
	std::shared_ptr ast = parser.parse();
	if (!ast) {
		parser.print_errors();
	}
}
