//
// Created by chris on 2/5/21.
//

#include "CheckStatements.h"

USemanticInfo Checker::visit_lvalue_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch_rvalue(node.parent);
    Entity entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::VALUE || entity_parent.value->type->kind == Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return error_stub();
    }
    if (entity_parent.value->metatype == Meta::ENUM) {
        this->error_reporter.fail("Error: no subscript in enum");
        return error_stub();
    }
    Class* cls = entity_parent.value->clazz;
    assert(cls != nullptr);
    if (cls->type_params.size() != 0) {
        cls = instantiate_generic(cls, entity_parent.value->type->object());
    }

    auto subscript_it = cls->methods.find("__set_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.object_no_special_method(*entity_parent.value->type, "__set_item__", node);
        return error_stub();
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
    }
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch_rvalue(c);
    Entity child_entity = ct->entity;
    if (child_entity.type != E_TYPE::VALUE) {
        this->error_reporter.error_type_mismatch(*subscript_fun->ft->param_types[0], *node.child[0], child_entity);
        return error_stub();
    }
    if (*child_entity.value->type != *subscript_fun->ft->param_types[0]) {
        this->error_reporter.error_type_mismatch(*subscript_fun->ft->param_types[0], *node.child[0], child_entity);

        // this->error_reporter.subscript_type(*child_entity.value->type, *subscript_fun->ft->param_types[0], node);
        return error_stub();
        // this->error_reporter.fail(
        //         "Error subscript type is " + child_entity.value->type->to_string() + " but should be " +
        //         subscript_fun->ft->param_types[0]->to_string());
    }
    SemanticInfo info;
    info.entity = Entity(new Value((ObjectType*) rtype));

    this->fill_value(info.entity.value);
    CallSNode* csn = new CallSNode();
    IdSNode* fsn = new IdSNode();
    fsn->identifier = sub_fun_path;
    csn->function = fsn;
    csn->arguments.push_back(parent_p->snode);
    csn->arguments.push_back(ct->snode);
    info.snode = csn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_assignment(AssignmentNode& n) {

    SemanticInfo info;

    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            USemanticInfo rv = this->dispatch_rvalue(n.rvalue);
            info.snode = rv->snode;
            return std::make_unique<SemanticInfo>(info);
        }
    }

    USemanticInfo linfo_p;
    bool is_subscript = false;
    CallSNode* csn = nullptr;
    if (n.lvalue->ntype == NodeType::SUB) {
        // special case
        linfo_p = this->visit_lvalue_subscript((SubscriptNode&) *n.lvalue);
        csn = (CallSNode*) linfo_p->snode;
        is_subscript = true;
    } else {
        linfo_p = this->dispatch(n.lvalue);
    }

    USemanticInfo expression_info_p = this->dispatch_rvalue(n.rvalue);

    if (linfo_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (expression_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }

    if (linfo_p->entity.type != E_TYPE::VALUE) {
        this->error_reporter.cant_assign(*n.lvalue);
        // this->error_reporter.fail("Cannot assign to this thing!");
        return error_stub();
    }

    if (linfo_p->entity.value->type->kind == Kind::OBJECT) {
        bool ff = n.lvalue->ntype == NodeType::MEMBER;
        if (linfo_p->is_tuple_member) {
            this->error_reporter.cant_assign(*n.lvalue);
            return error_stub();
        }
    }


    if (linfo_p->entity.type == E_TYPE::VALUE && linfo_p->entity.value->type->kind == Kind::OBJECT &&
        expression_info_p->entity.type == E_TYPE::CONST_FUNCTION) {
        // this->error_reporter.error_type_mismatch(*n.type, *n.rvalue, *expression_info_p_info_p->entity.value->type);
        this->error_reporter.assignment(*linfo_p->entity.value->type,
                                        *expression_info_p->entity.value->type,
                                        n.rvalue->start,
                                        *n.lvalue,
                                        *n.rvalue);
    }
    TypeNode* exp_type = expression_info_p->entity.value->type;

    if (exp_type->kind == Kind::OBJECT && this->module->aliased_types.count(exp_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
    } else {
        // if (exp_type->kind == Kind::OBJECT && exp_type->object().id.size() != 1) {
        //     this->module->fill_actual(exp_type);
        // }
    }

    if (expression_info_p->entity.type == E_TYPE::ERROR) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;

    const TypeNode& l_type = *linfo.entity.value->type;

    if (linfo.entity.type == E_TYPE::VALUE && expression_info_p->entity.type == E_TYPE::VALUE) {
        SNode* rvalue_snode = this->make_rvalue(expression_info_p->entity,
                                                expression_info_p->snode,
                                                *linfo.entity.value->type);
        if (rvalue_snode == nullptr) {
            // this->error_reporter.assignment(l_type, *exp_type, n.start, *n.lvalue, *n.rvalue);
            this->error_reporter.error_type_mismatch(l_type, *n.rvalue, expression_info_p->entity);
            return error_stub();
        }
        expression_info_p->snode = rvalue_snode;
    }
    // } else {
    //     this->error_reporter.assignment(*linfo_p->entity.function_value->ft,
    //                                     *expression_info_p->entity.value->type,
    //                                     n.rvalue->start);
    // }
    if (is_subscript) {
        info.snode = linfo_p->snode;
        csn->arguments.push_back(expression_info_p->snode);
    } else {
        AssignmentSNode* sn = new AssignmentSNode();
        sn->lvalue = linfo_p->snode;
        sn->rvalue = expression_info_p->snode;
        info.snode = sn;
    }

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_return(ReturnNode& n) {
    SemanticInfo info;
    ReturnSNode* sn = new ReturnSNode();
    info.snode = sn;
    Entity return_entity = this->scope->get("__return__");
    if (return_entity.type == E_TYPE::NOTHING) {
        if (n.expression != nullptr) {
            this->error_reporter.bad_return(n.start);
        }
        SemanticInfo info_r;
        info_r.snode = new ReturnSNode();
        return std::make_unique<SemanticInfo>(info_r);
    }
    TypeNode* return_typet = return_entity.value->type;
    TypeNode* return_type = return_typet;
    if (return_type->kind == Kind::OBJECT && this->module->aliased_types.count(return_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(return_type->object().id);
        return_type = aliased_type;
    } else {
        this->module->fill_actual(return_type);
    }
    if (n.expression == nullptr) {
        this->error_reporter.no_return(*return_type, n.start);
        return error_stub();
    }

    USemanticInfo expression_info_p = this->dispatch_rvalue(n.expression);
    if (expression_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    SNode* exp_snode = make_rvalue(expression_info_p->entity, expression_info_p->snode, *return_type);
    if (exp_snode == nullptr) {
        this->error_reporter.error_type_mismatch(*return_type, *n.expression, expression_info_p->entity);
        return error_stub();
    }
    sn->expression = exp_snode;
    // SemanticInfo& expression_info = *expression_info_p;
    // if (expression_info.entity.type == E_TYPE::ERROR) {
    //     return error_stub();
    // }
    // sn->expression = expression_info.snode;
    // if (return_type->object().id == "Union") {
    //     int type_index = target_union_type(return_type->object(), *expression_info.entity.value->type);
    //     if (type_index == -1) {
    //         this->error_reporter.assignment(*return_typet, *expression_info.entity.value->type, n.start);
    //     }
    //     SNode* union_wrapper = make_union_wrapper(type_index, sn->expression);
    //     sn->expression = union_wrapper;
    // } else {
    //     if (!this->can_assign(*expression_info.entity.value->type, *return_type)) {
    //         this->error_reporter.return_mismatch(*return_typet, *expression_info.entity.value->type, n.start);
    //         return error_stub();
    //     }
    // }
    n.ret_type = return_type->clone();
    n.reachables = this->scope->get_all();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_match(MatchExpressionNode* node) {
    SemanticInfo info;
    USemanticInfo exp_info = this->dispatch_rvalue(node->exp);
    if (exp_info->entity.type != E_TYPE::VALUE || exp_info->entity.value->type->kind != Kind::OBJECT) {
        this->error_reporter.error_type_mismatch(ObjectType("Union", {new ObjectType("...", {})}),
                                                 *node->exp,
                                                 exp_info->entity);
        return error_stub();
    }

    ObjectType* ot = &exp_info->entity.value->type->object();
    if (ot->aliased_type != nullptr) {
        ot = (ObjectType*) ot->aliased_type;
    }
    if (ot->id != "Union") {
        this->error_reporter.error_type_mismatch(ObjectType("Union", {new ObjectType("...", {})}),
                                                 *node->exp,
                                                 exp_info->entity);
        return error_stub();
    }
    std::vector<std::pair<int, BlockSNode*>> cas;
    std::string varname = "match_var";
    for (size_t i = 0; i < node->ids.size(); i++) {
        std::string case_id = node->ids[i];
        std::pair<TypeNode*, BlockNode*> c = node->cases[i];
        TypeNode* case_type = c.first;
        BlockNode* case_node = c.second;

        this->module->fill_actual(case_type);

        int union_index = target_union_type(*ot, *case_type);
        if (union_index == -1) {
            this->error_reporter.fail("Error, type " + case_type->to_string() + " not part of " + ot->to_string());
            return error_stub();
        }
        this->enter_scope("case");
        Entity ent(new Value(case_type));
        this->fill_value(ent.value);
        assert(ent.value->clazz != nullptr);
        this->scope->set(case_id, ent);
        USemanticInfo case_info = this->dispatch(case_node);
        BlockSNode* bn = (BlockSNode*) case_info->snode;
        DeclarationSNode* dn = new DeclarationSNode();
        dn->identifier = case_id;
        ObjectMemberSNode* omn = new ObjectMemberSNode();
        dn->expression = omn;
        omn->member_name = "o";
        omn->object = new IdSNode(varname);
        omn->class_path = Path("core.Union");
        bn->nodes.insert(bn->nodes.begin(), dn);
        cas.push_back(std::make_pair(union_index, (BlockSNode*) case_info->snode));
        this->leave_scope();
    }
    DeclarationSNode* init = new DeclarationSNode();
    init->expression = exp_info->snode;
    init->identifier = varname;
    MatchSNode* mn = new MatchSNode(init, varname, cas);
    info.snode = mn;

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_continue(ContinueNode& node) {
    SemanticInfo info;
    BlockSNode* bn = new BlockSNode();
    info.snode = bn;
    if (this->update_loop_index_snode != nullptr) {
        bn->nodes.push_back(this->update_loop_index_snode);
    }
    bn->nodes.push_back(new ContinueSNode());
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_for(ForNode& node) {
    USemanticInfo exp_info_p = this->dispatch_rvalue(node.exp);
    if (exp_info_p->entity.type != E_TYPE::VALUE) {
        this->error_reporter._for(exp_info_p->entity, node.exp->start);
    }
    ObjectType* exp_ot = &exp_info_p->entity.value->type->object();
    if (exp_ot->id != "List") {
        this->error_reporter._for(exp_info_p->entity, node.exp->start);
    }

    TypeNode* elem_type = exp_ot->type_params[0];
    Entity elem_entity(new Value(elem_type));
    this->fill_value(elem_entity.value);
    this->enter_scope("for");
    this->scope->set(node.var, elem_entity);


    std::string loop_c = std::to_string(this->loop_count++);
    this->loop_list_var_id = "__loop_list__" + loop_c;
    this->loop_index_var_id = "__loop_index__" + loop_c;
    this->loop_list_len_var_id = "__loop_list_len__" + loop_c;

    AssignmentSNode* increment_index_sn = new AssignmentSNode();
    this->update_loop_index_snode = increment_index_sn;
    increment_index_sn->lvalue = new IdSNode(this->loop_index_var_id);
    CallSNode* inc_exp_node = new CallSNode();
    inc_exp_node->function = new IdSNode("core.Integer.__add__");
    inc_exp_node->arguments.push_back(new IdSNode(this->loop_index_var_id));
    IntegerSNode* one_node = new IntegerSNode();
    one_node->str = "1";
    inc_exp_node->arguments.push_back(one_node);
    increment_index_sn->rvalue = inc_exp_node;


    USemanticInfo binfo = this->visit_block(*node.body);
    this->leave_scope();

    SemanticInfo rinfo;


    rinfo.snode = make_for_snode(node, binfo, exp_info_p);
    this->update_loop_index_snode = nullptr;
    return std::make_unique<SemanticInfo>(rinfo);
}

USemanticInfo Checker::visit_break(BreakNode& node) {
    // node.loop_vars = this->scope->get_all_in_loop();
    SemanticInfo info;
    info.snode = new BreakSNode();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_while(WhileNode& node) {
    WhileSNode* while_sn = new WhileSNode();
    SemanticInfo info;
    info.snode = while_sn;
    USemanticInfo condition_p = this->dispatch_rvalue(node.condition);
    SemanticInfo& condition = *condition_p;
    if (condition.entity.type != E_TYPE::VALUE) {
        this->error_reporter.error_type_mismatch(T_BOOL, *node.condition, condition_p->entity);
        // return error_stub();
        // this->error_reporter.condition(condition.entity, node.start, "while");
    }
    ObjectType* condition_ot = &condition.entity.value->type->object();
    if (*condition_ot != T_BOOL) {
        this->error_reporter.error_type_mismatch(T_BOOL, *node.condition, condition_p->entity);

        // this->error_reporter.condition(condition.entity, node.start, "while");
    }
    this->enter_scope("while");
    this->scope->is_loop = true;
    USemanticInfo body_info_p = this->visit_block(*node.body);
    this->scope->is_loop = false;
    for (auto v: this->scope->table) {
        // if (v.second->type == E_TYPE::OBJECT_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((ObjectValue*) v.second)->ot));
        // }
        // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((FunctionValue*) v.second)->ft));
        // }
    }
    this->leave_scope();
    while_sn->condition = condition.snode;
    while_sn->body = (BlockSNode*) body_info_p->snode;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_if(IfNode& n) {
    SemanticInfo info;
    USemanticInfo condition_info_p = this->dispatch_rvalue(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    if (condition_info.entity.type != E_TYPE::ERROR) {
        if (condition_info.entity.type != E_TYPE::VALUE) {
            this->error_reporter.error_type_mismatch(T_BOOL, *n.condition, condition_info.entity);
            // this->error_reporter.condition(condition_info.entity, n.condition->start, "if");
        }
        if (*condition_info.entity.value->type != T_BOOL) {
            this->error_reporter.error_type_mismatch(T_BOOL, *n.condition, condition_info.entity);
            // this->error_reporter.condition(condition_info.entity, n.condition->start, "if");
        }
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
        USemanticInfo elif_condition_info_p = this->dispatch_rvalue(n.elifs[i].first);
        SemanticInfo& elif_condition_info = *elif_condition_info_p;
        if (elif_condition_info.entity.type != E_TYPE::VALUE) {
            this->error_reporter.error_type_mismatch(T_BOOL, *n.elifs[i].first, elif_condition_info.entity);
            // this->error_reporter.condition(condition_info.entity, n.elifs[i].first->start, "elif");
        }
        if (*elif_condition_info.entity.value->type != T_BOOL) {
            this->error_reporter.error_type_mismatch(T_BOOL, *n.elifs[i].first, elif_condition_info.entity);
            // this->error_reporter.condition(condition_info.entity, n.elifs[i].first->start, "elif");
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
