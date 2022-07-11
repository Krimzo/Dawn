#include "FileIO/FileIO.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"


int main(int argc, const char* argv) {
	Faxdawn::FileIO fileIO;
	Faxdawn::Lexer lexer;
	Faxdawn::Parser parser;

	std::string source = fileIO.read("examples/test.fxdn");
	auto tokens = lexer.generate(source);
	tokens = parser.parse(tokens);

	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}


	std::cin.get();
}
