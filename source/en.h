#pragma once

#include "t.h"

namespace dawn
{
enum struct TokenType : uint8_t
{
    INTEGER = 0,
    FLOAT,
    CHAR,
    STRING,
    KEYWORD,
    TYPE,
    NAME,
    OPERATOR,
};

enum struct ExtractType : uint8_t
{
    WHOLE = 0,
    NEW_LINE_EXP_END,
    SPLITTER,
    SCOPE_START,
};

enum struct OperatorType : uint8_t
{
    ADD = 0,
    SUB,
    MUL,
    DIV,
    POW,
    MOD,
    EQ,
    NOT_EQ,
    LESS,
    GREAT,
    LESS_EQ,
    GREAT_EQ,
    NOT,
    AND,
    OR,
    RANGE,
    RANGE_INCL,
    ASSIGN, // assigns must be last in enum and in the same order as add, sub, etc...
    ADD_ASSIGN,
    SUB_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    POW_ASSIGN,
    MOD_ASSIGN,
    _COUNT,
};

enum struct NodeType : uint8_t
{
    NONE = 0,

    // Instruction.
    SCOPE,
    VARIABLE,
    RETURN,
    BREAK,
    CONTINUE,
    THROW,
    TRY,
    IF,
    SWITCH,
    LOOP,
    WHILE,
    FOR,

    // Expression.
    VALUE,
    IDENTIFIER,
    CALL,
    INDEX,
    LAMBDA,
    ENUM,
    STRUCT,
    ARRAY,
    ACCESS,
    OPERATOR,
    CAST,
};

enum struct VarKind : uint8_t
{
    CONSTANT = 0,
    VARIABLE,
    REFERENCE,
};

enum struct FrameType : uint8_t
{
    LOCAL = 0,
    GLOBAL,
};

std::ostream& operator<<(std::ostream& stream, TokenType type);
std::ostream& operator<<(std::ostream& stream, OperatorType type);
std::ostream& operator<<(std::ostream& stream, NodeType type);
} // namespace dawn
