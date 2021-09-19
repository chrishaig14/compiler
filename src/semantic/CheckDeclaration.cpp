//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"
#include "../ast/ObjectType.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "errors/ErrorTypeMismatch.h"
#include "errors/ErrorRedeclared.h"
#include "errors/ErrorExpectedExpression.h"

std::unique_ptr<SemanticInfo> Checker::expect_rvalue_of_type(const sem::Type& target, ast::Node& node) {
    USemanticInfo rinfo = this->dispatch_rvalue(node);
    if (rinfo->is_error()) {
        return error_stub();
    }
    Entity& r_entity = rinfo->entity.get();
    if (r_entity.type != E_TYPE::VALUE && r_entity.type != E_TYPE::CONST_FUNCTION) {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(target, node, r_entity));
        return error_stub();
    }
    sem::UExp snode = make_rvalue(r_entity, std::move(rinfo->exp_snode), target);
    if (snode == nullptr) {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(target, node, r_entity));
        return error_stub();
    }
    rinfo->exp_snode = std::move(snode);
    return rinfo;
}

sem::UExp Checker::make_rvalue(const Entity& t_entity, sem::UExp value_snode, const sem::Type& target) {
    if (t_entity.type == E_TYPE::VALUE) {
        Value& value_entity = (Value&) t_entity;
        if (value_entity.type.kind != target.kind) {
            return nullptr;
        }
        if (value_entity.type.kind == sem::Kind::FUNCTION) {
            if (value_entity.type == target) {
                return value_snode;
            } else {
                return nullptr;
                // throw std::runtime_error("Error cannot make function rvalue");
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
            return sem::UExp(make_option_rvalue(value_snode.release(),
                                             unaliased_value_type->to_ast(),
                                             unaliased_target_type->to_ast()));
        }

    } else if (t_entity.type == E_TYPE::CONST_FUNCTION) {
        EntityConstFunction& const_function_entity = (EntityConstFunction&) t_entity;
        if (const_function_entity.const_function.const_function_ft == target) {
            return value_snode;
        } else {
            return nullptr;
            throw std::runtime_error("Error cannot make function rvalue");
        }
        return value_snode;
        this->error_reporter.fail("MAKE RVALUE OF FUNCTION!");
        return nullptr;
    }
    return nullptr;
}

sem::Exp* Checker::make_option_rvalue(sem::Exp* value_snode, const ast::Type* unaliased_value_type,
                                        const ast::Type* unaliased_target_type) const {
    if (*unaliased_target_type->object().type_params[0] == *unaliased_value_type ||
        unaliased_value_type->object().id == "NoneType") {
        return value_snode;
    }
    return nullptr;
}

sem::UExp Checker::make_union_rvalue(sem::UExp value_snode, const sem::Type* unaliased_value_type,
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

USemanticInfo Checker::visit_declaration(ast::Declaration& n) {
    // Logger::info("Checking ast::DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier)) {
        this->error_reporter.error(std::make_unique<ErrorRedeclared>(n.identifier, n));
    }
    USemanticInfo info_u;
    if (n.type != nullptr) {
        info_u = this->check_declaration_with_type(n);
    } else {
        info_u = this->check_declaration_without_type(n);
    }
    this->scope->set(n.identifier, info_u->entity.get());
    return info_u;
}

USemanticInfo Checker::check_declaration_with_type(ast::Declaration& n) {
    ast::UTypeNode& type = n.type;
    if (type->kind == Kind::OBJECT && this->module.aliased_types.count(type->object().id) == 1) {
        ast::Type* aliased_type = this->module.aliased_types.at(type->object().id);
        type = ast::UTypeNode(aliased_type->clone());
    } else {
        this->module.fill_actual(*type);
    }
    sem::UType sem_type(type->to_sem());
    USemanticInfo rvalue_sinfo = this->expect_rvalue_of_type(*sem_type, n.expression);
    if (rvalue_sinfo->is_error()) {
        return error_stub();
    }
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    sem::UExp up = std::move(rvalue_sinfo->exp_snode);
    info.snode = std::make_unique<sem::Declaration>(n.identifier, std::move(up));
    // auto ov = std::make_unique<Value>(sem_type.release());
    // this->fill_value(*ov);
    auto ov = this->make_value(sem_type.release());
    info.set_entity(ov.release());
    return info_u;
}

USemanticInfo Checker::check_declaration_without_type(ast::Declaration& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    if (exp_info_p->is_error()) {
        return error_stub();
    }
    E_TYPE entity_type = exp_info_p->entity.get().type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(exp_info_p->entity, n.expression));
        return error_stub();
    }

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    sem::UExp u = std::move(exp_info_p->exp_snode);
    info.snode = std::make_unique<sem::Declaration>(n.identifier, std::move(u));
    info.set_entity(exp_info_p->entity.get().clone());
    if (info.entity.get().type == E_TYPE::CONST_FUNCTION) {
        Entity& entity_const_function = exp_info_p->entity;
        ConstFunction& const_function = ((EntityConstFunction&) entity_const_function).const_function;
        Value* value_entity = std::make_unique<Value>(const_function.const_function_ft.clone()).release();
        info.set_entity(value_entity);

        if (value_entity->type.is_generic()) {
            this->error_reporter.fail(
                    "Error: you need to specialize the generic function of type " + value_entity->type.to_string() +
                    " to be able to use it without calling it");
        }
    }
    return info_u;
}