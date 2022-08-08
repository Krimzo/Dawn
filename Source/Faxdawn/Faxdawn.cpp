#include "Machine/Machine.h"


int main(int argc, const char** argv) {
	Faxdawn::Machine machine;

	machine.compile_file("Examples/Test.fxdn");


	std::cin.get();
}
