#pragma once

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "FileIO/FileIO.h"


namespace Faxdawn {
	class Machine {
		Lexer lexer;
		Parser parser;
		FileIO fileIO;
		std::vector<std::string> types;

	public:
		Machine();
		Machine(const Machine&) = delete;
		void operator=(const Machine&) = delete;
		~Machine();

		bool compile(const std::string& source) const;
		bool compile_file(const std::string& filePath) const;

		bool run(const std::vector<Token>& tokens);
		bool run_file(const std::string& filePath);
	};
}
