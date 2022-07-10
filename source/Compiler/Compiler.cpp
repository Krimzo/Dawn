#include "Lexer/Lexer.h"
#include "IOHandler/IOHandler.h"

#include <iostream>


int main(int argc, const char* argv) {

	Faxdawn::Lexer lexer;
	Faxdawn::IOHandler handler;

	for (auto& token : lexer.generate(handler.read("examples/test.fxdn"))) {
		std::cout << token << std::endl;
	}


	std::cin.get();
}
