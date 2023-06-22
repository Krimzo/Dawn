#pragma once

#include "preprocessor/Preprocessor.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "logic/Function.h"


namespace dawn {
    class Compiler
    {
        Map<String, Function> Compile(const Array<Token>& tokens) const;

    public:
        Set<String> types = dawn::types::all;
        Preprocessor preprocessor = {};
        Lexer lexer = Lexer(types);
        Parser parser = Parser(types);

        Map<String, Function> Compile(const String& source) const;
    };
}
