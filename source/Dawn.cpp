#include "dawn/IO.h"
#include "dawn/FileIO.h"
#include "compiler/Compiler.h"
#include "machine/Machine.h"

using namespace dawn;


int main()
{
	Set<String> types = dawn::types::all;
	Lexer lexer = { types };
	Parser parser = { types };

	String source = ReadFileString(L"examples/mini.fxdn");
	Array<Token> tokens = lexer.Tokenize(source);

	Array<Array<Token>> uses = parser.ExtractUses(tokens);
	Array<Array<Token>> customTypes = parser.ExtractTypes(tokens);
	Array<Array<Token>> globalVars = parser.ExtractGlobalVars(tokens);
	Array<Array<Token>> functions = parser.ExtractFunctions(tokens);

	Print(functions);
}
