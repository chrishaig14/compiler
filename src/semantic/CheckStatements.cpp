//
// Created by chris on 2/5/21.
//

#include "CheckStatements.h"

#include <ast/general/ObjectType.h>
#include <simple_nodes/common/include/Throw.h>
#include <simple_nodes/common/include/TryCatch.h>
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/expressions/include/CallExp.h>
#include <simple_nodes/common/include/TypeFunction.h>


sem::UCommon ModuleChecker::visit_lvalue_subscript(const ast::Subscript& node) {
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
    const ConcreteClass* cls = entity_parent_value.clazz;
    assert(cls != nullptr);
    // if (!cls->type_params.empty()) {
    //     auto ccls = instantiate_generic(*cls, *(ast::ObjectType*) entity_parent_value.type.object().to_ast());
    //     cls = ccls.get();
    // }

    auto subscript_it = cls->methods.find("__set_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(std::make_unique<error::ObjectNoSpecialMethod>(entity_parent_value.type,
                                                                                  "__set_item__",
                                                                                  node));
        return nullptr;
    }
    const ConstFunction& subscript_fun = subscript_it->second.base.func;
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

sem::UCommon ModuleChecker::visit_assignment(const ast::Assignment& n) {
    if (n.lvalue.ntype == ExpNodeType::ID) {
        if (((ast::Id&) n.lvalue)._id == "_") {
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
        return nullptr;
    }

    if (is_subscript) {
        lsub->arguments.push_back(std::move(expression_info_p->exp_snode));
        return lsub;
    }

    if (linfo_p->is_error()) {
        return nullptr;
    }

    if (linfo_p->entity.get().e_type != E_TYPE::VALUE) {
        std::runtime_error("Error cant assignt to this thing!");
        return nullptr;
    }
    EntityValue& e_value = linfo_p->entity.get().get_value();

    if (e_value.type.kind == sem::Kind::OBJECT) {
        if (linfo_p->is_tuple_member) {
            std::runtime_error("Error cant assignt to this thing!");
            return nullptr;
        }
    }

    if (e_value.type.kind == sem::Kind::OBJECT && expression_info_p->entity.get().is_constfun()) {
        std::cerr << "assignment error" << std::endl;
        exit(111);
    }
    ast::Type* exp_type = expression_info_p->entity.get().get_value().type.to_ast();

    if (exp_type->kind == Kind::OBJECT && this->module.aliased_types.count(exp_type->object().id) == 1) {
        ast::Type* aliased_type = this->module.aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
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
            this->error_reporter.error(std::make_unique<error::TypeMismatch>(l_entity_value.type,
                                                                             n.rvalue,
                                                                             expression_info_p->entity));
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

sem::UCommon ModuleChecker::visit_return(const ast::Return& n) {
    Entity& return_entity = this->scope->get("__return__");
    if (return_entity.is_nothing()) {
        if (n.expression != nullptr) {
            this->error_reporter.error(std::make_unique<error::BadReturn>(n.start));
        }
        sem::UCommon info_r = std::make_unique<sem::Return>(nullptr);
        return info_r;
    }
    sem::Type& return_type = return_entity.get_value().type;
    this->module.fill_actual(return_type);
    if (n.expression == nullptr) {
        return nullptr;
    }

    UExpressionInfo expression_info_p = this->expect_rvalue_of_type(return_type, *n.expression);
    if (expression_info_p->is_error()) {
        return nullptr;
    }
    auto& u = expression_info_p->exp_snode;
    auto sn = std::make_unique<sem::Return>(std::move(u));
    for (auto& l : this->scope->get_all()) {
        sn->reachables.push_back(l.first);
    }

    return sn;
}

sem::UCommon ModuleChecker::visit_match(const ast::Match& node) {
    UExpressionInfo exp_info = this->dispatch_rvalue(*node.exp);
    bool has_error = false;
    bool exp_error = false;

    if (exp_info->is_error()) {
        has_error = true;
        exp_error = true;
    }
    sem::TypeObject* ot = nullptr;
    if (not exp_error) {
        Entity& entity = exp_info->entity.get();
        if (entity.e_type != E_TYPE::VALUE) {

            this->error_reporter.error(std::make_unique<error::TypeMismatch>(*new sem::TypeObject("Union",
                                                                                                  {new sem::TypeObject(
                                                                                                          "...",
                                                                                                          sem::VectorOfTypes{})}),
                                                                             *node.exp,
                                                                             exp_info->entity));
            has_error = true;
            exp_error = true;
        }
        if (not has_error) {
            if (entity.get_value().type.kind != sem::Kind::OBJECT) {

                this->error_reporter.error(std::make_unique<error::TypeMismatch>(*new sem::TypeObject("Union",
                                                                                                      {new sem::TypeObject(
                                                                                                              "...",
                                                                                                              sem::VectorOfTypes{})}),
                                                                                 *node.exp,
                                                                                 exp_info->entity));
                has_error = true;
                exp_error = true;
            }
        }
        if (not has_error) {
            EntityValue& value = entity.get_value();
            ot = &value.type.object();
            if (ot->id != "Union") {
                this->error_reporter.error(std::make_unique<error::TypeMismatch>(*new sem::TypeObject("Union",
                                                                                                      {new sem::TypeObject(
                                                                                                              "...",
                                                                                                              sem::VectorOfTypes{})}),
                                                                                 *node.exp,
                                                                                 exp_info->entity));
                has_error = true;
                exp_error = true;
            }
        }
    }
    std::vector<std::pair<int, sem::Block>> cas;
    std::string varname = "match_var";
    for (size_t i = 0; i < node.ids.size(); i++) {
        std::string case_id = node.ids[i];
        const std::pair<ast::UTypeNode, ast::UBlock>& c = node.cases[i];
        ast::Type& case_type = *c.first;
        ast::Block& case_node = *c.second;

        sem::Type* p_type = case_type.to_sem();
        this->module.fill_actual(*p_type);
        int union_index = -1;
        if (not exp_error) {
            union_index = target_union_type(*ot, *p_type);
            if (union_index == -1) {
                this->error_reporter.fail("Error, type " + case_type.to_string() + " not part of " + ot->to_string());
                // return error_stub();
                has_error = true;
            }
        }
        this->enter_scope();
        auto v = this->make_value(p_type);
        assert(v->clazz != nullptr);
        this->scope->set(case_id, *v);
        auto bn = this->visit_block(case_node);
        if (bn == nullptr) {
            has_error = true;
        }
        if (not has_error) {
            auto* omn = new sem::ObjectMember(std::make_unique<sem::Id>(varname), Path("libcore.libcore.Union"), "o");
            sem::UExp u(omn);
            auto dn = std::make_unique<sem::Declaration>(case_id, std::move(u));
            bn->nodes.insert(bn->nodes.begin(), std::move(dn));
            cas.emplace_back(union_index, *bn);
        }
        this->leave_scope();
    }
    if (has_error){
        return nullptr;
    }
    return std::make_unique<sem::Match>(std::move(exp_info->exp_snode), varname, std::move(cas));
}

sem::UCommon ModuleChecker::visit_continue(const ast::Continue& node) {
    auto bn = std::make_unique<sem::Block>();
    auto cn = std::make_unique<sem::Continue>();
    bn->nodes.push_back(std::move(cn));

    for (auto reachable : this->scope->get_all_in_loop()) {
        cn->reachables.push_back(reachable.first);
    }
    return bn;
}

std::unique_ptr<EntityValue> ModuleChecker::make_entity_value(sem::Type& type) {
    return this->make_value(type.clone());
}

sem::UCommon ModuleChecker::visit_for(const ast::For& node) {
    UExpressionInfo exp_info_p = this->dispatch_rvalue(node.exp);
    bool has_error = false;
    if (exp_info_p->is_error()) {
        has_error = true;
    }
    if (not has_error) {
        // not an error but it has to be a normal value
        if (exp_info_p->entity.get().e_type != E_TYPE::VALUE) {
            this->error_reporter.error(std::make_unique<error::For>(exp_info_p->entity, node.exp.start));
            has_error = true;
        }
    }
    std::unique_ptr<Entity> ev;
    if (not has_error) {
        // not an error, but it has to be a list
        EntityValue& exp_entity_value = exp_info_p->entity.get().get_value();
        sem::TypeObject& exp_ot = exp_entity_value.type.object();
        if (exp_ot.id != "List") {
            this->error_reporter.error(std::make_unique<error::For>(exp_entity_value, node.exp.start));
            has_error = true;
        } else {
            sem::Type* elem_type = exp_ot.type_params[0];
            ev = this->make_entity_value(*elem_type);
        }
    }
    if (has_error) {
        ev = std::make_unique<EntityError>();
    }

    this->enter_scope();
    this->scope->set(node.var, *ev);
    this->scope->is_loop = true;
    auto binfo = this->visit_block(node.body);
    if (binfo == nullptr) {
        has_error = true;
    }
    this->scope->is_loop = false;
    this->leave_scope();

    if (has_error) {
        return nullptr;
    }
    sem::UCommon rinfo_p = std::make_unique<sem::For>(node.var, std::move(exp_info_p->exp_snode), *binfo);
    return rinfo_p;
}

sem::UCommon ModuleChecker::visit_break(const ast::Break& node) {
    auto bn = std::make_unique<sem::Break>();
    for (auto reachable : this->scope->get_all_in_loop()) {
        bn->reachables.push_back(reachable.first);
    }
    return bn;
}

sem::UCommon ModuleChecker::visit_while(const ast::While& node) {
    UExpressionInfo condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), *node.condition);
    bool has_error = false;
    if (condition_sinfo->is_error()) {
        has_error = true;
    }
    this->enter_scope();
    this->scope->is_loop = true;
    auto body_snode = this->visit_block(*node.body);
    if (body_snode == nullptr) {
        has_error = true;
    }
    this->scope->is_loop = false;
    this->leave_scope();
    if (has_error) {
        return nullptr;
    }
    sem::UExp condition_snode = std::move(condition_sinfo->exp_snode);
    auto while_sn = std::make_unique<sem::While>(std::move(condition_snode), *body_snode);
    return while_sn;
}

sem::UCommon ModuleChecker::visit_if(const ast::If& n) {
    UExpressionInfo condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), n.condition);
    bool has_error = false;
    if (condition_sinfo->is_error()) {
        has_error = true;
    }
    auto& condition_snode = condition_sinfo->exp_snode;

    this->enter_scope();
    auto body_info = this->visit_block(n.then);
    if (body_info == nullptr) {
        // error
        has_error = true;
    }
    if (not has_error) {
        sem::Block& bn = *body_info;
        for (const auto& local_var : this->scope->table) {
            bn.locals.push_back(local_var.first);
        }
    }
    this->leave_scope();

    std::vector<std::pair<sem::UExp, sem::Block>> elifs;

    for (auto& elif : n.elifs) {
        UExpressionInfo elif_condition_sinfo = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), elif.first);
        if (elif_condition_sinfo->is_error()) {
            has_error = true;
        }
        auto& elif_condition_snode = elif_condition_sinfo->exp_snode;
        this->enter_scope();
        auto elif_block_info = this->visit_block(elif.second);
        if (elif_block_info == nullptr) {
            has_error = true;
        }
        if (not has_error) {
            sem::Block* bn1 = elif_block_info.release();
            for (const auto& local_var : this->scope->table) {
                bn1->locals.push_back(local_var.first);
            }
        }
        this->leave_scope();
        if (not has_error) {
            elifs.emplace_back(std::move(elif_condition_snode), *elif_block_info);
        }
    }
    std::unique_ptr<sem::Block> else_info;
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope();
        else_info = this->visit_block(*n.selse);
        if (else_info == nullptr) {
            has_error = true;
        }
        if (not has_error) {
            auto& bn2 = else_info;
            for (const auto& local_var : this->scope->table) {
                bn2->locals.push_back(local_var.first);
            }
        }
        this->leave_scope();
    }

    if (has_error) {
        return nullptr;
    }
    std::unique_ptr<sem::Block> else_snode = else_info == nullptr ? nullptr : std::move(else_info);

    return std::make_unique<sem::If>(std::move(condition_snode), *body_info, std::move(elifs), std::move(else_snode));
}
