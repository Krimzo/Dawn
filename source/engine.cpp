#include "engine.h"

dawn::Engine::Engine()
{
    load_standard_operators();
    load_standard_functions();
    load_standard_members();
    load_standard_casts();
}

void dawn::Engine::load_module(Module const& module)
{
    for (auto& entry : module.operators)
        load_operator(entry);

    for (auto& entry : module.functions)
        if (!entry.is_extension())
            load_function(entry);

    for (auto& entry : module.enums)
        load_enum(entry);

    for (auto& entry : module.structs)
        load_struct(entry);

    for (auto& entry : module.functions)
        if (entry.is_extension())
            load_function(entry);

    for (auto& entry : module.variables)
        load_variable(entry);
}

void dawn::Engine::load_operator(Operator const& entry)
{
    if (entry.args.size() != 2)
        ENGINE_PANIC({}, "operators must have 2 arguments");

    auto const& left = entry.args[0];
    auto const& right = entry.args[1];

    auto& left_types = operators[(Int)entry.type];
    auto& right_types = left_types.get_or_set(left.type.type_id);

    if (right_types.get(right.type.type_id))
        ENGINE_PANIC({}, "operator [", entry.type, "] with left type [", left.type.type_id, "] and right type [",
                     right.type.type_id, "] already defined");
    right_types.set(right.type.type_id, {}).as_global().func.emplace<DFunction>(entry.args, entry.body);
}

void dawn::Engine::load_function(Function const& entry)
{
    if (entry.is_extension())
    {
        auto& funcs = members.get_or_set(entry.type_id);
        if (funcs.get(entry.id))
            ENGINE_PANIC({}, "method [", entry.id, "] already defined for type [", entry.type_id, "]");
        funcs.set(entry.id, [entry](Location location, Engine& engine, Value const& self) -> Value {
            FunctionValue fv{};
            auto& method = fv.data.emplace<FunctionValue::AsMethod>();
            method.id = entry.id;
            method.func = DFunction{entry.args, entry.body};
            *method.self = self;
            return Value{fv, location};
        });
    }
    else
    {
        if (stack.root().get(entry.id))
            ENGINE_PANIC({}, "object [", entry.id, "] already exists");
        FunctionValue fv{};
        auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
        global.id = entry.id;
        global.func = DFunction{entry.args, entry.body};
        stack.root().set(entry.id, Value{fv});
    }
}

void dawn::Engine::load_enum(Enum const& entry)
{
    for (auto& entry : enums.set(entry.id, entry).entries)
    {
        if (std::holds_alternative<Value>(*entry.expr))
            continue;
        const NodeRef node_ref =
            entry.expr; // Forces read first/write later order since entry.expr is used in both cases.
        entry.expr->emplace<Value>(handle_expression(*node_ref));
    }
}

void dawn::Engine::load_struct(Struct const& entry)
{
    structs.set(entry.id, entry);
    auto& funcs = members.get_or_set(entry.id);
    for (auto& method : entry.methods)
    {
        FunctionValue fv{};
        auto& f = fv.data.emplace<FunctionValue::AsMethod>();
        f.id = method.id;
        auto& dfunc = f.func.emplace<DFunction>();
        dfunc.args = method.args;
        dfunc.body = method.body;
        funcs.set(method.id, [fv](Location location, Engine& engine, Value const& self) -> Value {
            *fv.as_method().self = self;
            return Value{fv, location};
        });
    }
}

void dawn::Engine::load_variable(Variable const& entry)
{
    auto& expr = *entry.expr;
    add_variable(expr.location(), entry.type, entry.id, handle_expression(expr));
}

void dawn::Engine::bind_operator(ID left_type_id, OperatorType op_type, ID right_type_id, Bool is_const,
                                 CFunction cfunc)
{
    auto& left_types = operators[(Int)op_type];

    auto* right_types = left_types.get(left_type_id);
    if (!right_types)
        right_types = &left_types.set(left_type_id, {});

    if (right_types->get(right_type_id))
        ENGINE_PANIC({}, "operator [", op_type, "] with left type [", left_type_id, "] and right type [", right_type_id,
                     "] already defined");
    right_types->set(right_type_id, {}).as_global().func.emplace<CFunction>(std::move(cfunc));

    if (is_const)
        m_ctime_ops[(Int)op_type].insert(combine_ids(left_type_id, right_type_id));
}

void dawn::Engine::bind_cast(ID left_type_id, ID right_type_id, Bool is_const, CastCFunc const& cfunc)
{
    auto* cast_ptr = casts.get_or_set(left_type_id).get(right_type_id);
    if (cast_ptr)
        ENGINE_PANIC(Location{}, "cast [", left_type_id, "] -> [", right_type_id, "] already defined");
    casts.get_or_set(left_type_id).set(right_type_id, cfunc);
}

void dawn::Engine::bind_function(ID id, Bool is_ctime, CFunction cfunc)
{
    if (stack.root().get(id))
        ENGINE_PANIC({}, "object [", id, "] already exists");
    if (is_ctime)
        m_ctime_funcs.insert(id);

    FunctionValue fv{};
    auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
    global.id = id;
    global.func = std::move(cfunc);
    stack.root().set(id, Value{fv});
}

dawn::Value dawn::Engine::call_function(ID id, Value* args, Int arg_count)
{
    Value* value = stack.root().get(id);
    if (!value)
        ENGINE_PANIC({}, "object [", id, "] does not exist");
    if (value->type_id() != id_function)
        ENGINE_PANIC({}, "object [", id, "] can not be called");
    return handle_function({}, value->as_function(), args, arg_count);
}

void dawn::Engine::add_variable(Location location, VarType const& type, ID id, Value const& value)
{
    if (type.type_id.valid() && type.type_id != value.type_id())
        ENGINE_PANIC(location, "can not init variable of type [", type.type_id, "] with type [", value.type_id(), "]");

    switch (type.kind)
    {
    case VarKind::CONSTANT:
        stack.current().set(id, value.clone());
        break;

    case VarKind::VARIABLE:
        stack.current().set(id, value.clone().unlock_const());
        break;

    default:
        stack.current().set(id, value);
        break;
    }
}

dawn::Value* dawn::Engine::get_variable(ID id)
{
    return stack.current().get(id);
}

void dawn::Engine::bind_field(ID type_id, ID id, FieldCFunc const& func)
{
    members.get_or_set(type_id).set(id, [func](Location location, Engine& engine, Value const& self) -> Value {
        return func(location, engine, const_cast<Value&>(self));
    });
}

void dawn::Engine::bind_method(ID type_id, ID id, Bool is_const, Int expected_args, MethodCFunc const& func)
{
    members.get_or_set(type_id).set(
        id, [id, is_const, expected_args, func](Location location, Engine& _, Value const& self) -> Value {
            FunctionValue fv{};
            auto& method = fv.data.emplace<FunctionValue::AsMethod>();
            method.id = id;
            method.func = [id, is_const, expected_args, func](Location location, Engine& engine, Value const* args,
                                                              Int arg_count) -> Value {
                if (!is_const && args[0].is_const())
                    ENGINE_PANIC(location, "can not call [", id, "] on a const value");
                if ((1 + expected_args) != arg_count)
                    ENGINE_PANIC(location, "method [", id, "] expects self + ", expected_args, " arguments");
                return func(location, engine, args[0], args + 1);
            };
            *method.self = self;
            return Value{fv, location};
        });
}

dawn::Value dawn::Engine::create_default_value(ID type_id, Location location)
{
    if (type_id == id_void)
        return Value{};

    else if (type_id == id_bool)
        return Value{Bool{}, location};

    else if (type_id == id_int)
        return Value{Int{}, location};

    else if (type_id == id_float)
        return Value{Float{}, location};

    else if (type_id == id_char)
        return Value{Char{}, location};

    else if (type_id == id_string)
        return Value{String{}, location};

    else if (type_id == id_range)
        return Value{RangeValue{}, location};

    else if (type_id == id_function)
        return Value{FunctionValue{}, location};

    else if (type_id == id_array)
        return Value{ArrayValue{}, location};

    else if (auto* enum_ptr = enums.get(type_id))
    {
        auto& entry = *enum_ptr->entries.begin();
        EnumNode node{location};
        node.type_id = type_id;
        node.key_id = entry.id;
        return handle_enum_node(node);
    }

    else if (auto* struct_ptr = structs.get(type_id))
    {
        StructNode node{location};
        node.type_id = type_id;
        return handle_struct_node(node);
    }

    else
        ENGINE_PANIC(location, "type [", type_id, "] does not exist");
}

void dawn::Engine::to_void(Value const& value)
{
    handle_cast(value.location(), value, id_void);
}

dawn::Bool dawn::Engine::to_bool(Value const& value)
{
    return handle_cast(value.location(), value, id_bool).as_bool();
}

dawn::Int dawn::Engine::to_int(Value const& value)
{
    return handle_cast(value.location(), value, id_int).as_int();
}

dawn::Float dawn::Engine::to_float(Value const& value)
{
    return handle_cast(value.location(), value, id_float).as_float();
}

dawn::Char dawn::Engine::to_char(Value const& value)
{
    return handle_cast(value.location(), value, id_char).as_char();
}

dawn::String dawn::Engine::to_string(Value const& value)
{
    return handle_cast(value.location(), value, id_string).as_string();
}

dawn::RangeValue dawn::Engine::to_range(Value const& value)
{
    return handle_cast(value.location(), value, id_range).as_range();
}

dawn::FunctionValue dawn::Engine::to_function(Value const& value)
{
    return handle_cast(value.location(), value, id_function).as_function();
}

dawn::ArrayValue dawn::Engine::to_array(Value const& value)
{
    return handle_cast(value.location(), value, id_array).as_array();
}

void dawn::Engine::handle_variable_node(VariableNode const& node)
{
    add_variable(node.location, node.var.type, node.var.id, handle_expression(*node.var.expr));
}

dawn::Value const& dawn::Engine::handle_identifier_node(IdentifierNode const& node)
{
    if (auto* ptr = get_variable(node.id))
        return *ptr;
    ENGINE_PANIC(node.location, "object [", node.id, "] does not exist");
}

dawn::Value dawn::Engine::handle_call_node(CallNode const& node)
{
    const Value left = handle_expression(*node.left_expr);
    if (left.type_id() != id_function)
        ENGINE_PANIC(node.location, "can not call [", left.type_id(), "]");

    auto const& func = left.as_function();
    const Int arg_count = func.is_method() ? (1 + node.args.size()) : node.args.size();

    Value* const args_ptr = SALLOC(Value, arg_count);
    const SAllocManager<Value> alloc_manager{args_ptr, arg_count};

    if (func.is_method())
    {
        args_ptr[0] = *func.as_method().self;
        for (Int i = 0; i < (Int)node.args.size(); i++)
            args_ptr[1 + i] = handle_expression(node.args[i]);
    }
    else
    {
        for (Int i = 0; i < (Int)node.args.size(); i++)
            args_ptr[i] = handle_expression(node.args[i]);
    }

    return handle_function(node.location, func, args_ptr, arg_count);
}

dawn::Value dawn::Engine::handle_index_node(IndexNode const& node)
{
    const Value left = handle_expression(*node.left_expr);
    const Int index = handle_expression(*node.expr).as_int();

    if (left.type_id() == id_string)
    {
        auto& value = left.as_string();
        if (index < 0 || index >= (Int)value.size())
            ENGINE_PANIC(node.location, "string access [", index, "] out of bounds");
        return Value{&value[index], left.is_const(), node.location};
    }
    else if (left.type_id() == id_array)
    {
        auto& value = left.as_array();
        if (index < 0 || index >= (Int)value.data.size())
            ENGINE_PANIC(node.location, "array access [", index, "] out of bounds");
        return value.data[index];
    }
    else
        ENGINE_PANIC(node.location, "can not index type [", left.type_id(), "]");
}

void dawn::Engine::handle_return_node(ReturnNode const& node, Opt<Value>& retval)
{
    retval = handle_expression(*node.expr);
}

void dawn::Engine::handle_break_node(BreakNode const& node, Bool* didbrk)
{
    if (!didbrk)
        ENGINE_PANIC(node.location, "break statement outside of loop");
    *didbrk = true;
}

void dawn::Engine::handle_continue_node(ContinueNode const& node, Bool* didcon)
{
    if (!didcon)
        ENGINE_PANIC(node.location, "continue statement outside of loop");
    *didcon = true;
}

void dawn::Engine::handle_throw_node(ThrowNode const& node)
{
    throw handle_expression(*node.expr);
}

void dawn::Engine::handle_try_node(TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon)
{
    try
    {
        const PopHandler pop_handler = stack.push();
        handle_scope(node.try_scope, retval, didbrk, didcon);
    }
    catch (Value const& value)
    {
        const PopHandler pop_handler = stack.push();
        stack.current().set(node.catch_id, value);
        handle_scope(node.catch_scope, retval, didbrk, didcon);
    }
}

void dawn::Engine::handle_if_node(IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon)
{
    for (auto& part : node.parts)
    {
        if (!handle_expression(part.expr).as_bool())
            continue;

        const PopHandler pop_handler = stack.push();
        handle_scope(part.scope, retval, didbrk, didcon);
        break;
    }
}

void dawn::Engine::handle_switch_node(SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon)
{
    const Value check_value = handle_expression(*node.main_expr);

    for (auto& case_part : node.cases)
    {
        for (auto& expr : case_part.exprs)
        {
            if (!handle_operator(expr.location(), check_value, OperatorType::EQ, handle_expression(expr)).as_bool())
                continue;

            const PopHandler pop_handler = stack.push();
            handle_scope(case_part.scope, retval, didbrk, didcon);
            return;
        }
    }

    if (node.def_scope)
    {
        const PopHandler pop_handler = stack.push();
        handle_scope(*node.def_scope, retval, didbrk, didcon);
    }
}

void dawn::Engine::handle_loop_node(LoopNode const& node, Opt<Value>& retval)
{
    Bool didbrk = false, didcon = false;
    while (true)
    {
        if (retval || didbrk)
            break;
        didcon = false;

        const PopHandler pop_handler = stack.push();
        handle_scope(node.scope, retval, &didbrk, &didcon);
    }
}

void dawn::Engine::handle_while_node(WhileNode const& node, Opt<Value>& retval)
{
    Bool didbrk = false, didcon = false;
    while (true)
    {
        if (retval || didbrk)
            break;
        didcon = false;

        if (!handle_expression(*node.expr).as_bool())
            break;

        const PopHandler pop_handler = stack.push();
        handle_scope(node.scope, retval, &didbrk, &didcon);
    }
}

void dawn::Engine::handle_for_node(ForNode const& node, Opt<Value>& retval)
{
    Value loop_value = handle_expression(*node.expr);
    const ID type_id = loop_value.type_id();
    if (type_id == id_string)
    {
        auto& value_str = loop_value.as_string();

        Bool didbrk = false, didcon = false;
        for (Char& c : value_str)
        {
            if (retval || didbrk)
                break;
            didcon = false;

            const PopHandler pop_handler = stack.push();
            stack.current().set(node.var_id, Value{&c, loop_value.is_const(), node.location});
            handle_scope(node.scope, retval, &didbrk, &didcon);
        }
    }
    else if (type_id == id_range)
    {
        auto& value_rng = loop_value.as_range();

#define FOR_LOOP_RANGE_BODY                                                                                            \
    {                                                                                                                  \
        if (retval || didbrk)                                                                                          \
            break;                                                                                                     \
        didcon = false;                                                                                                \
        const PopHandler pop_handler = stack.push();                                                                   \
        stack.current().set(node.var_id, Value{i, node.location});                                                     \
        handle_scope(node.scope, retval, &didbrk, &didcon);                                                            \
    }

        Bool didbrk = false, didcon = false;
        if (value_rng.start <= value_rng.end)
        {
            if (value_rng.inclusive)
                for (Int i = value_rng.start; i <= value_rng.end; ++i)
                    FOR_LOOP_RANGE_BODY
            else
                for (Int i = value_rng.start; i < value_rng.end; ++i)
                    FOR_LOOP_RANGE_BODY
        }
        else
        {
            if (value_rng.inclusive)
                for (Int i = value_rng.start; i >= value_rng.end; --i)
                    FOR_LOOP_RANGE_BODY
            else
                for (Int i = value_rng.start; i > value_rng.end; --i)
                    FOR_LOOP_RANGE_BODY
        }
    }
    else if (type_id == id_array)
    {
        auto& value_arr = loop_value.as_array();

        Bool didbrk = false, didcon = false;
        for (auto& value : value_arr.data)
        {
            if (retval || didbrk)
                break;
            didcon = false;

            const PopHandler pop_handler = stack.push();
            stack.current().set(node.var_id, value);
            handle_scope(node.scope, retval, &didbrk, &didcon);
        }
    }
    else
        ENGINE_PANIC(node.location, "can not for loop [", type_id, "]");
}

dawn::Value const& dawn::Engine::handle_lambda_node(LambdaNode const& node)
{
    auto& func_val = node.func_value;
    func_val.as_function().as_lambda().frame = stack.peek();
    return func_val;
}

dawn::Value dawn::Engine::handle_enum_node(EnumNode const& node)
{
    auto* enum_ptr = enums.get(node.type_id);
    if (!enum_ptr)
        ENGINE_PANIC(node.location, "enum [", node.type_id, "] does not exist");

    auto* entry_ptr = enum_ptr->get(node.key_id);
    if (!entry_ptr)
        ENGINE_PANIC(node.location, "enum [", node.type_id, "] does not have key [", node.key_id, "]");

    EnumValue result{};
    result.parent_id = node.type_id;
    result.key_id = node.key_id;

    return Value{result, node.location};
}

dawn::Value dawn::Engine::handle_struct_node(StructNode const& node)
{
    auto* struct_ptr = structs.get(node.type_id);
    if (!struct_ptr)
        ENGINE_PANIC(node.location, "struct [", node.type_id, "] does not exist");
    auto& struc = *struct_ptr;

    Value value{StructValue{}, node.location};
    auto& struct_value = value.as_struct();
    struct_value.parent_id = node.type_id;

    struct_value.fields.reserve(struc.fields.size());

    if (auto* named_init = std::get_if<StructNode::NamedInit>(&node.init))
    {
        for (auto& field : struc.fields)
        {
            const auto it = std::find_if(named_init->args.begin(), named_init->args.end(),
                                         [&](auto& entry) { return entry.first == field.id; });
            const Value value = (it != named_init->args.end()) ? handle_expression(it->second).clone()
                                                               : create_default_value(field.type_id, node.location);
            if (value.type_id() != field.type_id)
                ENGINE_PANIC(node.location, "can not assign type [", value.type_id(), "] to type [", field.type_id,
                             "]");
            struct_value.fields.emplace_back(field.id, value);
        }
    }
    else
    {
        auto& args = std::get<StructNode::ListInit>(node.init).args;
        if (args.size() > struc.fields.size())
            ENGINE_PANIC(node.location, "struct [", struc.id, "] contains [", struc.fields.size(), "] fields but [",
                         args.size(), "] were passed");
        for (Int i = 0; i < (Int)struc.fields.size(); i++)
        {
            auto& field = struc.fields[i];
            const Value value = (i < (Int)args.size()) ? handle_expression(args[i]).clone()
                                                       : create_default_value(field.type_id, node.location);
            if (value.type_id() != field.type_id)
                ENGINE_PANIC(node.location, "can not assign type [", value.type_id(), "] to type [", field.type_id,
                             "]");
            struct_value.fields.emplace_back(field.id, value);
        }
    }

    return value;
}

dawn::Value dawn::Engine::handle_array_node(ArrayNode const& node)
{
    ArrayValue result{};
    if (std::holds_alternative<ArrayNode::ListInit>(node.init))
    {
        auto& init_data = std::get<ArrayNode::ListInit>(node.init);
        result.data.reserve(init_data.elements.size());
        for (auto& expr : init_data.elements)
            result.data.push_back(handle_expression(expr).clone());
    }
    else
    {
        auto& init_data = std::get<ArrayNode::SizedInit>(node.init);
        const Int size = handle_expression(*init_data.size_expr).as_int();
        if (size < 0)
            ENGINE_PANIC(node.location, "array size can not be negative");
        result.data.reserve(size);
        for (Int i = 0; i < size; i++)
            result.data.push_back(create_default_value(init_data.type_id, node.location));
    }
    return Value{result, node.location};
}

dawn::Value dawn::Engine::handle_access_node(AccessNode const& node)
{
    const Value left = handle_expression(*node.left_expr);
    if (structs.get(left.type_id()))
        if (auto* field_ptr = left.as_struct().get(node.right_id))
            return *field_ptr;
    auto* member_func = members.get_or_set(left.type_id()).get(node.right_id);
    if (!member_func)
        ENGINE_PANIC(node.location, "type [", left.type_id(), "] does not have member [", node.right_id, "]");
    return (*member_func)(node.location, *this, left);
}

dawn::Value dawn::Engine::handle_operator_node(OperatorNode const& node)
{
    const Value left = handle_expression(node.sides[0]); // Make sure that left side is computed first.
    return handle_operator(node.location, left, node.type, handle_expression(node.sides[1]));
}

dawn::Value dawn::Engine::handle_assign_node(AssignNode const& node)
{
    Value left = handle_expression(node.sides[0]);
    switch (node.type)
    {
    case AssignType::ASSIGN:
        left.assign(handle_expression(node.sides[1]));
        return left;

    case AssignType::ADD:
        left.assign(handle_operator(node.location, left, OperatorType::ADD, handle_expression(node.sides[1])));
        return left;

    case AssignType::SUB:
        left.assign(handle_operator(node.location, left, OperatorType::SUB, handle_expression(node.sides[1])));
        return left;

    case AssignType::MUL:
        left.assign(handle_operator(node.location, left, OperatorType::MUL, handle_expression(node.sides[1])));
        return left;

    case AssignType::DIV:
        left.assign(handle_operator(node.location, left, OperatorType::DIV, handle_expression(node.sides[1])));
        return left;

    case AssignType::POW:
        left.assign(handle_operator(node.location, left, OperatorType::POW, handle_expression(node.sides[1])));
        return left;

    case AssignType::MOD:
        left.assign(handle_operator(node.location, left, OperatorType::MOD, handle_expression(node.sides[1])));
        return left;

    default:
        ENGINE_PANIC(node.location, "unknown assign node type: ", (Int)node.type);
    }
}

dawn::Value dawn::Engine::handle_cast_node(CastNode const& node)
{
    const Value left_value = handle_expression(*node.left_expr);
    return handle_cast(node.location, left_value, node.right_type_id);
}

void dawn::Engine::handle_scope(Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon)
{
    for (auto& instr : scope.instr)
    {
        if (retval || (didbrk && *didbrk) || (didcon && *didcon))
            break;

        handle_instruction(instr, retval, didbrk, didcon);
    }
}

dawn::Value dawn::Engine::handle_operator(Location location, Value const& left, OperatorType op_type,
                                          Value const& right)
{
    auto* op_right_ids = operators[(Int)op_type].get(left.type_id());
    if (!op_right_ids)
        ENGINE_PANIC(location, "type [", left.type_id(), "] does not support operator [", op_type, "]");
    auto* func = op_right_ids->get(right.type_id());
    if (!func)
        ENGINE_PANIC(location, "type [", left.type_id(), "] does not support operator [", op_type,
                     "] with right type being [", right.type_id(), "]");

    using ProxyArg = uint64_t;
    static_assert(sizeof(ProxyArg) == sizeof(Value), "ProxyArg size must be the same as Value");
    static_assert(alignof(ProxyArg) == alignof(Value), "ProxyArg alignment must be the same as Value");
    ProxyArg proxy_args[2] = {
        reinterpret_cast<ProxyArg const&>(left),
        reinterpret_cast<ProxyArg const&>(
            right)}; // Improves performance by not calling the constructors or destructors of Value.

    return handle_function(location, *func, reinterpret_cast<Value*>(proxy_args), (Int)std::size(proxy_args));
}

dawn::Value dawn::Engine::handle_function(Location location, FunctionValue const& func, Value const* args,
                                          Int arg_count)
{
    if (auto* dfunc = func.dfunction())
    {
        if (dfunc->args.size() != arg_count)
        {
            if (func.is_global())
                ENGINE_PANIC(location, "invalid argument count for function [", func.as_global().id, "]");
            else if (func.is_method())
                ENGINE_PANIC(location, "invalid argument count for method [", func.as_method().id, "]");
            else
                ENGINE_PANIC(location, "invalid argument count for lambda");
        }

        const PopHandler pop_handler =
            stack.push_from(func.is_lambda() ? func.as_lambda().frame : RegisterRef<Frame>{});

        for (Int i = 0; i < arg_count; i++)
            add_variable(location, dfunc->args[i].type, dfunc->args[i].id, args[i]);

        Opt<Value> retval;
        handle_scope(dfunc->body, retval, nullptr, nullptr);
        return retval ? *retval : Value{};
    }
    else
    {
        auto& cfunc = *func.cfunction();
        return cfunc(location, *this, args, arg_count);
    }
}

dawn::Value dawn::Engine::handle_cast(Location location, Value const& left, ID cast_type)
{
    auto* cast_ptr = casts.get_or_set(left.type_id()).get(cast_type);
    if (!cast_ptr)
        ENGINE_PANIC(location, "cast [", left.type_id(), "] -> [", cast_type, "] not defined");
    return (*cast_ptr)(location, *this, left);
}

void dawn::Engine::handle_instruction(Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon)
{
    switch (node.type())
    {
    case NodeType::SCOPE: {
        const PopHandler pop_handler = stack.push();
        handle_scope(std::get<Scope>(node), retval, didbrk, didcon);
    }
    break;

    case NodeType::VARIABLE:
        handle_variable_node(std::get<VariableNode>(node));
        break;

    case NodeType::RETURN:
        handle_return_node(std::get<ReturnNode>(node), retval);
        break;

    case NodeType::BREAK:
        handle_break_node(std::get<BreakNode>(node), didbrk);
        break;

    case NodeType::CONTINUE:
        handle_continue_node(std::get<ContinueNode>(node), didcon);
        break;

    case NodeType::THROW:
        handle_throw_node(std::get<ThrowNode>(node));
        break;

    case NodeType::TRY:
        handle_try_node(std::get<TryNode>(node), retval, didbrk, didcon);
        break;

    case NodeType::IF:
        handle_if_node(std::get<IfNode>(node), retval, didbrk, didcon);
        break;

    case NodeType::SWITCH:
        handle_switch_node(std::get<SwitchNode>(node), retval, didbrk, didcon);
        break;

    case NodeType::LOOP:
        handle_loop_node(std::get<LoopNode>(node), retval);
        break;

    case NodeType::WHILE:
        handle_while_node(std::get<WhileNode>(node), retval);
        break;

    case NodeType::FOR:
        handle_for_node(std::get<ForNode>(node), retval);
        break;

    default:
        handle_expression(node);
    }
}

dawn::Value dawn::Engine::handle_expression(Node const& node)
{
    switch (node.type())
    {
    case NodeType::VALUE:
        return std::get<Value>(node);

    case NodeType::IDENTIFIER:
        return handle_identifier_node(std::get<IdentifierNode>(node));

    case NodeType::CALL:
        return handle_call_node(std::get<CallNode>(node));

    case NodeType::INDEX:
        return handle_index_node(std::get<IndexNode>(node));

    case NodeType::LAMBDA:
        return handle_lambda_node(std::get<LambdaNode>(node));

    case NodeType::ENUM:
        return handle_enum_node(std::get<EnumNode>(node));

    case NodeType::STRUCT:
        return handle_struct_node(std::get<StructNode>(node));

    case NodeType::ARRAY:
        return handle_array_node(std::get<ArrayNode>(node));

    case NodeType::ACCESS:
        return handle_access_node(std::get<AccessNode>(node));

    case NodeType::OPERATOR:
        return handle_operator_node(std::get<OperatorNode>(node));

    case NodeType::ASSIGN:
        return handle_assign_node(std::get<AssignNode>(node));

    case NodeType::CAST:
        return handle_cast_node(std::get<CastNode>(node));

    default:
        ENGINE_PANIC(node.location(), "unknown expr node type: ", (Int)node.type());
    }
}
