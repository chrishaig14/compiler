//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"
#include <ast/general/ObjectType.h>
#include <simple_nodes/common/include/TypeObject.h>

UExpressionInfo ModuleChecker::expect_rvalue_of_type(const sem::Type& target, ast::ExpNode& node) {
    UExpressionInfo rinfo = this->dispatch_rvalue(node);
    if (rinfo->is_error()) {
        return exp_error_stub();
    }
    Entity& r_entity = rinfo->entity.get();
    if (r_entity.is_nothing()) {
        this->error_reporter.error(std::make_unique<error::ExpectedExpression>(r_entity, node));
        return exp_error_stub();
    }
    if (r_entity.e_type != E_TYPE::VALUE && r_entity.e_type != E_TYPE::CONST_FUNCTION &&
        r_entity.e_type != E_TYPE::NONE) {
        this->error_reporter.error(std::make_unique<error::TypeMismatch>(target, node, r_entity));
        return exp_error_stub();
    }
    sem::UExp snode = make_rvalue(r_entity, std::move(rinfo->exp_snode), target);
    if (snode == nullptr) {
        this->error_reporter.error(std::make_unique<error::TypeMismatch>(target, node, r_entity));
        return exp_error_stub();
    }
    rinfo->exp_snode = std::move(snode);
    return rinfo;
}

sem::UExp ModuleChecker::make_rvalue(const Entity& t_entity, sem::UExp value_snode, const sem::Type& target) {
    if (t_entity.is_value()) {
        const EntityValue& value_entity = t_entity.get_value();
        if (value_entity.type.kind != target.kind) {
            return nullptr;
        }
        if (value_entity.type.kind == sem::Kind::FUNCTION) {
            if (value_entity.type == target) {
                return value_snode;
            } else {
                return nullptr;
            }
        }
        const sem::TypeObject& value_ot = value_entity.type.object();
        const sem::TypeObject& target_ot = target.object();

        const sem::Type* unaliased_value_type = &value_ot;
        if (value_ot.data.aliased_type != nullptr) {
            unaliased_value_type = value_ot.data.aliased_type;
        }

        const sem::Type* unaliased_target_type = &target_ot;
        if (target_ot.data.aliased_type != nullptr) {
            unaliased_target_type = target_ot.data.aliased_type;
        }

        if (unaliased_value_type->kind != sem::Kind::OBJECT || unaliased_target_type->kind != sem::Kind::OBJECT) {
            return nullptr;
        }

        if (unaliased_value_type->object() == unaliased_target_type->object()) {
            return value_snode;
        }

        const std::string& unaliased_target_type_id = unaliased_target_type->object().id;
        if (unaliased_target_type_id == "Union") {
            return sem::UExp(make_union_rvalue(std::move(value_snode), unaliased_value_type, unaliased_target_type));
        }

        if (unaliased_target_type_id == "Option") {
            return std::make_unique<sem::Option>(std::move(value_snode));
        }

    } else if (t_entity.is_constfun()) {
        if (t_entity.get_constfun().const_function.const_function_ft == target) {
            return value_snode;
        } else {
            return nullptr;
            throw std::runtime_error("Error cannot make function rvalue");
        }
        return value_snode;
        this->error_reporter.fail("MAKE RVALUE OF FUNCTION!");
        return nullptr;
    } else if (t_entity.is_none()) {
        if (target.object().id == "Option") {
            return std::make_unique<sem::Option>(nullptr);
        }
    }
    return nullptr;
}

sem::UExp ModuleChecker::make_union_rvalue(sem::UExp value_snode, const sem::Type* unaliased_value_type,
                                           const sem::Type* unaliased_target_type) const {
    int union_index = target_union_type(unaliased_target_type->object(), unaliased_value_type->object());
    if (union_index != -1) {
        return sem::UExp(make_union_wrapper(union_index, std::move(value_snode)));
    } else {
        if (unaliased_value_type->object().id == "Union") {
            if (unaliased_value_type->object().type_params.size() <=
                unaliased_target_type->object().type_params.size()) {
                // might be assigning a Union[Int, Str] to a Union[Int, Str, Bool] which should work!
                // for now the types order must be the same, but it should be necessary
                for (size_t i = 0; i < unaliased_value_type->object().type_params.size(); i++) {
                    if (*unaliased_value_type->object().type_params[i] !=
                        *unaliased_target_type->object().type_params[i]) {
                        // this->error_reporter.fail(
                        //         "Error: Cannot lift union type " + unaliased_value_type->to_string() + " to " +
                        //         unaliased_target_type->to_string());
                        return nullptr;
                    }
                }
                return value_snode;
            }
        }
        return nullptr;
    }
}

sem::UCommon ModuleChecker::visit_declaration(const ast::Declaration& n) {
    // Logger::info("Checking ast::DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier.str)) {
        this->error_reporter.error(std::make_unique<error::Redeclared>(n));
    }
    sem::UCommon info_u;
    if (n.type != nullptr) {
        info_u = this->check_declaration_with_type(n);
    } else {
        info_u = this->check_declaration_without_type(n);
    }
    return info_u;
}

sem::UCommon ModuleChecker::check_declaration_with_type(const ast::Declaration& n) {
    const ast::UTypeNode& type = n.type;
    // if (type->kind == Kind::OBJECT && this->module.aliased_types.count(type->object().id) == 1) {
    //     ast::Type* aliased_type = this->module.aliased_types.at(type->object().id);
    //     type = ast::UTypeNode(aliased_type->clone());
    // } else {
    // }
    sem::UType sem_type(type->to_sem());
    this->module.fill_actual(*sem_type);
    UExpressionInfo rvalue_sinfo = this->expect_rvalue_of_type(*sem_type, n.expression);

    auto ov = this->make_value(sem_type.release());
    this->scope->set(n.identifier.str, *ov);
    if (rvalue_sinfo->is_error()) {
        return nullptr;
    }

    sem::UExp up = std::move(rvalue_sinfo->exp_snode);
    sem::UCommon info_u = std::make_unique<sem::Declaration>(n.identifier.str, std::move(up));
    return info_u;
}

sem::UCommon ModuleChecker::check_declaration_without_type(const ast::Declaration& n) {
    UExpressionInfo exp_info_p = this->dispatch_rvalue(n.expression);
    std::string identifier = n.identifier.str;
    if (exp_info_p->is_error()) {
        this->scope->set(identifier, exp_info_p->entity);
        return nullptr;
    }
    E_TYPE entity_type = exp_info_p->entity.get().e_type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<error::ExpectedExpression>(exp_info_p->entity, n.expression));
        return nullptr;
    }

    this->scope->set(identifier, exp_info_p->entity);
    if (exp_info_p->exp_snode == nullptr) {
        return nullptr;
    }
    if (exp_info_p->entity.get().is_constfun()) {
        Entity& entity_const_function = exp_info_p->entity;
        ConstFunction& const_function = entity_const_function.get_constfun().const_function;
        auto value_entity = EntityValue::function_value(const_function.const_function_ft.clone());
        this->scope->set(identifier, *value_entity);

        if (value_entity->type.is_generic()) {
            this->error_reporter.fail(
                    "Error: you need to specialize the generic function of type " + value_entity->type.to_string() +
                    " to be able to use it without calling it");
        }
    }
    sem::UExp u = std::move(exp_info_p->exp_snode);
    sem::UCommon info_u = std::make_unique<sem::Declaration>(identifier, std::move(u));
    return info_u;
}