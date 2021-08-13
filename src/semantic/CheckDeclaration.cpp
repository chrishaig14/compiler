//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"
#include "../nodes/ObjectType.h"
#include "errors/ErrorTypeMismatch.h"

std::unique_ptr<SemanticInfo> Checker::expect_rvalue_of_type(const TypeNode& target, Node& node) {
    USemanticInfo rinfo = this->dispatch_rvalue(&node);
    if (rinfo->is_error()) {
        return error_stub();
    }
    if (rinfo->entity.type != E_TYPE::VALUE && rinfo->entity.type != E_TYPE::CONST_FUNCTION) {
        this->error_reporter.error(*(new ErrorTypeMismatch(target, node, rinfo->entity)));
        return error_stub();
    }
    SNode* snode = make_rvalue(rinfo->entity, rinfo->snode, target);
    if (snode == nullptr) {
        this->error_reporter.error(*(new ErrorTypeMismatch(target, node, rinfo->entity)));
        return error_stub();
    }
    rinfo->snode = snode;
    return rinfo;
}

SNode* Checker::make_rvalue(const Entity& value_entity, SNode* value_snode, const TypeNode& target) {
    if (value_entity.type == E_TYPE::VALUE) {
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

    } else if (value_entity.type == E_TYPE::CONST_FUNCTION) {
        if (*value_entity.const_function->ft == target) {
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

SNode* Checker::make_option_rvalue(SNode* value_snode, const TypeNode* unaliased_value_type,
                                   const TypeNode* unaliased_target_type) const {
    if (*unaliased_target_type->object().type_params[0] == *unaliased_value_type ||
        unaliased_value_type->object().id == "NoneType") {
        return value_snode;
    }
    return nullptr;
}

SNode* Checker::make_union_rvalue(SNode* value_snode, const TypeNode* unaliased_value_type,
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

USemanticInfo Checker::visit_declaration(DeclarationNode& n) {
    // Logger::info("Checking DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier)) {
        this->error_reporter.redeclared(n.identifier, n);
    }
    USemanticInfo info;
    if (n.type != nullptr) {
        info = this->check_declaration_with_type(n);
    } else {
        info = this->check_declaration_without_type(n);
    }
    this->scope->set(n.identifier, info->entity);
    return info;
}

USemanticInfo Checker::check_declaration_with_type(DeclarationNode& n) {
    if (n.type->kind == Kind::OBJECT && this->module->aliased_types.count(n.type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(n.type->object().id);
        n.type = aliased_type;
    } else {
        this->module->fill_actual(n.type);
    }
    USemanticInfo rvalue_sinfo = this->expect_rvalue_of_type(*n.type, *n.expression);
    if (rvalue_sinfo->is_error()) {
        return error_stub();
    }
    SemanticInfo info;
    info.snode = new DeclarationSNode(n.identifier, rvalue_sinfo->snode);
    auto* ov = new Value(n.type->clone());
    info.entity = Entity(ov);
    this->fill_value(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_without_type(DeclarationNode& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    if (exp_info_p->is_error()) {
        return error_stub();
    }
    E_TYPE entity_type = exp_info_p->entity.type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.expected_expression(exp_info_p->entity, *n.expression);
        return error_stub();
    }

    SemanticInfo info;
    info.snode = new DeclarationSNode(n.identifier, exp_info_p->snode);
    info.entity = exp_info_p->entity;
    if (info.entity.type == E_TYPE::CONST_FUNCTION) {
        info.entity = Entity(new Value(exp_info_p->entity.const_function->ft->clone()));

        if (info.entity.value->type->is_generic()) {
            this->error_reporter.fail("Error: you need to specialize the generic function of type " +
                                      info.entity.value->type->to_string() +
                                      " to be able to use it without calling it");
        }
    }
    return std::make_unique<SemanticInfo>(info);
}