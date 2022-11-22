#pragma once

#include "preprocessor/preprocessor.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "logic/function.h"

#include <vector>


namespace faxdawn {
    class compiler
    {
        std::unordered_set<std::string> types_ = syntax::types;
        preprocessor preprocessor_ = {};
        lexer lexer_ = types_;
        parser parser_ = types_;

        std::vector<function> compile(const std::vector<token>& tokens) const;

    public:
        compiler() = default;

        std::vector<function> compile(const std::string& source) const;
    };
}
