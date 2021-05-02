//
// Created by chris on 2/5/21.
//

#include "CheckStatements.h"

USemanticInfo Checker::visit_assignment(AssignmentNode& n) {

    SemanticInfo info;

    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            USemanticInfo rv = this->dispatch(n.rvalue);
            info.snode = rv->snode;
            return std::make_unique<SemanticInfo>(info);
        }
    }

    this->is_lvalue = true;
    USemanticInfo linfo_p = this->dispatch(n.lvalue);
    this->is_lvalue = false;

    if (linfo_p->is_error) {
        return nullptr;
    }

    if (linfo_p->entity.type != E_TYPE::FUNCTION_VALUE && linfo_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Cannot assign to this thing!");
    }

    if (n.lvalue->ntype == NodeType::MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_reporter.tuple_assign(n.start);
    }

    USemanticInfo expression_info_p = this->dispatch(n.rvalue);

    if (linfo_p->entity.type == E_TYPE::OBJECT_VALUE && expression_info_p->entity.type == E_TYPE::CONST_FUNCTION) {
        this->error_reporter.assignment(*linfo_p->entity.function_value->ft,
                                        *expression_info_p->entity.object_value->ot,
                                        n.rvalue->start);
    }
    TypeNode* exp_type = expression_info_p->entity.object_value->ot;

    if (exp_type->kind == Kind::OBJECT && this->module->aliased_types.count(exp_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
    } else {
        this->module->fill_actual(exp_type);
    }

    if (expression_info_p->is_error) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;

    const TypeNode& l_type = *linfo.entity.object_value->ot;

    if (linfo.entity.type == E_TYPE::OBJECT_VALUE && expression_info_p->entity.type == E_TYPE::OBJECT_VALUE) {

        std::string lpath_as_str = linfo.entity.object_value->ot->actual_base_path.as_str();
        std::string exppath_as_str = expression_info_p->entity.object_value->ot->actual_base_path.as_str();

        if (lpath_as_str != exppath_as_str) {
            if (l_type.kind == Kind::OBJECT) {
                const ObjectType& actual_type = l_type.object();
                if (actual_type.id == "Option") {
                    // if type doesn't match exactly, we may be assigning to an Option[t]
                    if (*actual_type.type_params[0] != *exp_type) {
                        auto& foo = exp_type->object();
                        if (foo.id != "NoneType") {
                            this->error_reporter.assignment(l_type, *exp_type, n.start);
                        }
                    }
                } else {
                    if (actual_type.kind == Kind::OBJECT && actual_type.object().id == "Union") {
                        int type_index = target_union_type(actual_type.object(), *exp_type);
                        if (type_index == -1) {
                            this->error_reporter.assignment(actual_type, *exp_type, n.rvalue->start);
                        } else {
                            expression_info_p->snode = make_union_wrapper(type_index, expression_info_p->snode);
                        }
                    } else {
                        // if it's not Option[t], then it's an error
                        this->error_reporter.assignment(l_type, *exp_type, n.start);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_reporter.assignment(l_type, *exp_type, n.start);
            }
        }
        // else, type matches don't do anything
        n.type = l_type.clone();
    } else {
        this->error_reporter.assignment(*linfo_p->entity.function_value->ft,
                                        *expression_info_p->entity.object_value->ot,
                                        n.rvalue->start);
    }
    AssignmentSNode* sn = new AssignmentSNode();
    sn->lvalue = linfo_p->snode;
    sn->rvalue = expression_info_p->snode;

    info.snode = sn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_return(ReturnNode& n) {
    SemanticInfo info;
    ReturnSNode* sn = new ReturnSNode();
    info.snode = sn;
    Entity return_entity = this->scope->get("__return__");
    TypeNode* return_typet = nullptr;
    if (return_entity.type == E_TYPE::FUNCTION_VALUE) {
        return_typet = return_entity.function_value->ft;
    } else if (return_entity.type == E_TYPE::OBJECT_VALUE) {
        return_typet = return_entity.object_value->ot;
    }
    TypeNode* return_type = return_typet;
    if (return_type->kind == Kind::OBJECT && this->module->aliased_types.count(return_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(return_type->object().id);
        return_type = aliased_type;
    } else {
        this->module->fill_actual(return_type);
    }
    if (*return_type == T_NONE) {
        if (n.expression != nullptr) {
            this->error_reporter.bad_return(n.start);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_reporter.no_return(*return_type, n.start);
    }
    USemanticInfo expression_info_p = this->dispatch(n.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.is_error) {
        return nullptr;
    }
    sn->expression = expression_info.snode;
    if (return_type->object().id == "Union") {
        int type_index = target_union_type(return_type->object(), *expression_info.entity.object_value->ot);
        if (type_index == -1) {
            this->error_reporter.assignment(*return_typet, *expression_info.entity.object_value->ot, n.start);
        }
        SNode* union_wrapper = make_union_wrapper(type_index, sn->expression);
        sn->expression = union_wrapper;
    } else {
        if (!this->can_assign(*expression_info.entity.object_value->ot, *return_type)) {
            this->error_reporter.return_mismatch(*return_typet, *expression_info.entity.object_value->ot, n.start);
            return error_stub();
        }
    }
    n.ret_type = return_type->clone();
    n.reachables = this->scope->get_all();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_match(MatchExpressionNode* node) {
    SemanticInfo info;
    USemanticInfo exp_info = this->dispatch(node->exp);
    if (exp_info->entity.type != E_TYPE::OBJECT_VALUE) {
        this->error_reporter.match_type(exp_info->entity, TextPosition());
        return error_stub();
    }

    ObjectType* ot = exp_info->entity.object_value->ot;
    if (ot->aliased_type != nullptr) {
        ot = (ObjectType*) ot->aliased_type;
    }
    if (ot->id != "Union") {
        this->error_reporter.match_type(exp_info->entity, TextPosition());
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
            throw std::runtime_error("Error, type " + case_type->to_string() + " not part of " + ot->to_string());
        }
        this->enter_scope("case");
        this->scope->set(case_id, entity_from_type(*case_type));
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
    USemanticInfo exp_info_p = this->dispatch(node.exp);
    if (exp_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        this->error_reporter._for(exp_info_p->entity, node.exp->start);
    }
    ObjectType* exp_ot = exp_info_p->entity.object_value->ot;
    if (exp_ot->id != "List") {
        this->error_reporter._for(exp_info_p->entity, node.exp->start);
    }

    TypeNode* elem_type = exp_ot->type_params[0];
    Entity elem_entity = entity_from_type(*elem_type);
    this->enter_scope("for");
    this->scope->set(node.var, elem_entity);


    this->loop_list_var_id = "__loop_list__";
    this->loop_index_var_id = "__loop_index__";
    this->loop_list_len_var_id = "__loop_list_len__";

    AssignmentSNode* increment_index_sn = new AssignmentSNode();
    this->update_loop_index_snode = increment_index_sn;
    increment_index_sn->lvalue = new IdSNode(this->loop_index_var_id);
    CallSNode* inc_exp_node = new CallSNode();
    inc_exp_node->function = new IdSNode("core.Integer.add");
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
    USemanticInfo condition_p = this->dispatch(node.condition);
    SemanticInfo& condition = *condition_p;
    if (condition.entity.type != E_TYPE::OBJECT_VALUE) {
        this->error_reporter.condition(condition.entity, node.start, "while");
    }
    ObjectType* condition_ot = condition.entity.object_value->ot;
    if (*condition_ot != T_BOOL) {
        this->error_reporter.condition(condition.entity, node.start, "while");
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
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    if (condition_info.entity.type != E_TYPE::OBJECT_VALUE) {
        this->error_reporter.condition(condition_info.entity, n.condition->start, "if");
    }
    if (*condition_info.entity.object_value->ot != T_BOOL) {
        this->error_reporter.condition(condition_info.entity, n.condition->start, "if");
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
            this->error_reporter.condition(condition_info.entity, n.condition->start, "elif");
        }
        if (*elif_condition_info.entity.object_value->ot != T_BOOL) {
            this->error_reporter.condition(condition_info.entity, n.condition->start, "elif");
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
