//
// Created by chris on 17/1/21.
//

#include "Checker.h"


SNode* Checker::make_for_snode(ForNode& node, USemanticInfo& binfo, USemanticInfo& exp_info_p) {
    BlockSNode* bbn = new BlockSNode();
    DeclarationSNode* dsn = new DeclarationSNode();
    WhileSNode* wsn = new WhileSNode();


    dsn->identifier = this->loop_list_var_id;
    dsn->expression = exp_info_p->snode;
    bbn->nodes.push_back(dsn);
    DeclarationSNode* lidx_decl = new DeclarationSNode();
    lidx_decl->identifier = this->loop_index_var_id;
    IntegerSNode* init_idx = new IntegerSNode();
    init_idx->str = "0";
    lidx_decl->expression = init_idx;

    bbn->nodes.push_back(lidx_decl);

    DeclarationSNode* lensn = new DeclarationSNode();
    lensn->identifier = this->loop_list_len_var_id;
    CallSNode* call_list_len_sn = new CallSNode();
    IdSNode* list_len_fn = new IdSNode("core.List.len");
    call_list_len_sn->function = list_len_fn;
    IdSNode* list_sn = new IdSNode(this->loop_list_var_id);
    call_list_len_sn->arguments = {list_sn};
    lensn->expression = call_list_len_sn;
    bbn->nodes.push_back(lensn);


    IdSNode* idxsn = new IdSNode(this->loop_index_var_id);
    CallSNode* cn = new CallSNode();
    IdSNode* cmpfunsn = new IdSNode("core.Integer.lt");

    IdSNode* llensn = new IdSNode(this->loop_list_len_var_id);


    cn->function = cmpfunsn;
    cn->arguments = {idxsn, llensn};

    wsn->condition = cn;

    bbn->nodes.push_back(wsn);


    BlockSNode* bn = (BlockSNode*) (binfo->snode);
    DeclarationSNode* loop_elem_sn = new DeclarationSNode();

    CallSNode* list_subscript_n = new CallSNode();
    list_subscript_n->function = new IdSNode("core.List.__sub__");
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_list_var_id));
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_index_var_id));


    loop_elem_sn->expression = list_subscript_n;
    loop_elem_sn->identifier = node.var;
    bn->nodes.insert(bn->nodes.begin(), loop_elem_sn);

    bn->nodes.push_back(this->update_loop_index_snode);
    wsn->body = bn;
    return bbn;
}


USemanticInfo Checker::visit_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    Entity entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Error subscript of something that is not an object!");
    }
    Entity class_entity = this->scope->get(entity_parent.object_value->ot->id);
    if (class_entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("Error this should be a CLASS, but it's not!");
    }
    Class* cls = class_entity.clazz;

    if (cls->type_params.size() != 0) {
        cls = instantiate_generic(cls, *entity_parent.object_value->ot);
    }

    auto subscript_it = cls->methods.find("__sub__");
    if (subscript_it == cls->methods.end()) {
        throw std::runtime_error("Error class " + cls->class_name + " does not define the __sub__ operator!");
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    // SemanticInfo& parent = *parent_p;
    // if (parent.type().kind != Kind::OBJECT) {
    //     this->error_reporter.subscript_non_object(node.start);
    //     return error_stub();
    // }
    // const ObjectType& object_type = parent.type().object();

    // if (this->is_lvalue && object_type == T_STRING) {
    //     this->error_reporter.string_immutable(node.start);
    //     return error_stub();
    // }

    VectorOfTypes children;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch(c);
    Entity child_entity = ct->entity;
    if (child_entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Error using something that's not an object as a subscript!");
    }
    if (*child_entity.object_value->ot != *subscript_fun->ft->param_types[0]) {
        throw std::runtime_error(
                "Error subscript type is " + child_entity.object_value->ot->to_string() + " but should be " +
                subscript_fun->ft->param_types[0]->to_string());
    }
    this->is_lvalue = old_lvalue;
    SemanticInfo info;
    info.entity = Entity(new ObjectValue());
    info.entity.object_value->ot = (ObjectType*) rtype;
    CallSNode* csn = new CallSNode();
    IdSNode* fsn = new IdSNode();
    fsn->identifier = sub_fun_path;
    csn->function = fsn;
    csn->arguments.push_back(parent_p->snode);
    csn->arguments.push_back(ct->snode);
    info.snode = csn;
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::visit_ternary(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto& expression_type = *expression_info.entity.object_value->ot;

    if (expression_type.id != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    // TypeNode& type = *expression_type.type_params[0];
    // semanticInfo.set_type(type);
    this->enter_scope("true_case");
    // this->scope->set("it", type);
    USemanticInfo true_case_p = this->dispatch(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch(node.false_case);
    SemanticInfo& false_case = *false_case_p;
    if (*false_case.entity.object_value->ot != *true_case.entity.object_value->ot) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.entity.object_value->ot->to_string() +
                " != " + false_case.entity.object_value->ot->to_string());
    } else {
        // semanticInfo.set_type(true_case.type());
    }
    return std::make_unique<SemanticInfo>(semanticInfo);
}


USemanticInfo Checker::visit_enum(EnumNode& node) {
    SemanticInfo info;
    EnumSNode* esn = new EnumSNode();
    Enum* enumm = this->scope->get(node.id).enumm;
    esn->id = enumm->path.as_str();
    esn->values = node.values;
    info.snode = esn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_class(ClassNode& node) {
    this->error_reporter.current_class = node.class_name;
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    this->current_class = node.class_name;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;
    inits = std::map<std::string, bool>();

    ClassSNode* csn = new ClassSNode();

    Class* clazz = this->scope->get(node.class_name).clazz;
    csn->identifier = clazz->path.as_str();
    sn->nodes.push_back(csn);
    sn->nodes.push_back(make_class_default_init(clazz->path.as_str(), node.members_ordered));

    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        inits[mt] = false;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
        csn->members.push_back(mt);
    }

    for (auto sm: node.static_members) {
        USemanticInfo sm_exp_info = this->dispatch(sm.second.second);
        if (*sm.second.first != *sm_exp_info->entity.object_value->ot) {
            throw std::runtime_error("Err: cannt initialize static member of type " + sm.second.first->to_string() +
                                     " with expression of type " + sm_exp_info->entity.object_value->ot->to_string());
        }
        if (!sm_exp_info->is_constant) {
            throw std::runtime_error("Error: cannot initialize static member with non constant expression!");
        }
    }

    std::vector<SNode*> methods_snodes;
    std::vector<SNode*> static_methods_snodes;

    this->this_type = new ObjectType(node.class_name, tp);
    for (auto method: node.methods) {
        this->is_method = true;
        this->add_this = true;
        this->this_entity = Entity(new ObjectValue());
        this->this_entity.object_value->ot = new ObjectType(node.class_name);
        this->this_entity.object_value->ot->actual_base_path = clazz->path;
        // method.second->path = clazz->path + "." + method.second->identifier;
        USemanticInfo method_info = this->visit_function(*method.second);
        methods_snodes.push_back(method_info->snode);
    }

    for (auto method: node.static_methods) {
        this->is_method = false;
        this->add_this = false;
        USemanticInfo method_info = this->visit_function(*method.second);
        static_methods_snodes.push_back(method_info->snode);
    }

    for (auto m: methods_snodes) {
        sn->nodes.push_back(m);
    }
    for (auto m: static_methods_snodes) {
        sn->nodes.push_back(m);
    }

    this->add_this = true;
    std::string eq_method_name = "eq";

    this->is_method = false;
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->error_reporter.current_class = "";
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::visit_root(BlockNode& node) {
    this->error_reporter.__file__ = this->__file__;
    this->error_reporter.code_lines = code_lines;

    // Initialize module level Scope
    for (auto f: this->module->flirpins) {
        this->scope->set(f.first, map_flirpin_to_entity(f.second));
    }
    // for (auto i: this->module->imports) {
    //     this->scope->set(i.first, i.second);
    // }

    USemanticInfo info = this->visit_block(node);
    this->root_snode = static_cast<BlockSNode*>(info->snode);
    SemanticInfo f;
    return std::make_unique<SemanticInfo>(f);
}

USemanticInfo Checker::visit_block(BlockNode& node) {
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    VectorOfNodes vn;
    for (auto& n: node.nodes) {
        USemanticInfo sinfo_p = this->dispatch(n);

        // sn->nodes.push_back(sinfo_p->snode);

        if (n->ntype == NodeType::BLOCK) {
            for (auto bnode: n->block().nodes) {
                vn.push_back(bnode);
            }
        } else {
            vn.push_back(n);
            if (sinfo_p->snode != nullptr) {
                if (sinfo_p->snode->type == SNodeType::BLOCK) {
                    for (auto nn : ((BlockSNode*) sinfo_p->snode)->nodes) {
                        sn->nodes.push_back(nn);
                    }
                } else {
                    sn->nodes.push_back(sinfo_p->snode);
                }
            }
        }
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value

            if (!sinfo.is_error && *get_entity_type(sinfo.entity) != T_NONE) {
                this->error_reporter.unused_return_value(n->start);
            }
        }
    }
    node.nodes = vn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_function(FunctionNode& n) {
    this->error_reporter.current_function = n.identifier;
    SemanticInfo info;
    FunctionSNode* sn = new FunctionSNode();
    info.snode = sn;
    Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    sn->identifier = n.path.as_str();
    sn->params = n.parameter_names;
    this->current_function = function_name;
    this->enter_scope(function_name);
    this->scope->is_function = true;
    if (this->add_this) {
        this->scope->set("this", this->this_entity);
        sn->params.insert(sn->params.begin(), "this");
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        TypeNode* cl = type.clone();
        make_not_generic(cl);
        this->scope->set(n.parameter_names[i], entity_from_type(*cl));
        // if (!param_type.is_generic()) {
        //     if (param_type.kind == Kind::OBJECT) {
        //         ObjectType& o_type = param_type.object();
        //         Entity pt = this->scope->get(o_type.id);
        //         o_type.actual_base_path = pt.clazz->path;
        //         if (type.kind == Kind::OBJECT) {
        //             std::cout << "START" << std::endl;
        //             this->assert_type_exists(type, n.start);
        //             std::cout << "END" << std::endl;
        //         }
        //         this->scope->set(n.parameter_names[i], entity_from_type(*cl));
        //     } else {
        //         this->scope->set(n.parameter_names[i], entity_from_type(type));
        //     }
        // } else {
        //     this->scope->set(n.parameter_names[i], entity_from_type(param_type));
        // }
    }
    std::cout << "FINISH " << std::endl;

    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    this->scope->set("__return__", entity_from_type(returnType));
    USemanticInfo body_info = this->visit_block(*n.body);
    sn->body = static_cast<BlockSNode*>(body_info->snode);

    if (returnType != T_NONE) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes.back();
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.function_return_last_stmt(function_name, returnType, last_node->start);
            }
        } else {
            this->error_reporter.function_return_last_stmt(function_name, returnType, n.start);
        }
    }
    this->leave_scope();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_id(IdNode& n) {
    Logger::info("Checking id node " + n._id);
    SemanticInfo info;
    IdSNode* sn = new IdSNode();
    info.snode = sn;
    Entity entity = this->scope->get(n._id);
    if (entity.type == E_TYPE::NOT_FOUND) {
        this->error_reporter.variable_not_declared(n._id, n.start);
        this->scope->set(n._id, Entity(E_TYPE::ERROR));
        return error_stub();
    }
    if (entity.type == E_TYPE::CONST_FUNCTION) {
        sn->identifier = entity.const_function->path.as_str();
    } else {
        sn->identifier = n._id;
    }
    info.entity = entity;
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::member_tuple(const ObjectType& final_type, MemberNode& n) {
    if (n.type != MemberType::NUM) {
        throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                 std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
    }
    if (n.n_child < 1 || n.n_child > final_type.type_params.size()) {
        throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                 std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
    }
    SemanticInfo s;
    // s.set_type(*final_type.type_params[n.n_child - 1]);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_cast(CastNode& n) {
    USemanticInfo exp_info = this->dispatch(n.exp);
    SemanticInfo info;
    ObjectType cast_type(n.as_type, {});
    const TypeNode& exp_type = *exp_info->entity.object_value->ot;
    if (exp_type == T_INT || exp_type == T_FLOAT || exp_type == T_DOUBLE || exp_type == T_BOOL) {
        if (cast_type != T_BOOL && cast_type != T_FLOAT && cast_type != T_DOUBLE && cast_type != T_INT) {
            throw std::runtime_error("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
        }
        if (exp_type == cast_type) {
            throw std::runtime_error("Casting to same type " + cast_type.to_string());
        }
    } else {
        throw std::runtime_error("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
    }
    n.exp_type = exp_type.clone();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_if(IfNode& n) {
    SemanticInfo info;
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    if (condition_info.entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("If condition should be a Boolean");
    }
    if (*condition_info.entity.object_value->ot != T_BOOL) {
        throw std::runtime_error("If condition should be a Boolean");
    }

    // std::unordered_map<std::string, bool> not_null_vars;
    // if (condition_info.type() == T_NONE) {
    //     this->error_reporter.function_doesnt_return_a_value(n.condition->start, new T_BOOL);
    // } else if (condition_info.type() != T_BOOL) {
    //     this->error_reporter.condition(condition_info.type(), n.start, "if");
    // }

    this->enter_scope("if");
    USemanticInfo body_info = this->visit_block(*n.then);
    // for (auto v: this->scope->table) {
    //     n.then->local_vars.push_back(std::make_pair(v.first, v.second->clone()));
    // }
    this->leave_scope();

    std::vector<std::pair<SNode*, BlockSNode*>> elifs;

    for (size_t i = 0; i < n.elifs.size(); i++) {
        USemanticInfo elif_condition_info_p = this->dispatch(n.elifs[i].first);
        SemanticInfo& elif_condition_info = *elif_condition_info_p;
        if (elif_condition_info.entity.type != E_TYPE::OBJECT_VALUE) {
            throw std::runtime_error("If condition should be a Boolean");
        }
        if (*elif_condition_info.entity.object_value->ot != T_BOOL) {
            throw std::runtime_error("If condition should be a Boolean");
        }
        this->enter_scope("elif");
        USemanticInfo elif_block_info = this->visit_block(*n.elifs[i].second);
        this->leave_scope();
        elifs.push_back(std::make_pair(elif_condition_info.snode, (BlockSNode*) elif_block_info->snode));
    }
    USemanticInfo else_info;
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        else_info = this->visit_block(*n.selse);
        this->leave_scope();
    }
    SNode* else_snode = else_info == nullptr ? nullptr : else_info->snode;
    info.snode = make_if_snode(condition_info.snode, body_info->snode, elifs, else_snode);

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_boolop(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);
    SemanticInfo info;
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::OBJECT_VALUE || right_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }

    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        this->error_reporter.binop(ltype, rtype, n.start);
        return error_stub();
    }

    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        return error_stub();
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;


    if (left_info.is_error || right_info.is_error) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    std::string fun = map_boolop_to_method_name(n.op);

    Entity entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS && entity.type != E_TYPE::ENUM) {
        throw std::runtime_error("This should be a CLASS/ENUM, but it's not!");
    }
    if (entity.type == E_TYPE::ENUM) {
        if (fun != "eq" && fun != "ne") {
            throw std::runtime_error("Error: enum type doesnt support this operator");
        }
        ObjectType* ot = new ObjectType("Boolean", {});
        ot->actual_base_path = Path("core.Boolean");
        TypeNode* rettype = ot;
        info.entity = Entity(new ObjectValue());
        info.entity.object_value->ot = (ObjectType*) rettype;

        ConstFunction* opfun = entity.enumm->functions[fun];

        info.snode = make_boolop_snode(opfun, left_info, right_info);

        // IdSNode* function_id = new IdSNode(opfun->path.as_str());
        // CallSNode* sn = new CallSNode();
        // sn->function = function_id;
        // sn->arguments = {left_info.snode, right_info.snode};
        //
        // info.snode = sn;
    } else {
        Class* cls = entity.clazz;
        auto operator_fun_it = cls->static_methods.find(fun);
        if (operator_fun_it == cls->static_methods.end()) {
            throw std::runtime_error("Class " + cls->class_name + " has no operator " + fun + " defined ");
        }

        ConstFunction* operator_fun = operator_fun_it->second;
        TypeNode* rettype = operator_fun->ft->return_type->clone();
        info.entity = Entity(new ObjectValue());
        info.entity.object_value->ot = (ObjectType*) rettype;

        info.snode = make_boolop_snode(operator_fun, left_info, right_info);
    }

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_binop(BinopNode& n) {

    CallSNode* sn = new CallSNode();
    IdSNode* function_id = new IdSNode();
    sn->function = function_id;
    function_id->identifier = "";//mangle_path(this->module->imported_paths.at("Integer.add"));
    SemanticInfo info;
    info.snode = sn;

    Logger::info("Checking binop node");
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::OBJECT_VALUE || right_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }
    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        this->error_reporter.binop(ltype, rtype, n.start);
        return error_stub();
        // throw std::runtime_error("Binary operation between values of different types: " + ltype.to_string() + " and " +
        //                          rtype.to_string());
    }
    bool err = false;
    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        err = true;
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        err = true;
    }
    if (err) {
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    sn->arguments.push_back(left_info.snode);
    sn->arguments.push_back(right_info.snode);

    if (left_info.is_error || right_info.is_error) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    TypeNode* rettype;
    std::string fun = binoptype_to_str(n.op);

    Entity entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("This should be a CLASS, but it's not!");
    }
    Class* cls = entity.clazz;
    auto operator_fun_it = cls->static_methods.find(fun);
    if (operator_fun_it == cls->static_methods.end()) {
        throw std::runtime_error("Class " + cls->class_name + " has no operator " + fun + " defined ");
    }
    ConstFunction* operator_fun = operator_fun_it->second;
    function_id->identifier = operator_fun->path.as_str();
    rettype = operator_fun->ft->return_type->clone();
    // n.ltype = left.clone();
    info.entity = Entity(new ObjectValue());
    info.entity.object_value->ot = (ObjectType*) rettype;

    return std::make_unique<SemanticInfo>(info);
}


