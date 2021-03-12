//
// Created by chris on 17/1/21.
//

#include "Checker.h"

#define T_NONE ObjectType(".None", {})

USemanticInfo Checker::visit(ListNode& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    const TypeNode& element_type = element_type_p->type();
    node.elements[0] = this->replace_if_necessary(node.elements[0]);
    for (int i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
        const TypeNode& current_type = current_type_p->type();
        node.elements[i] = this->replace_if_necessary(node.elements[i]);
        if (current_type != element_type) {
            this->error_list_literal(element_type, current_type, node.elements[i]->start);
        }
    }
    node.type = element_type.clone();
    SemanticInfo return_info;
    return_info.is_function = false;
    return_info.set_type(ObjectType("List", {element_type.clone()}));
    return std::make_unique<SemanticInfo>(return_info);
}

USemanticInfo Checker::visit(BooleanNode& node) {
    SemanticInfo info;
    info.set_type(T_BOOL);
    info.is_function = false;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(WhileNode& node) {
    USemanticInfo condition_p = this->dispatch(node.condition);
    SemanticInfo& condition = *condition_p;
    if (condition.type() != T_BOOL) {
        this->error_condition(condition.type(), node.start, "elif");
    }
    this->enter_scope("while");
    this->visit(*node.body);
    for (auto v: this->scope->table) {
        node.body->local_vars.push_back(std::make_pair(v.first,v.second->clone()));
    }
    this->leave_scope();
    return nullptr;
}

USemanticInfo Checker::visit(NumberNode& node) {
    SemanticInfo info;
    info.set_type(T_INT);
    info.is_function = false;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(StringNode& node) {
    SemanticInfo info;
    info.set_type(T_STRING);
    info.is_function = false;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    SemanticInfo& parent = *parent_p;
    if (parent.type().kind != Kind::OBJECT) {
        this->error_subscript_non_object(node.start);
        return this->error();
    }
    SemanticInfo info;
    const ObjectType& object_type = parent.type().object();

    if (object_type.id == "String") {
        if (this->is_lvalue) {
            this->error_string_immutable(node.start);
            return this->error();
        }
    }

    VectorOfTypes children;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch(c);
    bool is_integer = ct->type() == T_INT;
    children.emplace_back(ct->type().clone());
    this->is_lvalue = old_lvalue;
    node.parent_t = object_type.clone();
    if (object_type.id == "List") {
        if (!is_integer) {
            this->error_subscript_type(object_type, ct->type(), T_INT, node.start);
        }
        info.set_type(*object_type.type_params[0]);
    } else if (object_type.id == "Dict") {
        TypeNode* key_type = object_type.type_params[0];
        if (ct->type() != *key_type) {
            throw std::runtime_error(
                    "Error key of dictionary must be of type " + key_type->to_string() + " but it is " +
                    ct->type().to_string());
        }
        TypeNode* value_type = object_type.type_params[1];
        info.set_type(*value_type);
    } else if (object_type.id == "String") {
        if (!is_integer) {
            this->error_subscript_type(object_type, ct->type(), T_INT, node.start);
        }
        info.set_type(object_type);
    }

    info.is_function = false;

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BreakNode& node) {
    return nullptr;
}

USemanticInfo Checker::visit(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto& expression_type = expression_info.type().object();

    if (expression_type.id != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    TypeNode& type = *expression_type.type_params[0];
    semanticInfo.set_type(type);
    this->enter_scope("true_case");
    this->scope->set("it", type);
    USemanticInfo true_case_p = this->dispatch(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    node.true_case = this->replace_if_necessary(node.true_case);
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch(node.false_case);
    SemanticInfo& false_case = *false_case_p;
    node.false_case = this->replace_if_necessary(node.false_case);
    if (false_case.type() != true_case.type()) {
        throw std::runtime_error("True case and false case type don't match: " + true_case.type().to_string() + " != " +
                                 false_case.type().to_string());
    } else {
        semanticInfo.set_type(true_case.type());
    }
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(NoneNode& node) {
    SemanticInfo info;
    info.set_type(ObjectType("NoneType", {}));
    info.is_function = false;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(EmptyListNode& node) {
    SemanticInfo info;
    info.set_type(T_LIST(node.type->clone()));
    info.is_function = false;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(ClassNode& node) {
    this->current_class = node.class_name;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;
    inits = std::map<std::string, bool>();

    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        inits[mt] = false;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
    }

    this->this_type = new ObjectType(node.class_name, tp);
    bool has_init = false;

    for (auto method: node.methods) {
        this->is_method = true;
        this->visit(*method.second);
        has_init = has_init || method.first == "init";
    }
    if (!has_init) {
        BlockNode* init_body = new BlockNode({});
        for (auto mt: node.members_ordered) {
            init_body->nodes.push_back(new AssignmentNode(new MemberNode(new IdNode("this"), mt), new IdNode(mt)));
        }
        node.methods["init"] = new FunctionNode("init",
                                                node.members_ordered,
                                                members_ordered_types,
                                                new ObjectType(node.class_name, tp),
                                                init_body);
        this->is_method = true;
        this->visit(*node.methods["init"]);
    }

    this->is_method = false;
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->current_class = "";
    return nullptr;
}

USemanticInfo Checker::visit(ContinueNode& node) {
    return nullptr;
}

USemanticInfo Checker::visit(TupleNode& node) {
    VectorOfTypes types;
    int i = 0;
    for (auto n: node.values) {
        i++;
        USemanticInfo vtype = this->dispatch(n);
        types.emplace_back(vtype->type().clone());
        if (!this->is_immutable(vtype->type())) {
            this->error_tuple_member_not_immutable(vtype->type(), node.start);
            return this->error();
        }
    }
    ObjectType tuple_type("Tuple", types);
    SemanticInfo sinfo;
    sinfo.set_type(tuple_type);
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit(FloatNode& node) {
    SemanticInfo s;
    s.set_type(T_FLOAT);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit(PartialApplication& node) {
    USemanticInfo func = this->dispatch(node.function);
    VectorOfTypes partial_args;
    if (node.args.size() != func->type().function().param_types.size()) {
        this->error_partial_wrong_num_args(node.start);
        return this->error();
    }
    for (int i = 0; i < node.args.size(); i++) {
        if (node.args[i] != nullptr) {
            USemanticInfo arg = this->dispatch(node.args[i]);
            if (arg->type() != *func->type().function().param_types[i]) {
                this->error_partial_function_call_type_mismatch(*func->type().function().param_types[i],
                                                                arg->type(),
                                                                node.args[i]->start,
                                                                node.args[i]->end);
                return this->error();
            }
        } else {
            partial_args.push_back(func->type().function().param_types[i]->clone());
        }
    }
    node.complete_type = &func->type().clone()->function();
    SemanticInfo s;
    s.set_type(FunctionType(partial_args, func->type().function().return_type->clone()));
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit(ClassLiteralFieldNode& node) {
    ObjectType* object_type = node.type;
    std::string& object_type_id = object_type->id;
    const std::string& object_type_str = object_type->to_string();

    if (!this->class_table->declared(object_type_id)) {
        this->error_class_not_found(*object_type, node.start);
        return this->error();
    }

    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_params.size();
    unsigned long num_actual_type_params = object_type->type_params.size();
    if (num_required_type_params != 0) {
        // it's a generic class
        if (num_required_type_params != num_actual_type_params) {
            this->error_generic_class_wrong_type_param_number(object_type->id,
                                                              num_required_type_params,
                                                              num_actual_type_params,
                                                              node.start);
            return this->error();
        }
        if (this->class_table->declared(object_type_str)) {
            class_info = this->class_table->get(object_type_str);
        } else {
            class_info = instantiate_generic(class_info, *object_type);
            this->class_table->set(object_type_str, class_info);
        }
    } else if (num_actual_type_params != 0) {
        this->error_class_not_generic(object_type->id, node.start);
        return this->error();
    }
    auto class_fields = class_info->members;

    for (int i = 0; i < node.init_names.size(); i++) {
        if (class_fields.find(node.init_names[i]) == class_fields.end()) {
            this->error_no_member(*node.type, node.init_names[i], node.start);
            SemanticInfo rv;
            rv.set_type(*object_type);
            return std::make_unique<SemanticInfo>(rv);
        }
    }
    if (class_fields.size() != node.init_names.size()) {
        this->error_class_init_wrong_number_init(object_type_id,
                                                 class_fields.size(),
                                                 node.init_names.size(),
                                                 node.start);
        return this->error();
    }
    for (int i = 0; i < node.init_names.size(); i++) {
        Node* exp = node.init_values[i];
        USemanticInfo semanticInfo_p = this->dispatch(exp);
        SemanticInfo& semanticInfo = *semanticInfo_p;
        node.init_values[i] = this->replace_if_necessary(node.init_values[i]);
        TypeNode& field_type = *class_fields[node.init_names[i]];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            this->error_class_init_bad_member_type(*object_type, field_type, semanticInfo.type(), exp->start);
        }
    }
    SemanticInfo rv;
    rv.set_type(*object_type);
    return std::make_unique<SemanticInfo>(rv);
}

USemanticInfo Checker::visit(ForNode& node) {
    USemanticInfo info_p = this->dispatch(node.exp);
    SemanticInfo& info = *info_p;
    if (info.type().kind != Kind::OBJECT) {
        this->error_for(info.type(), node.start);
    }

    const ObjectType& obj = info.type().object();
    if (obj.id != "List") {
        this->error_for(obj, node.start);
    }
    IdNode* lid = new IdNode("List.len");
    lid->is_global_function = true;
    CallNode* len_call = new CallNode(lid, {new IdNode(".list0")});
    len_call->arg_types.push_back(obj.clone());
    Node* new_condition = new BoolOpNode(BoolOp::LT, new IdNode(".index0"), len_call);

    BlockNode* new_body = new BlockNode({});
    new_body->nodes.push_back(new DeclarationNode(node.var,
                                                  obj.type_params[0]->clone(),
                                                  new SubscriptNode(new IdNode("_list0"), {new IdNode("_index0")})));
    new_body->nodes.insert(new_body->nodes.end(), node.body->nodes.begin(), node.body->nodes.end());
    AssignmentNode* asn = new AssignmentNode(new IdNode("_index0"),
                                             new BinopNode(OpType::ADD, new IdNode(".index0"), new NumberNode(1)));
    asn->type = new T_INT;
    new_body->nodes.push_back(asn);

    TypeNode& var_type = *obj.type_params[0];
    BlockNode* bn = new BlockNode({new DeclarationNode("_index0", new T_INT, new NumberNode(0)),
                                   new DeclarationNode("_list0", obj.clone(), node.exp),});
    this->visit(*bn);
    this->enter_scope("for");
    this->scope->set(node.var, var_type);


    this->visit(*node.body);
    this->dispatch(new_body->nodes[0]->decl().expression);
    for (auto v: this->scope->table) {
        new_body->local_vars.push_back(std::make_pair(v.first,v.second->clone()));
    }
    this->leave_scope();
    this->replacement = bn;
    this->replace_me = true;
    bn->nodes.push_back(new WhileNode(new_condition, new_body));
    return nullptr;
}

USemanticInfo Checker::visit(CallNode& n) {
    USemanticInfo fun_info_p = this->dispatch(n.function);
    if (fun_info_p->is_error) {
        return this->error();
    }
    SemanticInfo& fun_info = *fun_info_p;
    bool is_a_method = false;
    Node* object_node;
    SemanticInfo retv;
    if (fun_info.is_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)

        MemberNode& member_node = n.function->member();
        IdNode* pNode = new IdNode(fun_info.class_info->class_name + "." + member_node.s_child);
        pNode->location = VariableLocation(-2, -1);
        n.function = pNode;
        pNode->is_global_function = true;
        this->replace_me = false;
        object_node = member_node.parent;
        is_a_method = true;
    } else if (fun_info.is_class_method) {
        MemberNode& member_node = n.function->member();
        if (member_node.s_child == "init") {
            n.function = this->replacement;
            this->replace_me = false;
            const FunctionType& ftn = fun_info.type().function();
            FunctionType& copy_ftn = ftn.clone()->function();
            retv.set_type(*copy_ftn.clone());
            object_node = member_node.parent;
        } else {
            n.function = new IdNode(fun_info.class_info->class_name + "." + member_node.s_child);
            this->replace_me = false;
            const FunctionType& ftn = fun_info.type().function();
            FunctionType& copy_ftn = ftn.clone()->function();
            copy_ftn.param_types.insert(copy_ftn.param_types.begin(), TYPE(fun_info.class_info->class_name, {}));
            retv.set_type(*copy_ftn.clone());
            object_node = member_node.parent;
        }
    }
    if (fun_info.is_function || fun_info.is_method || fun_info.is_class_method) {
        // ok
        const FunctionType& function_type = fun_info.type().function();
        if (n.arguments.size() != function_type.param_types.size()) {
            this->error_function_call_num_args(n.start);
            if (!function_is_generic(function_type)) {
                retv.set_type(*function_type.return_type);
                return std::make_unique<SemanticInfo>(retv);
            } else {
                return this->error();
            }
        }
        VectorOfTypes arg_types;
        for (auto& arg: n.arguments) {
            USemanticInfo arg_type_p = this->dispatch(arg);
            const TypeNode& arg_type = arg_type_p->type();
            arg = this->replace_if_necessary(arg);
            arg_types.push_back(arg_type.clone());
            n.arg_types.push_back(arg_type.clone());
        }

        if (function_is_generic(function_type)) {
            retv = match_arguments_to_generic_function(function_type, arg_types);
        } else {
            retv.set_type(*function_type.return_type);
            for (int i = 0; i < n.arguments.size(); i++) {
                const TypeNode& arg_type = *arg_types[i];
                const TypeNode& param_type = *function_type.param_types[i];
                if (arg_type != param_type) {
                    if (arg_type.kind != Kind::UNKNOWN) {
                        this->error_function_call_type_mismatch(param_type,
                                                                arg_type,
                                                                n.arguments[i]->start,
                                                                n.arguments[i]->end);
                    }
                    return std::make_unique<SemanticInfo>(retv);
                }
            }
        }
    } else {
        this->error_call_not_a_function(n.start);
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
    return std::make_unique<SemanticInfo>(retv);
}

USemanticInfo Checker::visit(BlockNode& program) {
    for (auto& n: program.nodes) {
        USemanticInfo sinfo_p = this->dispatch(n);
        n = this->replace_if_necessary(n);
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value
            if (!sinfo.is_error && sinfo.type() != T_NONE) {
                this->error_unused_return_value(n->start);
            }
        }
    }
    return nullptr;
}

USemanticInfo Checker::visit(ClassLiteralExpressionNode& node) {
    ObjectType& object_type = *node.type;
    std::string& object_type_id = object_type.id;
    const std::string& object_type_str = object_type.to_string();

    if (!this->class_table->declared(object_type_id)) {
        this->error_class_not_found(object_type, node.start);
        return this->error();
    }
    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_params.size();
    unsigned long num_actual_type_params = object_type.type_params.size();
    if (num_required_type_params != 0) {
        // it's a generic class
        if (num_required_type_params != num_actual_type_params) {
            this->error_generic_class_wrong_type_param_number(object_type.id,
                                                              num_required_type_params,
                                                              num_actual_type_params,
                                                              node.start);
        }
        if (this->class_table->declared(object_type_str)) {
            class_info = this->class_table->get(object_type_str);
        } else {
            class_info = instantiate_generic(class_info, object_type);
            this->class_table->set(object_type_str, class_info);
        }
    } else if (num_actual_type_params != 0) {
        this->error_class_not_generic(object_type_id, node.start);
        return this->error();
    }
    auto class_field_types_ordered = class_info->member_types;
    auto class_field_names_ordered = class_info->member_names;

    unsigned long num_required_init = class_field_names_ordered.size();
    unsigned long num_actual_init = node.init.size();
    if (num_required_init != num_actual_init) {
        this->error_class_init_wrong_number_init(object_type_id, num_required_init, num_actual_init, node.start);
        return this->error();
    }

    for (int i = 0; i < num_actual_init; i++) {
        Node* exp = node.init[i];
        USemanticInfo semanticInfo_p = this->dispatch(exp);
        SemanticInfo& semanticInfo = *semanticInfo_p;
        node.init[i] = this->replace_if_necessary(node.init[i]);
        TypeNode& field_type = *class_field_types_ordered[i];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            this->error_class_init_bad_member_type(object_type, field_type, semanticInfo.type(), node.init[i]->start);
        }
    }
    node.names = class_field_names_ordered;
    SemanticInfo rv;
    rv.set_type(object_type);
    return std::make_unique<SemanticInfo>(rv);
}

USemanticInfo Checker::visit(FunctionNode& n) {
    std::string& function_name = n.identifier;
    this->current_function = function_name;
    this->enter_scope(function_name);
    this->scope->is_function = true;
    bool is_init_method = this->is_method && function_name == "init";
    if (this->add_this) {
        this->scope->set("this", *this->this_type);
    }
    for (int i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        if (type.kind == Kind::OBJECT) {
            this->assert_type_exists(type, n.start);
        }
        this->scope->set(n.parameter_names[i], type);
    }

    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    this->scope->set("__return__", returnType);
    this->visit(*n.body);
    for (auto v: this->scope->table) {
        n.body->local_vars.push_back(std::make_pair(v.first,v.second->clone()));
    }
    if (is_init_method) {
        for (int i = 0; i < n.body->nodes.size(); i++) {
            if (n.body->nodes[i]->ntype == ASSIGN) {
                AssignmentNode& nod = n.body->nodes[i]->assign();
                if (nod.lvalue->ntype == MEMBER) {
                    MemberNode& mem = nod.lvalue->member();
                    if (mem.parent->ntype == ID) {
                        if (mem.parent->id()._id == "this") {
                            inits[mem.s_child] = true;
                        }
                    }
                }
            }
        }
        bool er = false;
        for (auto x: this->inits) {
            if (x.second == false) {
                er = true;
                this->error_class_init_member_not_init(this->current_class, x.first, n.start);
                // std::cout << "MEMBER " + x.first + " not initialized in init method!" << std::endl;
            }
        }
        if (er) {
            throw std::runtime_error("FAILED");
        }
        this->leave_scope();
        return nullptr;
    }
    if (returnType != T_NONE) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes.back();
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_function_return_last_stmt(function_name, returnType, last_node->start);
            }
        } else {
            this->error_function_return_last_stmt(function_name, returnType, n.start);
        }
    }
    this->leave_scope();
    return nullptr;
}

USemanticInfo Checker::visit(IdNode& n) {
    SemanticInfo info;
    info.is_function = false;
    info.is_method = false;
    if (!this->scope->has(n._id)) {
        // it might be a function name
        if (this->function_table->has_function(n._id)) {
            n.is_global_function = true;
            info.is_function = true;
            n.location = VariableLocation(-2, -1);
            info.set_type(this->function_table->get(n._id));
        } else {
            this->error_variable_not_declared(n._id, n.start);
            return this->error();
        }
    } else {
        info.set_type(this->scope->get(n._id));
        if (info.type().kind == Kind::UNKNOWN) {
            info.is_error = true;
        } else {
            n.location = this->scope->find(n._id);
            if (info.type().kind == Kind::OBJECT) {
                const ObjectType& otn = info.type().object();
                if (otn.id == "Option") {
                    if (this->scope->get_not_none(n._id)) {
                        info.set_type(*otn.type_params[0]);
                    }
                }

            }
            if (info.type().kind == Kind::FUNCTION) {
                info.is_function = true;
                info.is_method = false;
                info.is_class_method = false;
            }
        }
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        this->error_redeclared(n.identifier, n.start);
        this->failed = true;
    }
    SemanticInfo info;
    info.is_function = false;
    if (n.expression->ntype != NodeType::UNINITIALIZED and n.type != nullptr) {
        TypeNode& n_type = *n.type;
        if (!this->assert_type_exists(n_type, n.start)) {
            return this->error();
        }
        USemanticInfo exp_info_p = this->dispatch(n.expression);
        SemanticInfo& exp_info = *exp_info_p;
        if (exp_info.is_error) {
            this->scope->set(n.identifier, n_type);
            return std::make_unique<SemanticInfo>(info);
        }
        n.expression = this->replace_if_necessary(n.expression);
        if (n.type->kind == Kind::FUNCTION) {
            // it's a function
            if (n_type != exp_info.type()) {
                this->error_assignment(n_type, exp_info.type(), n.start);
            }
        } else {
            SemanticInfo expression_info = exp_info;
            const ObjectType& actual_type = n.type->object();
            const TypeNode& exp_type = expression_info.type();
            if (actual_type.id == "Option") {
                if (*actual_type.type_params[0] != exp_type) {
                    auto foo = exp_type.object();
                    if (foo.id != "NoneType") {
                        this->error_assignment(n_type, exp_type, n.start);
                    }
                }
            } else if (actual_type.id == "Union") {
                bool ok = false;
                for (auto type_param: actual_type.type_params) {
                    if (*type_param != exp_type) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    this->error_assignment(n_type, exp_type, n.start);
                }
            } else {
                if (n_type != exp_type) {
                    this->error_assignment(n_type, exp_type, n.start);
                }
            }
        }
        info.set_type(n_type);

    } else if (n.expression->ntype != NodeType::UNINITIALIZED) {
        USemanticInfo exp_info_p = this->dispatch(n.expression);
        if (exp_info_p->type() == T_NONE) {
            this->error_function_doesnt_return_a_value(n.expression->start, nullptr);
            USemanticInfo error_t = this->error();
            this->scope->set(n.identifier, error_t->type());
            return error_t;
        }
        n.type = exp_info_p->type().clone();
        SemanticInfo& exp_info = *exp_info_p;
        n.expression = this->replace_if_necessary(n.expression);
        info.set_type(exp_info.type());
    }
    this->scope->set(n.identifier, info.type());
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(AssignmentNode& n) {
    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            this->dispatch(n.rvalue);
            return nullptr;
        }
    }
    this->is_lvalue = true;
    USemanticInfo linfo_p = this->dispatch(n.lvalue);
    if (linfo_p->is_error) {
        return nullptr;
    }
    this->is_lvalue = false;
    if (n.lvalue->ntype == MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_tuple_assign(n.start);
        this->failed = true;
    }
    USemanticInfo expression_type_p = this->dispatch(n.rvalue);
    if (expression_type_p->type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.rvalue->start, &linfo_p->type());
        return nullptr;
    }
    if (expression_type_p->is_error) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;
    SemanticInfo& expression_type = *expression_type_p;
    n.rvalue = this->replace_if_necessary(n.rvalue);

    const TypeNode& l_type = linfo.type();
    const TypeNode& exp_type = expression_type.type();

    if (l_type != exp_type) {
        if (l_type.kind == Kind::OBJECT) {
            const ObjectType& actual_type = l_type.object();
            if (actual_type.id == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (*actual_type.type_params[0] != exp_type) {
                    auto& foo = exp_type.object();
                    if (foo.id != "NoneType") {
                        this->error_assignment(l_type, exp_type, n.start);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_assignment(l_type, exp_type, n.start);
            }
        } else {
            // if it's not Option[t], then it's an error
            this->error_assignment(l_type, exp_type, n.start);
        }
    }
    // else, type matches don't do anything
    n.type = l_type.clone();
    return nullptr;
}

USemanticInfo Checker::visit(MemberNode& n) {
    SemanticInfo rv;
    std::string& child = n.s_child;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& id_node = n.parent->id();
        // It might be something like <class>.<method>, so we need to handle this case differently
        if (this->class_table->declared(id_node._id)) {
            ClassInfo* class_info = this->class_table->get(id_node._id);
            std::string& class_name = class_info->class_name;
            if (class_info->methods.find(child) != class_info->methods.end()) {

                if (child == "init") {
                    const FunctionType& ft = *class_info->methods.find(child)->second;
                    VectorOfTypes params = ft.param_types;
                    for (int i = 0; i < params.size(); i++) {
                        params[i] = params[i]->clone();
                    }
                    VectorOfTypes tp;
                    for (auto t: class_info->type_params) {
                        tp.push_back(new ObjectType(t, {}));
                    }
                    FunctionType f(params, new ObjectType(class_name, tp));
                    rv.set_type(f);
                    rv.is_class_method = true;
                    this->replace_me = true;
                    IdNode* idn = new IdNode(class_name + "." + child);
                    idn->is_global_function = true;
                    this->replacement = idn;
                    return std::make_unique<SemanticInfo>(rv);
                }

                rv.set_type(*class_info->methods.find(child)->second);
                rv.class_info = class_info;

                const FunctionType& ftn = rv.type().function();
                FunctionType& copy_ftn = *ftn.clone();
                VectorOfTypes tp;
                for (auto tttp: rv.class_info->type_params) {
                    tp.push_back(new ObjectType(tttp, {}));
                }
                auto instance_type = new ObjectType(rv.class_info->class_name, tp);
                copy_ftn.param_types.insert(copy_ftn.param_types.begin(), instance_type);

                rv.set_type(copy_ftn);
                rv.is_method = false;
                rv.is_class_method = true;
                this->replace_me = true;
                IdNode* idn = new IdNode(class_name + "." + child);
                idn->is_global_function = true;
                this->replacement = idn;
                return std::make_unique<SemanticInfo>(rv);
            } else {
                this->error_class_no_method(class_name, child, n.start);
                return this->error();
            }
        }
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    USemanticInfo symbol_info_p = this->dispatch(n.parent);
    this->is_lvalue = old_lvalue;
    SemanticInfo& info = *symbol_info_p;
    if (info.is_error) {
        return this->error();
    }
    if (info.type().kind != Kind::OBJECT) {
        this->error_member_no_object(n.start);
        return this->error();
    }
    n.parent_t = symbol_info_p->type().clone();
    const ObjectType& object = info.type().object();
    const ObjectType* option_type = nullptr;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& idn = n.parent->id();
        if (object.id == "Option") {
            if (this->scope->get_not_none(idn._id)) {
                // we can guarantee that it's not null, so we can access the members
                option_type = &(object.type_params[0])->object();
            } else {
                throw std::runtime_error(
                        "Error: line " + text_pos_to_string(this->__file__, idn.start) + " -> " + idn._id +
                        " might be none here, make sure to  this in a if XXX != none {...}!");
            }
        }
    }
    const ObjectType& final_type = option_type != nullptr ? *option_type : object;
    if (final_type.id == "Tuple") {
        // special treatment for tuples
        if (n.type != MemberType::NUM) {
            throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                     std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
        }
        if (n.n_child < 1 || n.n_child > final_type.type_params.size()) {
            throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                     std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
        }
        SemanticInfo s;
        s.set_type(*final_type.type_params[n.n_child - 1]);
        return std::make_unique<SemanticInfo>(s);
    } else {
        if (n.type != MemberType::STR) {
            throw std::runtime_error(
                    "Error: can access number member for tuple types only, but got " + final_type.to_string());
        }

        ClassInfo* class_info;
        if (this->class_table->declared(final_type.to_string())) {
            class_info = this->class_table->get(final_type.to_string());
        } else {
            if (is_generic((final_type)) && final_type.type_params.size() == 0) {
                throw std::runtime_error(
                        "Cannot access member of totally generic value of generic type " + object.id + "!");
            }
            class_info = this->class_table->get(object.id);
            class_info = instantiate_generic(class_info, final_type);
            this->class_table->set(object.to_string(), class_info);
        }
        if (class_info->members.find(child) != class_info->members.end()) {
            // It's a member
            info.set_type(*class_info->members[child]);
            rv = info;
            rv.is_function = false;
            rv.is_method = false;
        } else if (class_info->methods.find(child) != class_info->methods.end()) {
            // It's a method
            info.set_type(*class_info->methods.find(child)->second);
            rv = info;
            rv.is_method = true;
            rv.class_info = class_info;
        } else {
            this->error_no_member(object, child, n.start);
            return this->error();
        }
    }

    return std::make_unique<SemanticInfo>(rv);

}

USemanticInfo Checker::visit(IfNode& n) {
    SemanticInfo info;
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    std::unordered_map<std::string, bool> not_null_vars;
    if (condition_info.type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.condition->start, new T_BOOL);
    } else if (condition_info.type() != T_BOOL) {
        this->error_condition(condition_info.type(), n.start, "if");
    }

    this->enter_scope("if");
    this->visit(*n.then);
    for (auto v: this->scope->table) {
        n.then->local_vars.push_back(std::make_pair(v.first,v.second->clone()));
    }
    this->leave_scope();

    for (int i = 0; i < n.elifs.size(); i++) {
        condition_info_p = this->dispatch(n.elifs[i].first);
        SemanticInfo& condition_info = *condition_info_p;
        if (condition_info.type() != T_BOOL) {
            this->error_condition(condition_info.type(), n.start, "elif");
        }
        this->enter_scope("elif");
        this->visit(*n.elifs[i].second);
        this->leave_scope();
    }
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        this->visit(*n.selse);
        this->leave_scope();
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    SemanticInfo info;
    bool ok = false;
    const TypeNode& l_type = left_info.type();
    const TypeNode& r_type = right_info.type();
    if (l_type.kind == Kind::OBJECT) {
        auto& left = l_type.object();
        if (r_type.kind == Kind::OBJECT) {
            auto& right = r_type.object();
            if (left.id == "Option" && right.id == "NoneType") {
                info.set_type(T_BOOL);
                ok = true;
            }
        }
    }
    if (!ok && l_type != r_type) {
        this->error_bool_op(l_type, r_type, n.start);
    }

    info.set_type(T_BOOL);
    n.ltype = l_type.clone();
    n.rtype = r_type.clone();

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BinopNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    Node* left_replace = this->replace_if_necessary(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);
    bool err = false;
    if (left_info_p->type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.left->start, nullptr);
        err = true;
    }

    if (right_info_p->type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.right->start, nullptr);
        err = true;
    }
    if (err) {
        return this->error();
    }
    n.left = left_replace;
    // n.left = this->replace_if_necessary(n.left);
    n.right = this->replace_if_necessary(n.right);

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    if (left_info.is_error || right_info.is_error) {
        auto e = ErrorStub();
        auto s = std::make_unique<SemanticInfo>(e);
        return s;
    }

    SemanticInfo info;
    auto& left = left_info.type().object();
    auto& right = right_info.type().object();
    auto ltype = left.id;
    auto rtype = right.id;
    bool ok = false;
    if (ltype == "Integer" && rtype == "Integer") {
        info.set_type(T_INT);
        info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Float") {
        info.set_type(ObjectType("Float", {}));
        info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Integer" || ltype == "Integer" && rtype == "Float") {
        info.set_type(ObjectType("Float", {}));
        info.is_function = false;
        ok = true;
    } else if (ltype == "String" && rtype == "String") {
        if (n.op == OpType::ADD) {
            info.set_type(T_STRING);
            info.is_function = false;
            IdNode* idn = new IdNode("String_add");
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}));
            ok = true;
        }
    } else if (ltype == "List" && rtype == "List" && left == right) {
        if (n.op == OpType::ADD) {
            info.set_type(left);
            info.is_function = false;
            IdNode* idn = new IdNode("List_add");
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}));
            ok = true;
        }
    }

    if (!ok) {
        this->error_binop(left, right, n.op_pos);
        return std::make_unique<ErrorStub>(ErrorStub());
    }

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(ReturnNode& n) {
    const TypeNode& return_type = this->scope->get("__return__");
    if (return_type == T_NONE) {
        if (n.expression != nullptr) {
            this->error_bad_return(n.start);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_no_return(return_type, n.start);
    }
    USemanticInfo expression_info_p = this->dispatch(n.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.is_error) {
        return nullptr;
    }
    n.expression = this->replace_if_necessary(n.expression);
    if (!this->can_assign(expression_info.type(), return_type)) {
        this->error_return_mismatch(return_type, expression_info.type(), n.start);
        this->failed = true;
        return this->error();
    }
    n.reachables = this->scope->get_all();
    return nullptr;
}

USemanticInfo Checker::visit(DictNode& node) {
    SemanticInfo info;
    USemanticInfo first_key_type = this->dispatch(node.items[0].first);
    USemanticInfo first_value_type = this->dispatch(node.items[0].second);

    for (int i = 1; i < node.items.size(); i++) {
        USemanticInfo key_type = this->dispatch(node.items[i].first);
        USemanticInfo value_type = this->dispatch(node.items[i].second);
        if (key_type->type() != first_key_type->type()) {
            throw std::runtime_error("Second key type different to first");
        }
        if (value_type->type() != first_value_type->type()) {
            throw std::runtime_error("Second value type different to first");
        }
    }
    info.set_type(ObjectType("Dict", {first_key_type->type().clone(), first_value_type->type().clone()}));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(EmptyDictNode& node) {
    SemanticInfo info;
    info.set_type(ObjectType("Dict", {node.key_type, node.value_type}));
    return std::make_unique<SemanticInfo>(info);
}
