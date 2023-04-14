#include "dawn/Syntax.h"


dawn::TokenType dawn::GetTokenType(const Set<String>& allTypes, const String& value)
{
    if (IsSeparator(value)) {
        return Separator;
    }
    if (IsOperator(value)) {
        return Operator;
    }
    if (IsKeyword(value)) {
        return Keyword;
    }
    if (IsLiteral(value)) {
        return Literal;
    }
    if (allTypes.contains(value)) {
        return Type;
    }
    return Identifier;
}

bool dawn::IsDiscarded(const String& value)
{
    return synatx::discarded.contains(value);
}

bool dawn::IsIgnored(const String& value)
{
    return synatx::ignored.contains(value);
}

bool dawn::IsSeparator(const String& value)
{
    return separator::all.contains(value);
}

bool dawn::IsOperator(const String& value)
{
    return operator_::all.contains(value);
}

bool dawn::IsKeyword(const String& value)
{
    return keyword::all.contains(value);
}

bool dawn::IsLiteral(const String& value)
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
    if (IsIgnored(value) || IsFloat(value)) {
        return true;
    }
    return false;
}

bool dawn::IsIgnored(const Char value)
{
    return IsIgnored(String(&value, 1));
}

bool dawn::IsSeparator(const Char value)
{
    return IsSeparator(String(&value, 1));
}

bool dawn::IsOperator(const Char value)
{
    return IsOperator(String(&value, 1));
}

bool dawn::IsFloat(const String& value) {
    if (value.empty()) {
        return false;
    }
    Char* res = nullptr;
    wcstod(value.data(), &res);
    return !*res;
}

bool dawn::IsInt(const String& value) {
    if (value.empty()) {
        return false;
    }
    Char* res = nullptr;
    wcstol(value.data(), &res, 10);
    return !*res;
}
