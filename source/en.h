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
    DEFAULT = 0,
    NEW_LINE,
    SPLITTER,
    SCOPE_START,
};

enum struct UnaryType : uint8_t
{
    PLUS = 0,
    MINUS,
    NOT,
};

enum struct OperatorType : uint8_t
{
    ACCESS = 0,
    POW,
    MOD,
    MUL,
    DIV,
    ADD,
    SUB,
    COMPARE,
    LESS,
    GREAT,
    LESS_EQ,
    GREAT_EQ,
    EQ,
    NOT_EQ,
    AND,
    OR,
    RANGE,
};

enum struct AssignType : uint8_t
{
    ASSIGN = 0,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    MOD,
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
    UNARY,
    OPERATOR,
    ASSIGN,
};

enum struct ValueType : uint8_t
{
    VOID = 0,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    FUNCTION,
    ENUM,
    STRUCT,
    ARRAY,
    RANGE,
    _COUNT,
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
    GLOBAL = 1,
};

std::ostream& operator<<( std::ostream& stream, TokenType type );
std::ostream& operator<<( std::ostream& stream, ValueType type );
}
