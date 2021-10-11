//
// Created by chris on 2/5/21.
//

#include "CheckStatements.h"

#include "../ast/general/ObjectType.h"
#include "../simple_nodes/common/include/Throw.h"
#include "../simple_nodes/common/include/TryCatch.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/expressions/include/CallExp.h"
#include "../simple_nodes/common/include/TypeFunction.h"


sem::UCommon ModuleChecker::visit_lvalue_subscript(ast::Subscript& node) {
    UExpressionInfo parent_p = this->dispatch_rvalue(*node.parent);
    Entity& entity_parent = parent_p->entity.get();
    if (entity_parent.e_type != E_TYPE::VALUE || entity_parent.get_value().type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return nullptr;
    }
    auto& entity_parent_value = entity_parent.get_value();
    if (entity_parent_value.metatype == Meta::ENUM) {
        this->error_reporter.fail("Error: no subscript in enum");
        return nullptr;
    }
    ConcreteClass* cls = entity_parent_value.clazz;
    assert(cls != nullptr);
    if (!cls->type_params.empty()) {
        cls = instantiate_generic(*cls, *(ast::ObjectType*) entity_parent_value.type.object().to_ast());
    }

    auto subscript_it = cls->methods.find("__set_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(std::make_unique<ErrorObjectNoSpecialMethod>(entity_parent_value.type,
                                                                                "__set_item__",
                                                                                node));
        return nullptr;
    }
    ConstFunction& subscript_fun = *subscript_it->second;
    std::string sub_fun_path = subscript_fun.path.as_str();
    sem::Type& rtype = *subscript_fun.const_function_ft.return_type;

    ast::VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
    }
    UExpressionInfo child_sinfo = this->expect_rvalue_of_type(*subscript_fun.const_function_ft.param_types[0],
                                                              *node.child[0]);
    if (child_sinfo->is_error()) {
        return nullptr;
    }
    auto& child_snode = child_sinfo->exp_snode;

    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    // auto value = std::make_unique<Value>(rtype.clone());
    // this->fill_value(*value);
    info.set_entity(this->make_value(rtype.clone()));

    auto fsn = std::make_unique<sem::ObjectMethod>(std::move(parent_p->exp_snode), cls->path, "__set_item__");
    std::vector<sem::UExp> v;
    v.push_back(std::move(child_snode));
    auto csn = std::make_unique<sem::Call>(std::move(fsn), std::move(v));
    return csn;
}

sem::UCommon ModuleChecker::visit_assignment(ast::Assignment& n) {
    if (n.lvalue.ntype == ExpNodeType::ID) {
        if (((ast::Id&) n.lvalue)._id == "_") {
            // ExpressionInfo rv = this->dispatch_rvalue(n.rvalue);
            // return rv;
            return nullptr;
        }
    }

    UExpressionInfo linfo_p;
    bool is_subscript = false;
    std::unique_ptr<sem::Call> csn = nullptr;
    std::unique_ptr<sem::Call> lsub;
    if (n.lvalue.ntype == ExpNodeType::SUB) {
        // special case
        lsub = std::unique_ptr<sem::Call>(static_cast<sem::Call*>(this->visit_lvalue_subscript((ast::Subscript&) n.lvalue).release()));
        is_subscript = true;
    } else {
        linfo_p = this->dispatch_rvalue(n.lvalue);
    }

    UExpressionInfo expression_info_p = this->dispatch_rvalue(n.rvalue);

    if (expression_info_p->is_error()) {
        // return error_stub();
        return nullptr;
    }

    if (is_subscript) {
        // info.snode = std::move(linfo_p->snode);
        // csn->arguments.push_back(std::move(expression_info_p->exp_snode));
        lsub->arguments.push_back(std::move(expression_info_p->exp_snode));
        return lsub;
    }

    if (linfo_p->is_error()) {
        // return error_stub();
        return nullptr;
    }

    if (linfo_p->entity.get().e_type != E_TYPE::VALUE) {
        // this->error_reporter.error(std::make_unique<ErrorCantAssign>(n.lvalue));
        // this->error_reporter.fail("Cannot assign to this thing!");
        std::runtime_error("Error cant assignt to this thing!");
        // return error_stub();
        return nullptr;
    }
    EntityValue& e_value = linfo_p->entity.get().get_value();

    if (e_value.type.kind == sem::Kind::OBJECT) {
        // bool ff = n.lvalue->ntype == NodeType::MEMBER;
        if (linfo_p->is_tuple_member) {
            // this->error_reporter.error(std::make_unique<ErrorCantAssign>(n.lvalue));
            std::runtime_error("Error cant assignt to this thing!");

            // return error_stub();
            return nullptr;
        }
    }

    if (e_value.type.kind == sem::Kind::OBJECT && expression_info_p->entity.get().is_constfun()) {
        std::cerr << "assignment error" << std::endl;
        exit(111);
        // this->error_reporter.assignment(*e_value.value->type,
        //                                 *expression_info_p->entity.value->type,
        //                                 n.rvalue->start,
        //                                 *n.lvalue,
        //                                 *n.rvalue);
    }
    ast::Type* exp_type = expression_info_p->entity.get().get_value().type.to_ast();

    if (exp_type->kind == Kind::OBJECT && this->module.aliased_types.count(exp_type->object().id) == 1) {
        ast::Type* aliased_type = this->module.aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
    } else {
        // if (exp_type->kind == Kind::OBJECT && exp_type->object().id.size() != 1) {
        //     this->module.fill_actual(exp_type);
        // }
    }

    if (expression_info_p->is_error()) {
        return nullptr;
    }
    ExpressionInfo& linfo = *linfo_p;

    EntityValue& l_entity_value = linfo.entity.get().get_value();

    if (expression_info_p->entity.get().is_value()) {
        sem::UExp rvalue_snode = this->make_rvalue(expression_info_p->entity,
                                                   std::move(expression_info_p->exp_snode),
                                                   l_entity_value.type);
        if (rvalue_snode == nullptr) {
            this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(l_entity_value.type,
                                                                           n.rvalue,
                                                                           expression_info_p->entity));
            // return error_stub();
            return nullptr;
        }
        expression_info_p->exp_snode = std::move(rvalue_snode);
    }

    sem::UCommon info_u;

    auto lu = std::move(linfo_p->exp_snode);
    auto eu = std::move(expression_info_p->exp_snode);
    info_u = std::make_unique<sem::Assignment>(std::move(lu), std::move(eu));


    return info_u;
}

sem::UCommon ModuleChecker::visit_return(ast::Return& n) {
    Entity& return_entity = this->scope->get("__return__");
    if (return_entity.is_nothing()) {
        if (n.expression != nullptr) {
            this->error_reporter.error(std::make_unique<ErrorBadReturn>(n.start));
        }
        sem::UCommon info_r = std::make_unique<sem::Return>(nullptr);
        return info_r;
    }
    sem::Type& return_type = return_entity.get_value().type;
    // if (return_type.kind == sem::Kind::OBJECT && this->module.aliased_types.count(return_type.object().id) == 1) {
    //     ast::Type* aliased_type = this->module.aliased_types.at(return_type.object().id);
    //     return_type = aliased_type;
    // } else {
    this->module.fill_actual(return_type);
    // }
    if (n.expression == nullptr) {
        // this->error_reporter.no_return(*return_type, n.start);
        // return error_stub();
        return nullptr;
    }

    UExpressionInfo expression_info_p = this->expect_rvalue_of_type(return_type, *n.expression);
    if (expression_info_p->is_error()) {
        // return error_stub();
        return nullptr;
    }
    auto& u = expression_info_p->exp_snode;
    auto sn = std::make_unique<sem::Return>(std::move(u));
    for (auto& l : this->scope->get_all()) {
        sn->reachables.push_back(l.first);
    }

    return sn;
}

// sem::UCommon Checker::visit_throw(ThrowNode& n) {
//     sem::UCommon expression_info_p = this->dispatch_rvalue(n.exp);
//     if (expression_info_p->is_error()) {
//         return error_stub();
//     }
//
//     auto* sn = new ThrowSNode(expression_info_p->snode);
//     for (auto l: this->scope->get_all()) {
//         sn->reachables.push_back(l.first);
//     }
//
//     sem::UCommon info_u; SemanticInfo& info = *info_u;
//     info.snode = sn;
//     return info_u;
// }

sem::UCommon ModuleChecker::visit_match(ast::Match& node) {
    UExpressionInfo exp_info = this->dispatch_rvalue(*node.exp);
    Entity& entity = exp_info->entity.get();
    bool a = entity.e_type != E_TYPE::VALUE;
    EntityValue& value = entity.get_value();
    bool b = value.type.kind != sem::Kind::OBJECT;
    if (a || b) {

        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Union",
                                                                                            {new sem::TypeObject("...",
                                                                                                                 sem::VectorOfTypes{})}),
                                                                       *node.exp,
                                                                       exp_info->entity));
        // return error_stub();
        return nullptr;
    }

    sem::TypeObject& ot = value.type.object();
    // if (ot.data.aliased_type != nullptr) {
    //     ot = (ast::ObjectType*) ot->data.aliased_type;
    // }
    if (ot.id != "Union") {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Union",
                                                                                            {new sem::TypeObject("...",
                                                                                                                 sem::VectorOfTypes{})}),
                                                                       *node.exp,
                                                                       exp_info->entity));
        // return error_stub();
        return nullptr;
    }
    std::vector<std::pair<int, std::unique_ptr<sem::Block>>> cas;
    std::string varname = "match_var";
    for (size_t i = 0; i < node.ids.size(); i++) {
        std::string case_id = node.ids[i];
        std::pair<ast::UTypeNode, ast::UBlock>& c = node.cases[i];
        ast::Type& case_type = *c.first;
        ast::Block& case_node = *c.second;

        this->module.fill_actual(case_type);

        sem::Type* p_type = case_type.to_sem();
        int union_index = target_union_type(ot, *p_type);
        if (union_index == -1) {
            this->error_reporter.fail("Error, type " + case_type.to_string() + " not part of " + ot.to_string());
            // return error_stub();
            return nullptr;
        }
        this->enter_scope("case");
        // auto v = std::make_unique<Value>(case_type.to_sem());
        // this->fill_value(*v);
        auto v = this->make_value(p_type);
        assert(v->clazz != nullptr);
        this->scope->set(case_id, *v);
        auto bn = this->visit_block(case_node);
        auto* omn = new sem::ObjectMember(std::make_unique<sem::Id>(varname), Path("libcore.libcore.Union"), "o");
        sem::UExp u(omn);
        auto dn = std::make_unique<sem::Declaration>(case_id, std::move(u));
        bn->nodes.insert(bn->nodes.begin(), std::move(dn));
        cas.emplace_back(union_index, std::move(bn));
        this->leave_scope();
    }
    return std::make_unique<sem::Match>(std::move(exp_info->exp_snode), varname, std::move(cas));
}

sem::UCommon ModuleChecker::visit_continue(ast::Continue& node) {
    auto bn = std::make_unique<sem::Block>();
    if (this->update_loop_index_snode != nullptr) {
        bn->nodes.push_back(sem::UCommon(this->update_loop_index_snode));
    }
    auto cn = std::make_unique<sem::Continue>();
    bn->nodes.push_back(std::move(cn));

    // sem::UCommon info_u;
    // SemanticInfo& info = *info_u;
    for (auto reachable : this->scope->get_all_in_loop()) {
        cn->reachables.push_back(reachable.first);
    }
    // info.snode = std::move(bn);
    return bn;
}

std::unique_ptr<EntityValue> ModuleChecker::make_entity_value(sem::Type& type) {
    return this->make_value(type.clone());
    // auto e = std::make_unique<EntityValue>(type.clone());
    // this->fill_value(*e);
    // return e;
}

sem::UCommon ModuleChecker::visit_for(ast::For& node) {
    UExpressionInfo exp_info_p = this->dispatch_rvalue(node.exp);
    if (exp_info_p->entity.get().e_type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<ErrorFor>(exp_info_p->entity, node.exp.start));
        return nullptr;
    }
    EntityValue& exp_entity_value = exp_info_p->entity.get().get_value();
    sem::TypeObject& exp_ot = exp_entity_value.type.object();
    if (exp_ot.id != "List") {
        this->error_reporter.error(std::make_unique<ErrorFor>(exp_entity_value, node.exp.start));
        return nullptr;
    }

    sem::Type* elem_type = exp_ot.type_params[0];
    // EntityValue v(elem_type->clone());
    // this->fill_value(v);
    auto ev = this->make_entity_value(*elem_type);
    this->enter_scope("for");
    this->scope->set(node.var, *ev);
    //
    // std::string loop_c = std::to_string(this->loop_count++);
    // std::string loop_list_var_id = "__loop_list__" + loop_c;
    // std::string loop_index_var_id = "__loop_index__" + loop_c;
    // std::string loop_list_len_var_id = "__loop_list_len__" + loop_c;
    //
    // sem::UExp lu;
    // sem::UExp eu;
    // auto* increment_index_sn = new sem::Assignment(std::move(lu), std::move(eu));
    // this->update_loop_index_snode = increment_index_sn;
    // increment_index_sn->lvalue = std::make_unique<sem::Id>(loop_index_var_id);
    // std::vector<sem::UExp> vv;
    // vv.push_back(std::make_unique<sem::Id>(loop_index_var_id));
    // auto inc_exp_node = std::make_unique<sem::CallExp>(std::make_unique<sem::Id>("libcore.libcore.Integer.__add__"),
    //                                                    std::move(vv));
    // auto one_node = std::make_unique<sem::Integer>(std::string());
    // one_node->str = "1";
    // inc_exp_node->arguments.push_back(std::move(one_node));
    // increment_index_sn->rvalue = std::move(inc_exp_node);

    this->scope->is_loop = true;
    auto binfo = this->visit_block(node.body);
    this->scope->is_loop = false;
    // sem::Block* bn = binfo.release();
    // for (auto& local_var : this->scope->table) {
    //     if (local_var.second->is_value()) {
    //         bn->locals.push_back(local_var.first);
    //     }
    // }
    this->leave_scope();

    if (binfo == nullptr) {
        return nullptr;
    }
    sem::UCommon rinfo_p = std::make_unique<sem::For>(node.var, std::move(exp_info_p->exp_snode), std::move(binfo));
    return rinfo_p;
}

sem::UCommon ModuleChecker::visit_break(ast::Break& node) {
    // node.loop_vars = this->scope->get_all_in_loop();
    auto bn = std::make_unique<sem::Break>();
    for (auto reachable : this->scope->get_all_in_loop()) {
        bn->reachables.push_back(reachable.first);
    }
    return bn;
}

sem::UCommon ModuleChecker::visit_while(ast::While& node) {
    UExpressionInfo condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), *node.condition);
    if (condition_sinfo->is_error()) {
        // return error_stub();
        return nullptr;
    }
    sem::UExp condition_snode = std::move(condition_sinfo->exp_snode);

    this->enter_scope("while");
    this->scope->is_loop = true;
    auto body_snode = this->visit_block(*node.body);
    this->scope->is_loop = false;
    // for (auto& v : this->scope->table) {
    // if (v.second->type == E_TYPE::OBJECT_VALUE) {
    //     node.body->local_vars.push_back(std::make_pair(v.first, ((ObjectValue*) v.second)->ot));
    // }
    // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
    //     node.body->local_vars.push_back(std::make_pair(v.first, ((FunctionValue*) v.second)->ft));
    // }
    // }
    this->leave_scope();

    auto while_sn = std::make_unique<sem::While>(std::move(condition_snode), std::move(body_snode));

    return while_sn;
}

sem::UCommon ModuleChecker::visit_if(ast::If& n) {
    UExpressionInfo condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), n.condition);
    if (condition_sinfo->is_error()) {
        // return error_stub();
        return nullptr;
    }
    auto& condition_snode = condition_sinfo->exp_snode;

    this->enter_scope("if");
    auto body_info = this->visit_block(n.then);
    sem::Block& bn = *body_info;
    for (const auto& local_var : this->scope->table) {
        bn.locals.push_back(local_var.first);
    }
    this->leave_scope();

    std::vector<std::pair<sem::UExp, std::unique_ptr<sem::Block>>> elifs;

    for (auto& elif : n.elifs) {
        UExpressionInfo elif_condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), elif.first);
        auto& elif_condition_snode = elif_condition_sinfo->exp_snode;
        this->enter_scope("elif");
        auto elif_block_info = this->visit_block(elif.second);
        sem::Block* bn1 = elif_block_info.release();
        for (const auto& local_var : this->scope->table) {
            bn1->locals.push_back(local_var.first);
        }
        this->leave_scope();
        elifs.emplace_back(std::move(elif_condition_snode), std::move(elif_block_info));
    }
    std::unique_ptr<sem::Block> else_info;
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        else_info = this->visit_block(*n.selse);
        auto& bn2 = else_info;
        for (const auto& local_var : this->scope->table) {
            bn2->locals.push_back(local_var.first);
        }
        this->leave_scope();
    }
    std::unique_ptr<sem::Block> else_snode = else_info == nullptr ? nullptr : std::move(else_info);

    return std::make_unique<sem::If>(std::move(condition_snode),
                                     std::move(body_info),
                                     std::move(elifs),
                                     std::move(else_snode));
}
