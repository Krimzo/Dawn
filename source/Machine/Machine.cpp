#include "Machine/Machine.h"


Faxdawn::Machine::Machine() : types({ "bool", "byte", "char", "int", "float", "string", "lambda" }), lexer(types), parser(types) {

}
Faxdawn::Machine::~Machine() {

}

bool Faxdawn::Machine::compile(const std::string& source) const {
	auto tokens = lexer.generate(source);
	tokens = parser.parse(tokens);

	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}

	print<NULL>("Types: [ ");
	for (int i = 0; i < types.size() - 1; i++) {
		Faxdawn::print<NULL>(types[i], ", ");
	}
	print(types[types.size() - 1], " ]");

	return true;
}
bool Faxdawn::Machine::compile_file(const std::string& filePath) const {
	return compile(fileIO.read(filePath));
}

bool Faxdawn::Machine::run(const std::vector<Token>& tokens) {
	return true;
}
bool Faxdawn::Machine::run_file(const std::string& filePath) {
	return true;
}
