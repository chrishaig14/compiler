//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"

SNode* Checker::make_rvalue(Entity value_entity, SNode* value_snode, const TypeNode& target) {
    if (value_entity.type == E_TYPE::VALUE) {
        if (value_entity.value->type->kind != target.kind){
            return nullptr;
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

        if (unaliased_target_type->object().id == "Union") {
            int union_index = target_union_type(unaliased_target_type->object(), unaliased_value_type->object());
            if (union_index != -1) {
                return make_union_wrapper(union_index, value_snode);
            } else {
                if (unaliased_value_type->object().id == "Union") {
                    if (unaliased_value_type->object().type_params.size() <=
                        unaliased_target_type->object().type_params.size()) {
                        // might be assigning a Union[Int, Str] to a Union[Int, Str, Bool] which should work!
                        // for now the types order must be the same, but it should be necessary
                        for (int i = 0; i < unaliased_value_type->object().type_params.size(); i++) {
                            if (*unaliased_value_type->object().type_params[i] !=
                                *unaliased_target_type->object().type_params[i]) {
                                this->error_reporter.fail(
                                        "Error: Cannot lift union type " + unaliased_value_type->to_string() + " to " +
                                        unaliased_target_type->to_string());
                            }
                        }
                        return value_snode;
                    }
                }
                return nullptr;
            }
        }

        if (unaliased_target_type->object().id == "Option") {
            if (*unaliased_target_type->object().type_params[0] == *unaliased_value_type) {
                return value_snode;
            } else if (unaliased_value_type->object().id == "NoneType") {
                return value_snode;
            } else {
                return nullptr;
            }
        }

    } else {
        // this->error_reporter.fail("MAKE RVALUE OF FUNCTION!");
        return value_snode;
    }
    return nullptr;
}

USemanticInfo Checker::visit_declaration(DeclarationNode& n) {
    // Logger::info("Checking DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier)) {
        this->error_reporter.redeclared(n.identifier, n.start);
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
    SemanticInfo info;
    DeclarationSNode* sn = new DeclarationSNode();
    info.snode = sn;
    sn->identifier = n.identifier;

    TypeNode* orig_type = n.type;

    if (n.type->kind == Kind::OBJECT && this->module->aliased_types.count(n.type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(n.type->object().id);
        n.type = aliased_type;
    } else {
        this->module->fill_actual(n.type);
    }

    USemanticInfo exp_info_p = this->dispatch_rvalue(n.expression);

    E_TYPE entity_type = exp_info_p->entity.type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.expected_expression_with_type(exp_info_p->entity, *n.type, n.start);
    }

    sn->expression = exp_info_p->snode;
    SemanticInfo& exp_info = *exp_info_p;
    if (exp_info.entity.type == E_TYPE::ERROR) {
        Value* ov = new Value(n.type->clone());
        info.entity = Entity(ov);
        return std::make_unique<SemanticInfo>(info);
    }

    SNode* rvalue_snode = this->make_rvalue(exp_info_p->entity, exp_info_p->snode, *n.type);
    if (rvalue_snode == nullptr) {
        this->error_reporter.fail("Cannot assign!");
    }
    sn->expression = rvalue_snode;
    Value* ov = new Value(n.type->clone());
    info.entity = Entity(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_without_type(DeclarationNode& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    if (exp_info_p->entity.type == E_TYPE::ERROR) {
        std::cout
                << "Ignoring all subsequent errors involving variable '" + n.identifier + "' as type cannot be inferred"
                << std::endl;
        return error_stub();
    }
    E_TYPE entity_type = exp_info_p->entity.type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::VALUE) {
        this->error_reporter.expected_expression(exp_info_p->entity, n.start);
        return error_stub();
    }
    SemanticInfo info;
    DeclarationSNode* sn = new DeclarationSNode();
    info.snode = sn;
    sn->identifier = n.identifier;
    sn->expression = exp_info_p->snode;
    info.entity = exp_info_p->entity;
    if (info.entity.type == E_TYPE::CONST_FUNCTION) {
        info.entity = Entity(new Value(exp_info_p->entity.const_function->ft->clone()));

        if (info.entity.value->type->is_generic()) {
            this->error_reporter.fail("Error: you need to specialize the generic function of type " +
                                     info.entity.value->type->to_string() + " to be able to use it without calling it");
        }
    }
    // this->fill_value(info.entity.value);
    return std::make_unique<SemanticInfo>(info);
}