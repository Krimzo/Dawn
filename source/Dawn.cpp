#include "dawn/IO.h"
#include "dawn/FileIO.h"
#include "compiler/Compiler.h"
#include "machine/Machine.h"

using namespace dawn;


int main() {
	// Source
	String source = ReadFileString(L"examples/test.fxdn");

	// Compiling
	Compiler compiler = {};
	Map<String, Function> functions = {};
	functions = compiler.Compile(source);

	// Executing
	Machine machine = {};
	for (auto& function : functions) {
		machine.Load(function.first, function.second);
	}
	machine.ExecuteMain();

	std::cin.get();
}
