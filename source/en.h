#pragma once

#include "t.h"


namespace dawn
{
enum struct TokenType
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

enum struct ExtractType
{
    DEFAULT,
    NEW_LINE,
    SPLITTER,
    SCOPE_START,
};

enum struct UnaryType
{
    PLUS = 0,
    MINUS,
    NOT,
};

enum struct OperatorType
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

enum struct AssignType
{
    ASSIGN = 0,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    MOD,
};

enum struct NodeType
{
    EMPTY = 0,

    // instr
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

    // expr
    REF,
    IDENTIFIER,
    CALL,
    INDEX,
    ENUM,
    STRUCT,
    ARRAY,
    UNARY,
    OPERATOR,
    ASSIGN,
};

enum struct VariableKind
{
    CONST = 0,
    VAR,
    REF,
};

enum struct ArrayType
{
    LIST = 0,
    SIZE,
};

enum struct FunctionType
{
    GLOBAL = 0,
    METHOD,
    LAMBDA,
};

enum struct ValueType
{
    NOTHING = 0,
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

enum struct FrameType
{
    LOCAL = 0,
    GLOBAL = 1,
};

std::ostream& operator<<( std::ostream& stream, TokenType type );
std::ostream& operator<<( std::ostream& stream, VariableKind kind );
std::ostream& operator<<( std::ostream& stream, ValueType type );
}
