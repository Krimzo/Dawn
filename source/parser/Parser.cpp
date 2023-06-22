#include "parser/Parser.h"


dawn::Parser::Parser(Set<String>& typesRef)
    : typesRef(typesRef)
{}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractUses(const Array<Token>& tokens) const
{
    Int scopeLevel = 0;
    Bool readingUse = false;
    Array<Token> buffer = {};

    Array<Array<Token>> result = {};
    for (const auto& token : tokens) {
        if (token.value == separator::open_scope) {
            scopeLevel += 1;
        }
        else if (token.value == separator::close_scope) {
            scopeLevel -= 1;
        }

        if (scopeLevel == 0 && token.value == keyword::use_module) {
            readingUse = true;
        }

        if (readingUse) {
            buffer.push_back(token);

            if (token.value == separator::end_command) {
                result.push_back(buffer);
                buffer.clear();
                readingUse = false;
            }
        }
    }
    return result;
}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractTypes(const Array<Token>& tokens) const
{
    Int scopeLevel = 0;
    Bool readingType = false;
    Array<Token> buffer = {};

    Array<Array<Token>> result = {};
    for (const auto& token : tokens) {
        if (token.value == separator::open_scope) {
            scopeLevel += 1;
        }
        else if (token.value == separator::close_scope) {
            scopeLevel -= 1;
        }
        
        if (scopeLevel == 0 && token.value == keyword::type_decl) {
            readingType = true;
        }

        if (readingType) {
            buffer.push_back(token);

            if (scopeLevel == 0 && token.value == separator::close_scope) {
                result.push_back(buffer);
                buffer.clear();
                readingType = false;
            }
        }
    }
    return result;
}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractGlobalVars(const Array<Token>& tokens) const
{
    Int scopeLevel = 0;
    Bool readingGlobal = false;
    Array<Token> buffer = {};

    Array<Array<Token>> result = {};
    for (const auto& token : tokens) {
        if (token.value == separator::open_scope) {
            scopeLevel += 1;
        }
        else if (token.value == separator::close_scope) {
            scopeLevel -= 1;
        }

        if (scopeLevel == 0 && token.value == keyword::global_var) {
            readingGlobal = true;
        }

        if (readingGlobal) {
            buffer.push_back(token);

            if (token.value == separator::end_command) {
                result.push_back(buffer);
                buffer.clear();
                readingGlobal = false;
            }
        }
    }
    return result;
}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractFunctions(const Array<Token>& tokens) const
{
    Int scopeLevel = 0;
    Bool readingFunction = false;
    Array<Token> buffer = {};

    Array<Array<Token>> result = {};
    for (const auto& token : tokens) {
        if (token.value == separator::open_scope) {
            scopeLevel += 1;
        }
        else if (token.value == separator::close_scope) {
            scopeLevel -= 1;
        }
        
        if (scopeLevel == 0 && token.value == keyword::function_decl) {
            readingFunction = true;
        }

        if (readingFunction) {
            buffer.push_back(token);

            if (scopeLevel == 0 && token.value == separator::close_scope) {
                result.push_back(buffer);
                buffer.clear();
                readingFunction = false;
            }
        }
    }
    return result;
}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractFunctionCommands(const Array<Token>& tokens) const
{
    Int scopeStart = 0;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].value == separator::open_scope) {
            scopeStart = i + 1;
            break;
        }
    }
    return ExtractScopeCommands(Array<Token>(tokens.begin() + scopeStart, tokens.end() - 1));
}

dawn::Array<dawn::Array<dawn::Token>> dawn::Parser::ExtractScopeCommands(const Array<Token>& tokens) const
{
    Array<Token> buffer = {};
    
    Array<Array<Token>> result = {};
    for (const auto& token : tokens) {
        if (token.value == separator::end_command) {
            result.push_back(buffer);
            buffer.clear();
        }
        else {
            buffer.push_back(token);
        }
    }
    return result;
}
