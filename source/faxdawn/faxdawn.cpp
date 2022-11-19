#include "utility.h"
#include "machine/machine.h"


static faxdawn::machine machine;

int main(const int argc, const char** argv)
{
#ifdef NDEBUG
	if (argc > 1) {
		machine.compile_file(argv[1]);
	}
	else {
		faxdawn::utility::print("Please provide a source file");
	}
#else
	machine.compile_file("examples/test.fxdn");
#endif
}
