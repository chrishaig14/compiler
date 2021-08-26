//
// Created by chris on 2/5/21.
//

#include "CheckStatements.h"

#include "../ast/ObjectType.h"
#include "../simple_nodes/ThrowSNode.h"
#include "../simple_nodes/TryCatchSNode.h"
#include "errors/ErrorTypeMismatch.h"
#include "errors/ErrorCantAssign.h"
#include "errors/ErrorBadReturn.h"
#include "errors/ErrorObjectNoSpecialMethod.h"
#include "errors/ErrorFor.h"

USemanticInfo Checker::visit_lvalue_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch_rvalue(*node.parent);
    Entity& entity_parent = parent_p->entity.get();
    if (entity_parent.type != E_TYPE::VALUE || ((EntityValue&) entity_parent).value->type->kind == Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return error_stub();
    }
    auto& entity_parent_value = (EntityValue&) entity_parent;
    if (entity_parent_value.value->metatype == Meta::ENUM) {
        this->error_reporter.fail("Error: no subscript in enum");
        return error_stub();
    }
    Class* cls = entity_parent_value.value->clazz;
    assert(cls != nullptr);
    if (!cls->type_params.empty()) {
        cls = instantiate_generic(cls, entity_parent_value.value->type->object());
    }

    auto subscript_it = cls->methods.find("__set_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(ErrorObjectNoSpecialMethod(*entity_parent_value.value->type, "__set_item__", node));
        return error_stub();
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
    }
    USemanticInfo child_sinfo = this->expect_rvalue_of_type(*subscript_fun->ft->param_types[0], *node.child[0]);
    if (child_sinfo->is_error()) {
        return error_stub();
    }
    SNode* child_snode = child_sinfo->snode;

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    auto value = std::make_unique<Value>((ObjectType*) rtype);
    this->fill_value(*value);
    info.entity = *new EntityValue(std::move(value));

    auto* fsn = new IdSNode(sub_fun_path);
    auto* csn = new CallSNode(fsn, {parent_p->snode, child_snode});
    info.snode = csn;
    return info_u;
}

USemanticInfo Checker::visit_assignment(ast::Assignment& n) {
    if (n.lvalue->ntype == NodeType::ID) {
        if (((ast::Id&) *n.lvalue)._id == "_") {
            USemanticInfo rv = this->dispatch_rvalue(*n.rvalue);
            return rv;
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
        linfo_p = this->dispatch(*n.lvalue);
    }

    USemanticInfo expression_info_p = this->dispatch_rvalue(*n.rvalue);

    if (linfo_p->is_error()) {
        return error_stub();
    }
    if (expression_info_p->is_error()) {
        return error_stub();
    }

    if (linfo_p->entity.get().type != E_TYPE::VALUE) {
        this->error_reporter.error(ErrorCantAssign(*n.lvalue));
        // this->error_reporter.fail("Cannot assign to this thing!");
        return error_stub();
    }
    EntityValue& e_value = (EntityValue&) linfo_p->entity;

    if (e_value.value->type->kind == Kind::OBJECT) {
        // bool ff = n.lvalue->ntype == NodeType::MEMBER;
        if (linfo_p->is_tuple_member) {
            this->error_reporter.error(ErrorCantAssign(*n.lvalue));
            return error_stub();
        }
    }

    if (e_value.value->type->kind == Kind::OBJECT && expression_info_p->entity.get().type == E_TYPE::CONST_FUNCTION) {
        std::cerr << "assignment error" << std::endl;
        exit(111);
        // this->error_reporter.assignment(*e_value.value->type,
        //                                 *expression_info_p->entity.value->type,
        //                                 n.rvalue->start,
        //                                 *n.lvalue,
        //                                 *n.rvalue);
    }
    TypeNode* exp_type = ((EntityValue&) expression_info_p->entity).value->type;

    if (exp_type->kind == Kind::OBJECT && this->module.aliased_types.count(exp_type->object().id) == 1) {
        TypeNode* aliased_type = this->module.aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
    } else {
        // if (exp_type->kind == Kind::OBJECT && exp_type->object().id.size() != 1) {
        //     this->module.fill_actual(exp_type);
        // }
    }

    if (expression_info_p->is_error()) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;

    EntityValue& l_entity_value = (EntityValue&) linfo.entity;
    const TypeNode& l_type = *l_entity_value.value->type;

    if (l_entity_value.type == E_TYPE::VALUE && expression_info_p->entity.get().type == E_TYPE::VALUE) {
        SNode* rvalue_snode = this->make_rvalue(expression_info_p->entity,
                                                expression_info_p->snode,
                                                *l_entity_value.value->type);
        if (rvalue_snode == nullptr) {
            this->error_reporter.error(ErrorTypeMismatch(l_type, *n.rvalue, expression_info_p->entity));
            return error_stub();
        }
        expression_info_p->snode = rvalue_snode;
    }

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    if (is_subscript) {
        info.snode = linfo_p->snode;
        csn->arguments.push_back(expression_info_p->snode);
    } else {
        auto lu = USNode(linfo_p->snode);
        auto eu = USNode(expression_info_p->snode);
        info.snode = new AssignmentSNode(lu, eu);
    }

    return info_u;
}

USemanticInfo Checker::visit_return(ast::ReturnNode& n) {
    Entity& return_entity = this->scope->get("__return__");
    if (return_entity.type == E_TYPE::NOTHING) {
        if (n.expression != nullptr) {
            this->error_reporter.error(ErrorBadReturn(n.start));
        }
        SemanticInfo info_r;
        USNode u;
        info_r.snode = new ReturnSNode(u);
        return std::make_unique<SemanticInfo>(info_r);
    }
    TypeNode* return_type = ((EntityValue&) return_entity).value->type;
    if (return_type->kind == Kind::OBJECT && this->module.aliased_types.count(return_type->object().id) == 1) {
        TypeNode* aliased_type = this->module.aliased_types.at(return_type->object().id);
        return_type = aliased_type;
    } else {
        this->module.fill_actual(return_type);
    }
    if (n.expression == nullptr) {
        // this->error_reporter.no_return(*return_type, n.start);
        return error_stub();
    }

    USemanticInfo expression_info_p = this->expect_rvalue_of_type(*return_type, *n.expression);
    if (expression_info_p->is_error()) {
        return error_stub();
    }
    auto u = USNode(expression_info_p->snode);
    auto* sn = new ReturnSNode(u);
    for (auto l : this->scope->get_all()) {
        sn->reachables.push_back(l.first);
    }

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.snode = sn;
    return info_u;
}

// USemanticInfo Checker::visit_throw(ThrowNode& n) {
//     USemanticInfo expression_info_p = this->dispatch_rvalue(n.exp);
//     if (expression_info_p->is_error()) {
//         return error_stub();
//     }
//
//     auto* sn = new ThrowSNode(expression_info_p->snode);
//     for (auto l: this->scope->get_all()) {
//         sn->reachables.push_back(l.first);
//     }
//
//     USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
//     info.snode = sn;
//     return info_u;
// }

USemanticInfo Checker::visit_match(MatchExpressionNode& node) {
    USemanticInfo exp_info = this->dispatch_rvalue(*node.exp);
    if (exp_info->entity.get().type != E_TYPE::VALUE ||
        ((EntityValue&) exp_info->entity).value->type->kind != Kind::OBJECT) {

        this->error_reporter.error(ErrorTypeMismatch(*new ObjectType("Union", {new ObjectType("...", {})}),
                                                     *node.exp,
                                                     exp_info->entity));
        return error_stub();
    }

    ObjectType* ot = &((EntityValue&) exp_info->entity).value->type->object();
    if (ot->aliased_type != nullptr) {
        ot = (ObjectType*) ot->aliased_type;
    }
    if (ot->id != "Union") {
        this->error_reporter.error(ErrorTypeMismatch(*new ObjectType("Union", {new ObjectType("...", {})}),
                                                     *node.exp,
                                                     exp_info->entity));
        return error_stub();
    }
    std::vector<std::pair<int, BlockSNode*>> cas;
    std::string varname = "match_var";
    for (size_t i = 0; i < node.ids.size(); i++) {
        std::string case_id = node.ids[i];
        std::pair<TypeNode*, ast::Block*> c = node.cases[i];
        TypeNode* case_type = c.first;
        ast::Block* case_node = c.second;

        this->module.fill_actual(case_type);

        int union_index = target_union_type(*ot, *case_type);
        if (union_index == -1) {
            this->error_reporter.fail("Error, type " + case_type->to_string() + " not part of " + ot->to_string());
            return error_stub();
        }
        this->enter_scope("case");
        auto v = std::make_unique<Value>(case_type);
        this->fill_value(*v);
        Entity* ent = new EntityValue(std::move(v));
        assert(v->clazz != nullptr);
        this->scope->set(case_id, ent);
        USemanticInfo case_info = this->dispatch(*case_node);
        auto* bn = (BlockSNode*) case_info->snode;
        auto* omn = new ObjectMemberSNode(new IdSNode(varname), Path("core.core.Union"), "o");
        USNode u(omn);
        auto* dn = new DeclarationSNode(case_id, u);
        bn->nodes.insert(bn->nodes.begin(), dn);
        cas.emplace_back(union_index, (BlockSNode*) case_info->snode);
        this->leave_scope();
    }
    USNode up(exp_info->snode);
    auto* init = new DeclarationSNode(varname, up);
    auto* mn = new MatchSNode(init, varname, cas);

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.snode = mn;
    return info_u;
}

USemanticInfo Checker::visit_continue(ContinueNode& node) {
    auto* bn = new BlockSNode();
    if (this->update_loop_index_snode != nullptr) {
        bn->nodes.push_back(this->update_loop_index_snode);
    }
    auto* cn = new ContinueSNode();
    bn->nodes.push_back(cn);

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    for (auto reachable : this->scope->get_all_in_loop()) {
        cn->reachables.push_back(reachable.first);
    }
    info.snode = bn;
    return info_u;
}

USemanticInfo Checker::visit_for(ForNode& node) {
    USemanticInfo exp_info_p = this->dispatch_rvalue(*node.exp);
    if (exp_info_p->entity.get().type != E_TYPE::VALUE) {
        this->error_reporter.error(ErrorFor(exp_info_p->entity, node.exp->start));
    }
    EntityValue& exp_entity_value = (EntityValue&) exp_info_p->entity;
    ObjectType* exp_ot = &exp_entity_value.value->type->object();
    if (exp_ot->id != "List") {
        this->error_reporter.error(ErrorFor(exp_entity_value, node.exp->start));
    }

    TypeNode* elem_type = exp_ot->type_params[0];
    auto v = std::make_unique<Value>(elem_type);
    this->fill_value(*v);
    Entity* elem_entity = new EntityValue(std::move(v));
    this->enter_scope("for");
    this->scope->set(node.var, elem_entity);

    std::string loop_c = std::to_string(this->loop_count++);
    std::string loop_list_var_id = "__loop_list__" + loop_c;
    std::string loop_index_var_id = "__loop_index__" + loop_c;
    std::string loop_list_len_var_id = "__loop_list_len__" + loop_c;

    USNode lu;
    USNode eu;
    auto* increment_index_sn = new AssignmentSNode(lu, eu);
    this->update_loop_index_snode = increment_index_sn;
    increment_index_sn->lvalue = std::make_unique<IdSNode>(loop_index_var_id);
    auto inc_exp_node = std::make_unique<CallSNode>(new IdSNode("core.core.Integer.__add__"),
                                                    std::vector<SNode*>{new IdSNode(loop_index_var_id)});
    auto* one_node = new IntegerSNode(std::string());
    one_node->str = "1";
    inc_exp_node->arguments.push_back(one_node);
    increment_index_sn->rvalue = std::move(inc_exp_node);

    this->scope->is_loop = true;
    USemanticInfo binfo = this->visit_block(*node.body);
    this->scope->is_loop = false;
    BlockSNode* bn = (BlockSNode*) binfo->snode;
    for (auto local_var : this->scope->table) {
        if (local_var.second->type == E_TYPE::VALUE) {
            bn->locals.push_back(local_var.first);
        }
    }
    this->leave_scope();

    SemanticInfo rinfo;
    rinfo.snode = make_for_snode(node,
                                 binfo,
                                 exp_info_p,
                                 loop_list_var_id,
                                 loop_index_var_id,
                                 loop_list_len_var_id,
                                 this->update_loop_index_snode);
    BlockSNode* pn = (BlockSNode*) rinfo.snode;
    pn->locals.push_back(loop_list_var_id);
    this->update_loop_index_snode = nullptr;
    return std::make_unique<SemanticInfo>(rinfo);
}

USemanticInfo Checker::visit_break(BreakNode& node) {
    // node.loop_vars = this->scope->get_all_in_loop();
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    BreakSNode* bn = new BreakSNode();
    info.snode = bn;
    for (auto reachable : this->scope->get_all_in_loop()) {
        bn->reachables.push_back(reachable.first);
    }
    return info_u;
}

USemanticInfo Checker::visit_while(ast::While& node) {
    USemanticInfo condition_sinfo = this->expect_rvalue_of_type(T_BOOL, *node.condition);
    if (condition_sinfo->is_error()) {
        return error_stub();
    }
    USNode condition_snode = USNode(condition_sinfo->snode);

    this->enter_scope("while");
    this->scope->is_loop = true;
    USemanticInfo body_info_p = this->visit_block(*node.body);
    this->scope->is_loop = false;
    for (auto v : this->scope->table) {
        // if (v.second->type == E_TYPE::OBJECT_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((ObjectValue*) v.second)->ot));
        // }
        // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((FunctionValue*) v.second)->ft));
        // }
    }
    this->leave_scope();

    auto* while_sn = new WhileSNode(std::move(condition_snode), (BlockSNode*) body_info_p->snode);

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.snode = while_sn;
    return info_u;
}

USemanticInfo Checker::visit_if(IfNode& n) {
    USemanticInfo condition_sinfo = this->expect_rvalue_of_type(T_BOOL, *n.condition);
    if (condition_sinfo->is_error()) {
        return error_stub();
    }
    SNode* condition_snode = condition_sinfo->snode;

    this->enter_scope("if");
    USemanticInfo body_info = this->visit_block(*n.then);
    BlockSNode* bn = (BlockSNode*) body_info->snode;
    for (const auto& local_var : this->scope->table) {
        bn->locals.push_back(local_var.first);
    }
    this->leave_scope();

    std::vector<std::pair<SNode*, BlockSNode*>> elifs;

    for (auto& elif : n.elifs) {
        USemanticInfo elif_condition_sinfo = this->expect_rvalue_of_type(T_BOOL, *elif.first);
        SNode* elif_condition_snode = elif_condition_sinfo->snode;
        this->enter_scope("elif");
        USemanticInfo elif_block_info = this->visit_block(*elif.second);
        BlockSNode* bn1 = (BlockSNode*) elif_block_info->snode;
        for (const auto& local_var : this->scope->table) {
            bn1->locals.push_back(local_var.first);
        }
        this->leave_scope();
        elifs.emplace_back(elif_condition_snode, (BlockSNode*) elif_block_info->snode);
    }
    USemanticInfo else_info;
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        else_info = this->visit_block(*n.selse);
        BlockSNode* bn2 = (BlockSNode*) else_info->snode;
        for (const auto& local_var : this->scope->table) {
            bn2->locals.push_back(local_var.first);
        }
        this->leave_scope();
    }
    SNode* else_snode = else_info == nullptr ? nullptr : else_info->snode;

    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.snode = new IfSNode(condition_snode, (BlockSNode*) body_info->snode, elifs, (BlockSNode*) else_snode);
    return info_u;
}

USemanticInfo Checker::visit_try_catch(TryCatchNode& node) {
    // this->enter_scope("try");
    // USemanticInfo body_info = this->visit_block(*node.body);
    // this->leave_scope();
    // std::vector<std::pair<std::string, std::string>> e_names_types;
    // std::vector<SNode*> catches_bodies_snodes;
    // for (size_t i = 0; i < node.catches.size(); i++) {
    //     this->enter_scope("catch");
    //     ObjectType* et = node.catches[i].second;
    //     std::string eid = node.catches[i].first;
    //
    //     Node* catch_body = node.catches_bodies[i];
    //
    //     USemanticInfo ex_info = this->dispatch(*new IdNode(et->id, {0, 0}, {0, 0}));
    //     Entity& ex_class_entity = *ex_info->entity;
    //     Entity& ex_entity = *entity_from_type(*et);
    //     ex_entity.value->type->object().actual_base_path = ex_class_entity.clazz->path;
    //     this->fill_value(ex_entity.value);
    //     e_names_types.push_back(std::make_pair(eid, ex_class_entity.clazz->path.as_str()));
    //
    //     this->scope->set(eid, ex_entity);
    //     USemanticInfo catch_body_info = this->visit_block(*((BlockNode*) catch_body));
    //     catches_bodies_snodes.push_back(catch_body_info->snode);
    //     this->leave_scope();
    // }
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    // info.snode = new TryCatchSNode((BlockSNode*) body_info->snode, e_names_types, catches_bodies_snodes);
    return info_u;
}