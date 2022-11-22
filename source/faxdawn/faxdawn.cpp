#include "faxdawn/utility.h"
#include "file_io/file_io.h"
#include "machine/machine.h"
#include "compiler/compiler.h"


int main(const int argc, const char** argv)
{
	std::unordered_map<std::string, faxdawn::function> functions = {};

	// Compiling
	faxdawn::compiler compiler = {};
#ifdef NDEBUG
	if (argc > 1) {
		functions = compiler.compile(faxdawn::file_io::read_string(argv[1]));
	}
    else {
		faxdawn::utility::print("Please provide a source file");
	}
#else
	functions = compiler.compile(faxdawn::file_io::read_string("examples/test.fxdn"));
#endif

	// Executing
	faxdawn::machine machine = {};
	for (auto& function : functions) {
		machine.load(function.first, function.second);
	}
	machine.execute_main();
}
