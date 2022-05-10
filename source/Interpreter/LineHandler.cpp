#include "LineHandler.h"

#include "Interpreter/Interpreter.h"


void Faxdawn::ProcessLines(std::stringstream& source) {
	for (String line; std::getline(source, line);) {
		Faxdawn::InterpretLine(line);
	}
}
