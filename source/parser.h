#pragma once

#include "lexer.h"
#include "node.h"

namespace dawn
{
struct Module
{
    StringSet imports;
    Vector<Variable> variables;
    Vector<Operator> operators;
    Vector<Function> functions;
    Vector<Cast> casts;
    Vector<Enum> enums;
    Vector<Struct> structs;

    Bool contains_id(ID id) const;
};

struct TokenIterator
{
    constexpr TokenIterator(Token const* ptr, Token const* end) : m_ptr(ptr), m_start(ptr), m_end(end)
    {
    }

    Bool valid() const;
    Token const& operator*() const;
    Token const* operator->() const;
    Bool operator==(TokenIterator const& other) const;
    void operator++();
    void operator--();

  private:
    Token const* m_ptr = nullptr;
    Token const* const m_start;
    Token const* const m_end;
};

struct Parser
{
    friend struct Engine;

    void parse(Vector<Token> const& tokens, Module& module) const;
    void parse(Token const* token_ptr, Int token_count, Module& module) const;

    Bool is_variable(TokenIterator const& it) const;
    Value create_default_value(ID type_id, TokenIterator const& it) const;

    void parse_import(TokenIterator& it, Module& module) const;
    void parse_global_struct(TokenIterator& it, Module& module) const;
    void parse_global_enum(TokenIterator& it, Module& module) const;
    void parse_global_cast(TokenIterator& it, Module& module) const;
    void parse_global_function(TokenIterator& it, Module& module) const;
    void parse_global_operator(TokenIterator& it, Module& module) const;
    void parse_global_variable(TokenIterator& it, Module& module) const;

    void parse_struct(TokenIterator& it, Struct& struc) const;
    void parse_enum(TokenIterator& it, Enum& en) const;
    void parse_cast(TokenIterator& it, Cast& cast) const;
    void parse_function(TokenIterator& it, Function& function) const;
    void parse_operator(TokenIterator& it, Operator& oper) const;
    void parse_variable(TokenIterator& it, Variable& variable) const;

    void parse_expression(ExtractType type, TokenIterator& it, Node& tree) const;
    void expression_extract(ExtractType type, TokenIterator& it, Vector<Token>& tokens) const;
    void expression_precedence(Vector<Token>& tokens, Int& index, Bool& unary) const;
    void expression_complex_expr(Vector<Token>& left, Token op, Vector<Token>& right, Node& tree) const;
    void expression_complex_scope(Vector<Token>& left, Token op, Vector<Token>& right, Node& tree) const;
    void expression_complex_array(Vector<Token>& left, Token op, Vector<Token>& right, Node& tree) const;
    void expression_complex_link(Vector<Token>& left, Token op, Vector<Token>& right, Node& tree) const;
    void expression_complex_default(Vector<Token>& left, Token op, Vector<Token>& right, Node& tree) const;
    void expression_pure(Vector<Token>& tokens, Node& tree) const;
    void expression_single(Token const& token, Node& tree) const;
    void expression_single_literal(Token const& token, Node& tree) const;
    void expression_single_keyword(Token const& token, Node& tree) const;
    void expression_single_type(Token const& token, Node& tree) const;
    void expression_single_identifier(Token const& token, Node& tree) const;

    void parse_args(TokenIterator& it, Vector<Arg>& args) const;
    void parse_scope(TokenIterator& it, Scope& scope) const;
    void scope_return(TokenIterator& it, Node& tree) const;
    void scope_break(TokenIterator& it, Node& tree) const;
    void scope_continue(TokenIterator& it, Node& tree) const;
    void scope_throw(TokenIterator& it, Node& tree) const;
    void scope_try(TokenIterator& it, Node& tree) const;
    void scope_if(TokenIterator& it, Node& tree) const;
    void scope_switch(TokenIterator& it, Node& tree) const;
    void scope_loop(TokenIterator& it, Node& tree) const;
    void scope_while(TokenIterator& it, Node& tree) const;
    void scope_for(TokenIterator& it, Node& tree) const;
};

Bool is_unary(Token const& token);
Int token_depth(Token const& token, Bool& in_lambda);

void create_operator_node(Token const& token, Node& node);
OperatorType get_op(StringRef value);
} // namespace dawn
