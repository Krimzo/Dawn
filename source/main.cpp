#include "lexer.h"
#include "parser.h"

#include "file_helper.h"
#include "display_helper.h"


int main()
{
	using namespace dawn;
	
	// Read source
#if 0
	const std::string source = read_file("examples/min_example.dw");
#else
	const std::string source = "let a: int = (2 + 3) * 4;";
#endif

	// Prepare lexer
	Lexer lexer = {};
	lexer.load_defualt_dawn();

	// Process the source
	std::vector<Token> tokens = {};
	if (std::optional error = lexer.process(source, &tokens)) {
		std::cout << error.value() << std::endl;
		return 1;
	}

	// Print tokens
#if 0
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << (i + 1) << ". " << tokens[i] << std::endl;
	}
#elif 0
	display_colored(tokens);
#endif

	// Prepare parser
	Parser parser = {};

	// Process tokens
	std::shared_ptr ast = parser.parse_expression(tokens);
	if (!ast) {
		for (auto& error : parser.errors) {
			std::cout << error << std::endl;
		}
	}
}
