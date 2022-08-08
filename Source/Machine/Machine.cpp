#include "Machine/Machine.h"


Faxdawn::Machine::Machine() : types({ "bool", "byte", "char", "int", "float", "String", "Lambda" }), lexer(types), parser(types) {

}

Faxdawn::Machine::~Machine() {

}

bool Faxdawn::Machine::compile(const std::string& source) const {
	auto tokens = lexer.generate(source);
	tokens = parser.parse(tokens);

	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}

	Print<false>("Types: [ ");
	for (int i = 0; i < types.size() - 1; i++) {
		Faxdawn::Print<false>(types[i], ", ");
	}
	Print(types[types.size() - 1], " ]");

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
