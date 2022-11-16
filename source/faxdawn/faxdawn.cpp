#include "utility.h"
#include "machine/machine.h"


static faxdawn::machine machine;

static void debug_run(const std::string& filepath)
{
	machine.compile_file(filepath);
}

static void release_run(const std::string& filepath)
{
	
}

int main(const int argc, const char** argv)
{
#ifdef NDEBUG
	if (argc > 1) {
		release_run(argv[1]);
	}
	else {
		faxdawn::utility::print("Please provide a source file");
	}
#else
	debug_run("examples/test.fxdn");
#endif
}
