#include "optimizer.h"

void dawn::Optimizer::optimize(Module& module)
{
    engine.load_module(module);
    optimize_imports(module.imports);
    optimize_variables(module.variables);
    optimize_operators(module.operators);
    optimize_functions(module.functions);
    optimize_enums(module.enums);
    optimize_structs(module.structs);
}

void dawn::Optimizer::optimize_imports(StringSet& imports)
{
    for (auto& import : imports)
        optimize_import(const_cast<String&>(import));
}

void dawn::Optimizer::optimize_variables(Vector<Variable>& vars)
{
    for (auto& var : vars)
        optimize_variable(var);
}

void dawn::Optimizer::optimize_operators(Vector<Operator>& ops)
{
    for (auto& op : ops)
        optimize_operator(op);
}

void dawn::Optimizer::optimize_functions(Vector<Function>& funcs)
{
    for (auto& func : funcs)
        optimize_function(func);
}

void dawn::Optimizer::optimize_enums(Vector<Enum>& enums)
{
    for (auto& enu : enums)
        optimize_enum(enu);
}

void dawn::Optimizer::optimize_structs(Vector<Struct>& structs)
{
    for (auto& struc : structs)
        optimize_struct(struc);
}

void dawn::Optimizer::optimize_import(String& import)
{
    // None.
}

void dawn::Optimizer::optimize_variable(Variable& var)
{
    optimize_expression(*var.expr);
}

void dawn::Optimizer::optimize_operator(Operator& op)
{
    const InlineDropper inline_dropper{inlines};
    for (auto& arg : op.args)
        inlines.emplace_back(arg.id);
    optimize_instruction(op.body.instr);
}

void dawn::Optimizer::optimize_function(Function& func)
{
    const InlineDropper inline_dropper{inlines};
    for (auto& arg : func.args)
        inlines.emplace_back(arg.id);
    optimize_instruction(func.body.instr);
}

void dawn::Optimizer::optimize_enum(Enum& enu)
{
    for (auto& entry : enu.entries)
        optimize_expression(*entry.expr);
}

void dawn::Optimizer::optimize_struct(Struct& struc)
{
    for (auto& method : struc.methods)
    {
        const InlineDropper inline_dropper{inlines};
        for (auto& arg : method.args)
            inlines.emplace_back(arg.id);
        optimize_instruction(method.body.instr);
    }
}

void dawn::Optimizer::optimize_instruction(Vector<Node>& scope)
{
    const InlineDropper inline_dropper{inlines};
    for (Int i = 0; i < (Int)scope.size(); i++)
    {
        auto& instr = scope[i];
        optimize_expression(instr);
        if (instr.type() != NodeType::VARIABLE)
            continue;
        auto& var = std::get<VariableNode>(instr).var;
        if (var.type.is_typeless())
            continue;
        inline_variable(var, scope, i);
    }
}

void dawn::Optimizer::optimize_expression(Node& node)
{
    switch (node.type())
    {
    case NodeType::NONE:
        optimize_expression_none(std::get<None>(node), node);
        break;
    case NodeType::SCOPE:
        optimize_expression_scope(std::get<Scope>(node), node);
        break;
    case NodeType::VARIABLE:
        optimize_expression_variable(std::get<VariableNode>(node), node);
        break;
    case NodeType::RETURN:
        optimize_expression_return(std::get<ReturnNode>(node), node);
        break;
    case NodeType::BREAK:
        optimize_expression_break(std::get<BreakNode>(node), node);
        break;
    case NodeType::CONTINUE:
        optimize_expression_continue(std::get<ContinueNode>(node), node);
        break;
    case NodeType::THROW:
        optimize_expression_throw(std::get<ThrowNode>(node), node);
        break;
    case NodeType::TRY:
        optimize_expression_try(std::get<TryNode>(node), node);
        break;
    case NodeType::IF:
        optimize_expression_if(std::get<IfNode>(node), node);
        break;
    case NodeType::SWITCH:
        optimize_expression_switch(std::get<SwitchNode>(node), node);
        break;
    case NodeType::LOOP:
        optimize_expression_loop(std::get<LoopNode>(node), node);
        break;
    case NodeType::WHILE:
        optimize_expression_while(std::get<WhileNode>(node), node);
        break;
    case NodeType::FOR:
        optimize_expression_for(std::get<ForNode>(node), node);
        break;
    case NodeType::VALUE:
        optimize_expression_value(std::get<Value>(node), node);
        break;
    case NodeType::IDENTIFIER:
        optimize_expression_identifier(std::get<IdentifierNode>(node), node);
        break;
    case NodeType::CALL:
        optimize_expression_call(std::get<CallNode>(node), node);
        break;
    case NodeType::INDEX:
        optimize_expression_index(std::get<IndexNode>(node), node);
        break;
    case NodeType::LAMBDA:
        optimize_expression_lambda(std::get<LambdaNode>(node), node);
        break;
    case NodeType::ENUM:
        optimize_expression_enum(std::get<EnumNode>(node), node);
        break;
    case NodeType::STRUCT:
        optimize_expression_struct(std::get<StructNode>(node), node);
        break;
    case NodeType::ARRAY:
        optimize_expression_array(std::get<ArrayNode>(node), node);
        break;
    case NodeType::ACCESS:
        optimize_expression_access(std::get<AccessNode>(node), node);
        break;
    case NodeType::OPERATOR:
        optimize_expression_operator(std::get<OperatorNode>(node), node);
        break;
    case NodeType::ASSIGN:
        optimize_expression_assign(std::get<AssignNode>(node), node);
        break;
    case NodeType::CAST:
        optimize_expression_cast(std::get<CastNode>(node), node);
        break;
    default:
        ENGINE_PANIC(node.location(), "unknown optimizer node type [", node.type(), "]");
        break;
    }
}

void dawn::Optimizer::inline_variable(Variable& var, Vector<Node>& scope, Int& i)
{
    if (var.type.kind != VarKind::CONSTANT && var.type.kind != VarKind::REFERENCE)
    {
        inlines.emplace_back(var.id);
        return;
    }

    auto& expr = *var.expr;
    if (expr.type() != NodeType::VALUE)
    {
        inlines.emplace_back(var.id);
        return;
    }

    auto& value = std::get<Value>(expr);
    if (!value.is_const())
    {
        inlines.emplace_back(var.id);
        return;
    }

    if (value.type_id() != var.type.type_id)
        ENGINE_PANIC(expr.location(), "optimizer can not inline variable of type [", var.type.type_id,
                     "] because expr is of type [", value.type_id(), "]");

    inlines.emplace_back(var.id, value, true);
    scope.erase(scope.begin() + i);
    --i;
}

void dawn::Optimizer::optimize_expression_none(None& node, Node& out_node)
{
    // None.
}

void dawn::Optimizer::optimize_expression_scope(Scope& node, Node& out_node)
{
    optimize_instruction(node.instr);
}

void dawn::Optimizer::optimize_expression_variable(VariableNode& node, Node& out_node)
{
    optimize_expression(*node.var.expr);
}

void dawn::Optimizer::optimize_expression_return(ReturnNode& node, Node& out_node)
{
    optimize_expression(*node.expr);
}

void dawn::Optimizer::optimize_expression_break(BreakNode& node, Node& out_node)
{
    // None.
}

void dawn::Optimizer::optimize_expression_continue(ContinueNode& node, Node& out_node)
{
    // None.
}

void dawn::Optimizer::optimize_expression_throw(ThrowNode& node, Node& out_node)
{
    optimize_expression(*node.expr);
}

void dawn::Optimizer::optimize_expression_try(TryNode& node, Node& out_node)
{
    if (!node.try_scope.instr.empty())
    {
        optimize_instruction(node.try_scope.instr);
        optimize_instruction(node.catch_scope.instr);
    }
    else
        out_node.emplace<Scope>();
}

void dawn::Optimizer::optimize_expression_if(IfNode& node, Node& out_node)
{
    for (Int i = 0; i < (Int)node.parts.size(); i++)
    {
        auto& part = node.parts[i];
        optimize_expression(part.expr);
        if (part.expr.type() != NodeType::VALUE)
        {
            optimize_instruction(part.scope.instr);
            continue;
        }

        if (std::get<Value>(part.expr).as_bool())
        {
            optimize_instruction(part.scope.instr); // Optimize before resizing (invalid reference otherwise).
            node.parts.resize(i + 1);
            break;
        }
        else
        {
            node.parts.erase(node.parts.begin() + i);
            --i;
        }
    }
    if (node.parts.size() == 1 && node.parts.front().expr.type() == NodeType::VALUE)
    {
        auto& part = node.parts.front();
        out_node.emplace<Scope>(Scope{std::move(part.scope)});
    }
}

void dawn::Optimizer::optimize_expression_switch(SwitchNode& node, Node& out_node)
{
    auto& main_expr = *node.main_expr;
    optimize_expression(main_expr);
    const Bool main_is_val = (main_expr.type() == NodeType::VALUE);
    for (Int i = 0; i < (Int)node.cases.size(); i++)
    {
        auto& casee = node.cases[i];
        Int is_value_counter = 0;
        for (auto& expr : casee.exprs)
        {
            optimize_expression(expr);
            if (!main_is_val || expr.type() != NodeType::VALUE)
                continue;
            ++is_value_counter;

            if (engine
                    .handle_operator(expr.location(), std::get<Value>(main_expr), OperatorType::EQ,
                                     std::get<Value>(expr))
                    .as_bool())
            {
                optimize_instruction(casee.scope.instr);
                out_node.emplace<Scope>(Scope{std::move(casee.scope)});
                return;
            }
        }
        if (main_is_val && is_value_counter == (Int)casee.exprs.size())
        {
            node.cases.erase(node.cases.begin() + i);
            --i;
        }
        else
            optimize_instruction(casee.scope.instr);
    }
    if (node.def_scope)
    {
        optimize_instruction(node.def_scope->instr);
        if (node.cases.empty())
            out_node.emplace<Scope>(Scope{std::move(*node.def_scope)});
    }
    else if (node.cases.size() == 1)
        out_node.emplace<Scope>(Scope{std::move(node.cases.front().scope)});
}

void dawn::Optimizer::optimize_expression_loop(LoopNode& node, Node& out_node)
{
    if (!node.scope.instr.empty())
        optimize_instruction(node.scope.instr);
    else
        out_node.emplace<Scope>();
}

void dawn::Optimizer::optimize_expression_while(WhileNode& node, Node& out_node)
{
    auto& expr_node = *node.expr;
    optimize_expression(expr_node);
    optimize_instruction(node.scope.instr);
    if (expr_node.type() == NodeType::VALUE)
    {
        if (std::get<Value>(expr_node).as_bool())
            out_node.emplace<LoopNode>(node.location).scope = Scope{std::move(node.scope)};
        else
            out_node.emplace<Scope>();
    }
}

void dawn::Optimizer::optimize_expression_for(ForNode& node, Node& out_node)
{
    auto& expr_node = *node.expr;
    optimize_expression(expr_node);
    if (expr_node.type() == NodeType::VALUE)
    {
        auto const& expr_value = std::get<Value>(expr_node);
        const ID type_id = expr_value.type_id();
        if (type_id == id_string)
        {
            if (expr_value.as_string().empty())
            {
                out_node.emplace<Scope>();
                return;
            }
        }
        else if (type_id == id_range)
        {
            if (expr_value.as_range().empty())
            {
                out_node.emplace<Scope>();
                return;
            }
        }
        else if (type_id == id_array)
        {
            if (expr_value.as_array().data.empty())
            {
                out_node.emplace<Scope>();
                return;
            }
        }
    }
    optimize_instruction(node.scope.instr);
}

void dawn::Optimizer::optimize_expression_value(Value& value, Node& out_node)
{
    // None.
}

void dawn::Optimizer::optimize_expression_identifier(IdentifierNode& node, Node& out_node)
{
    for (auto it = inlines.rbegin(); it != inlines.rend(); ++it)
    {
        auto const& [id, value, can_inline] = *it;
        if (id != node.id)
            continue;
        if (!can_inline)
            return;
        out_node.emplace<Value>(it->value);
        break;
    }
}

void dawn::Optimizer::optimize_expression_call(CallNode& node, Node& out_node)
{
    Bool is_ctime = true;
    auto& left_expr = *node.left_expr;
    optimize_expression(left_expr);
    if (left_expr.type() != NodeType::IDENTIFIER ||
        !engine.m_ctime_funcs.contains(std::get<IdentifierNode>(left_expr).id))
        is_ctime = false;
    for (auto& arg : node.args)
    {
        optimize_expression(arg);
        if (arg.type() != NodeType::VALUE)
            is_ctime = false;
    }
    if (is_ctime)
        out_node.emplace<Value>(engine.handle_call_node(node));
}

void dawn::Optimizer::optimize_expression_index(IndexNode& node, Node& out_node)
{
    auto& left_node = *node.left_expr;
    optimize_expression(left_node);
    auto& expr_node = *node.expr;
    optimize_expression(expr_node);
    if (left_node.type() == NodeType::VALUE && expr_node.type() == NodeType::VALUE)
        out_node.emplace<Value>(engine.handle_index_node(node));
}

void dawn::Optimizer::optimize_expression_lambda(LambdaNode& node, Node& out_node)
{
    auto& func = *node.func_value.as_function().dfunction();
    const InlineDropper inline_dropper{inlines};
    for (auto& arg : func.args)
        inlines.emplace_back(arg.id);
    optimize_instruction(func.body.instr);
}

void dawn::Optimizer::optimize_expression_enum(EnumNode& node, Node& out_node)
{
    // None.
}

void dawn::Optimizer::optimize_expression_struct(StructNode& node, Node& out_node)
{
    if (std::holds_alternative<StructNode::NamedInit>(node.init))
    {
        Bool is_ctime = true;
        for (auto& [_, arg] : std::get<StructNode::NamedInit>(node.init).args)
        {
            optimize_expression(arg);
            if (arg.type() != NodeType::VALUE)
                is_ctime = false;
        }
        if (is_ctime)
            out_node.emplace<Value>(engine.handle_struct_node(node));
    }
    else
    {
        Bool is_ctime = true;
        for (auto& arg : std::get<StructNode::ListInit>(node.init).args)
        {
            optimize_expression(arg);
            if (arg.type() != NodeType::VALUE)
                is_ctime = false;
        }
        if (is_ctime)
            out_node.emplace<Value>(engine.handle_struct_node(node));
    }
}

void dawn::Optimizer::optimize_expression_array(ArrayNode& node, Node& out_node)
{
    if (std::holds_alternative<ArrayNode::ListInit>(node.init))
    {
        Bool is_ctime = true;
        for (auto& element : std::get<ArrayNode::ListInit>(node.init).elements)
        {
            optimize_expression(element);
            if (element.type() != NodeType::VALUE)
                is_ctime = false;
        }
        if (is_ctime)
            out_node.emplace<Value>(engine.handle_array_node(node));
    }
    else
    {
        auto& init = std::get<ArrayNode::SizedInit>(node.init);
        auto& size_expr = *init.size_expr;
        optimize_expression(size_expr);
        if (size_expr.type() == NodeType::VALUE)
            out_node.emplace<Value>(engine.handle_array_node(node));
    }
}

void dawn::Optimizer::optimize_expression_access(AccessNode& node, Node& out_node)
{
    optimize_expression(*node.left_expr);
}

void dawn::Optimizer::optimize_expression_operator(OperatorNode& node, Node& out_node)
{
    auto& left_node = node.sides[0];
    auto& right_node = node.sides[1];
    optimize_expression(left_node);
    optimize_expression(right_node);

    if (left_node.type() != NodeType::VALUE || right_node.type() != NodeType::VALUE)
        return;

    Value left_value = std::get<Value>(left_node);
    auto& right_value = std::get<Value>(right_node);
    if (!engine.m_ctime_ops[(Int)node.type].contains(combine_ids(left_value.type_id(), right_value.type_id())))
        return;

    left_value = engine.handle_operator(node.location, left_value, node.type, right_value);
    out_node.emplace<Value>(left_value);
}

void dawn::Optimizer::optimize_expression_assign(AssignNode& node, Node& out_node)
{
    for (auto& side : node.sides)
        optimize_expression(side);
}

void dawn::Optimizer::optimize_expression_cast(CastNode& node, Node& out_node)
{
    optimize_expression(*node.left_expr);
    if (node.left_expr->type() != NodeType::VALUE)
        return;

    Value left_value = std::get<Value>(*node.left_expr);
    if (!engine.m_ctime_casts.contains(combine_ids(left_value.type_id(), node.right_type_id)))
        return;

    auto* left_casts = engine.casts.get(left_value.type_id());
    if (!left_casts)
        return;

    auto* cast_func = left_casts->get(node.right_type_id);
    if (!cast_func)
        return;

    out_node.emplace<Value>(engine.handle_cast(node.location, left_value, node.right_type_id));
}
