#include "lexer.h"


static std::string read_file(const std::string& path)
{
	std::stringstream stream = {};
	stream << std::ifstream(path).rdbuf();
	return stream.str();
}

int main()
{
	using namespace dawn;
	
	// Read source
	const std::string source = read_file("examples/min_example.dw");

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
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << (i + 1) << ". " << tokens[i] << std::endl;
	}
}
