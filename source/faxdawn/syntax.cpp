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

faxdawn::token_type faxdawn::syntax::get_token_type(const std::unordered_set<std::string>& all_types, const std::string& value)
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
    if (all_types.contains(value)) {
        return token_type::Type;
    }
    return token_type::Identifier;
}

bool faxdawn::syntax::is_discarded(const std::string& value)
{
    return discarded.contains(value);
}

bool faxdawn::syntax::is_ignored(const char value)
{
    return is_ignored(std::string(&value, 1));
}

bool faxdawn::syntax::is_ignored(const std::string& value)
{
    return ignored.contains(value);
}

bool faxdawn::syntax::is_separator(const char value)
{
    return is_separator(std::string(&value, 1));
}

bool faxdawn::syntax::is_separator(const std::string& value)
{
    return separators.contains(value);
}

bool faxdawn::syntax::is_operator(const char value)
{
    return is_operator(std::string(&value, 1));
}

bool faxdawn::syntax::is_operator(const std::string& value)
{
    return operators.contains(value);
}

bool faxdawn::syntax::is_keyword(const std::string& value)
{
    return keywords.contains(value);
}

bool faxdawn::syntax::is_literal(const std::string& value)
{
    if (value.front() == literal::character.front() && value.back() == literal::character.front()) {
        return true;
    }
    if (value.front() == literal::string.front() && value.back() == literal::string.front()) {
        return true;
    }
    if (value == literal::empty || value == literal::false_ || value == literal::true_) {
        return true;
    }
    if (is_integer(value) || is_double(value)) {
        return true;
    }
    return false;
}
