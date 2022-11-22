#pragma once

#include "preprocessor/preprocessor.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "logic/function.h"

#include <vector>
#include <unordered_map>


namespace faxdawn {
    class compiler
    {
        std::unordered_set<std::string> types_ = syntax::types;
        preprocessor preprocessor_ = {};
        lexer lexer_ = lexer(types_);
        parser parser_ = parser(types_);

        std::unordered_map<std::string, function> compile(const std::vector<token>& tokens) const;

    public:
        compiler() = default;

        void set_macro(const std::string& name, const std::string& value);

        std::string get_macro(const std::string& name) const;

        void remove_macro(const std::string& name);

        std::unordered_map<std::string, function> compile(const std::string& source) const;
    };
}
