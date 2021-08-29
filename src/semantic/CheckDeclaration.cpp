//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"
#include "../ast/ObjectType.h"
#include "errors/ErrorTypeMismatch.h"
#include "errors/ErrorRedeclared.h"
#include "errors/ErrorExpectedExpression.h"

std::unique_ptr<SemanticInfo> Checker::expect_rvalue_of_type(const TypeNode& target, ast::Node& node) {
    USemanticInfo rinfo = this->dispatch_rvalue(node);
    if (rinfo->is_error()) {
        return error_stub();
    }
    Entity& r_entity = rinfo->entity.get();
    if (r_entity.type != E_TYPE::VALUE && r_entity.type != E_TYPE::CONST_FUNCTION) {
        this->error_reporter.error(ErrorTypeMismatch(*target.clone(), node, r_entity));
        return error_stub();
    }
    sem::SNode* snode = make_rvalue(r_entity, rinfo->snode, target);
    if (snode == nullptr) {
        this->error_reporter.error(ErrorTypeMismatch(*target.clone(), node, r_entity));
        return error_stub();
    }
    rinfo->snode = snode;
    return rinfo;
}

sem::SNode* Checker::make_rvalue(const Entity& t_entity, sem::SNode* value_snode, const TypeNode& target) {
    if (t_entity.type == E_TYPE::VALUE) {
        EntityValue& value_entity = (EntityValue&) t_entity;
        if (value_entity.value->type->kind != target.kind) {
            return nullptr;
        }
        if (value_entity.value->type->kind == Kind::FUNCTION) {
            if (*value_entity.value->type == target) {
                return value_snode;
            } else {
                return nullptr;
                // throw std::runtime_error("Error cannot make function rvalue");
            }
        }
        const ObjectType& value_ot = value_entity.value->type->object();
        const ObjectType& target_ot = target.object();

        const TypeNode* unaliased_value_type = &value_ot;
        if (value_ot.aliased_type != nullptr) {
            unaliased_value_type = value_ot.aliased_type;
        }

        const TypeNode* unaliased_target_type = &target_ot;
        if (target_ot.aliased_type != nullptr) {
            unaliased_target_type = target_ot.aliased_type;
        }

        if (unaliased_value_type->kind != Kind::OBJECT || unaliased_target_type->kind != Kind::OBJECT) {
            return nullptr;
        }

        if (unaliased_value_type->object() == unaliased_target_type->object()) {
            return value_snode;
        }

        const std::string& unaliased_target_type_id = unaliased_target_type->object().id;
        if (unaliased_target_type_id == "Union") {
            return make_union_rvalue(value_snode, unaliased_value_type, unaliased_target_type);
        }

        if (unaliased_target_type_id == "Option") {
            return make_option_rvalue(value_snode, unaliased_value_type, unaliased_target_type);
        }

    } else if (t_entity.type == E_TYPE::CONST_FUNCTION) {
        EntityConstFunction& const_function_entity = (EntityConstFunction&) t_entity;
        if (*const_function_entity.const_function->ft == target) {
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

sem::SNode* Checker::make_option_rvalue(sem::SNode* value_snode, const TypeNode* unaliased_value_type,
                                   const TypeNode* unaliased_target_type) const {
    if (*unaliased_target_type->object().type_params[0] == *unaliased_value_type ||
        unaliased_value_type->object().id == "NoneType") {
        return value_snode;
    }
    return nullptr;
}

sem::SNode* Checker::make_union_rvalue(sem::SNode* value_snode, const TypeNode* unaliased_value_type,
                                  const TypeNode* unaliased_target_type) const {
    int union_index = target_union_type(unaliased_target_type->object(), unaliased_value_type->object());
    if (union_index != -1) {
        return make_union_wrapper(union_index, value_snode);
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
        this->error_reporter.error(ErrorRedeclared(n.identifier, n));
    }
    USemanticInfo info_u;
    if (n.type != nullptr) {
        info_u = this->check_declaration_with_type(n);
    } else {
        info_u = this->check_declaration_without_type(n);
    }
    this->scope->set(n.identifier, &info_u->entity.get());
    return info_u;
}

USemanticInfo Checker::check_declaration_with_type(ast::Declaration& n) {
    if (n.type->kind == Kind::OBJECT && this->module.aliased_types.count(n.type->object().id) == 1) {
        TypeNode* aliased_type = this->module.aliased_types.at(n.type->object().id);
        n.type = UTypeNode(aliased_type->clone());
    } else {
        this->module.fill_actual(*n.type);
    }
    USemanticInfo rvalue_sinfo = this->expect_rvalue_of_type(*n.type, *n.expression);
    if (rvalue_sinfo->is_error()) {
        return error_stub();
    }
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    USNode up(rvalue_sinfo->snode);
    info.snode = new sem::DeclarationSNode(n.identifier, up);
    auto ov = std::make_unique<Value>(n.type->clone());
    this->fill_value(*ov);
    info.entity = *new EntityValue(std::move(ov));
    return info_u;
}

USemanticInfo Checker::check_declaration_without_type(ast::Declaration& n) {
    USemanticInfo exp_info_p = this->dispatch(*n.expression);
    if (exp_info_p->is_error()) {
        return error_stub();
    }
    E_TYPE entity_type = exp_info_p->entity.get().type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.error(ErrorExpectedExpression(exp_info_p->entity, *n.expression));
        return error_stub();
    }

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    USNode u(exp_info_p->snode);
    info.snode = new sem::DeclarationSNode(n.identifier, u);
    info.entity = exp_info_p->entity;
    if (info.entity.get().type == E_TYPE::CONST_FUNCTION) {
        EntityValue* value_entity = new EntityValue(std::make_unique<Value>(((EntityConstFunction&) exp_info_p->entity).const_function->ft->clone()));
        info.entity = *value_entity;

        if (value_entity->value->type->is_generic()) {
            this->error_reporter.fail("Error: you need to specialize the generic function of type " +
                                      value_entity->value->type->to_string() +
                                      " to be able to use it without calling it");
        }
    }
    return info_u;
}