#include "faxdawn/syntax.h"


static bool is_integer(const std::string& value)
{
    if (value.empty()) {
        return false;
    }
    char* res = nullptr;
    strtol(value.data(), &res, 10);
    return !*res;
}

static bool is_double(const std::string& value)
{
    if (value.empty()) {
        return false;
    }
    char* res = nullptr;
    strtod(value.data(), &res);
    return !*res;
}

static bool container_contains(const std::vector<std::string>& container, const std::string& value)
{
    for (auto& container_value : container) {
        if (container_value == value) {
            return true;
        }
    }
    return false;
}

faxdawn::token_type faxdawn::syntax::get_token_type(const std::vector<std::string>& all_types, const std::string& value)
{
    if (is_separator(value)) {
        return token_type::Separator;
    }
    if (is_operator(value)) {
        return token_type::Operator;
    }
    if (is_keyword(value)) {
        return token_type::Keyword;
    }
    if (is_literal(value)) {
        return token_type::Literal;
    }
    if (is_type(all_types, value)) {
        return token_type::Type;
    }
    return token_type::Identifier;
}

bool faxdawn::syntax::is_discarded(const std::string& value)
{
    return container_contains(discarded, value);
}

bool faxdawn::syntax::is_ignored(const char value)
{
    return is_ignored(std::string(&value, 1));
}

bool faxdawn::syntax::is_ignored(const std::string& value)
{
    return container_contains(ignored, value);
}

bool faxdawn::syntax::is_separator(const char value)
{
    return is_separator(std::string(&value, 1));
}

bool faxdawn::syntax::is_separator(const std::string& value)
{
    return container_contains(separators, value);
}

bool faxdawn::syntax::is_operator(const char value)
{
    return is_operator(std::string(&value, 1));
}

bool faxdawn::syntax::is_operator(const std::string& value)
{
    return container_contains(operators, value);
}

bool faxdawn::syntax::is_keyword(const std::string& value)
{
    return container_contains(keywords, value);
}

bool faxdawn::syntax::is_type(const std::vector<std::string>& all_types, const std::string& value)
{
    return container_contains(all_types, value);
}

bool faxdawn::syntax::is_literal(const std::string& value)
{
    if (value.length() == 3 && value.front() == '\'' && value.back() == '\'') {
        return true;
    }
    if (value.front() == '"' && value.back() == '"') {
        return true;
    }
    if (value == "true" || value == "false") {
        return true;
    }
    if (is_integer(value) || is_double(value)) {
        return true;
    }
    return false;
}
