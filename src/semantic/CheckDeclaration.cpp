//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"

SNode* Checker::make_rvalue(Entity value_entity, SNode* value_snode, const TypeNode& target) {
    if (value_entity.type == E_TYPE::OBJECT_VALUE) {
        if (target.kind != Kind::OBJECT) {
            return nullptr;
        }
        const ObjectType& value_ot = *value_entity.object_value->ot;
        const ObjectType& target_ot = target.object();
        if (value_ot == target_ot) {
            return value_snode;
        }
        if (target_ot.id == "Union") {
            int union_index = target_union_type(target_ot, value_ot);
            if (union_index == -1) {
                return nullptr;
            } else {
                return make_union_wrapper(union_index, value_snode);
            }
        }
    } else {
        // kind = FUNCTION
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

    USemanticInfo exp_info_p = this->dispatch(n.expression);

    E_TYPE entity_type = exp_info_p->entity.type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::FUNCTION_VALUE &&
        entity_type != E_TYPE::OBJECT_VALUE) {
        this->error_reporter.expected_expression_with_type(exp_info_p->entity, *n.type, n.start);
    }

    sn->expression = exp_info_p->snode;
    SemanticInfo& exp_info = *exp_info_p;
    if (exp_info.entity.type == E_TYPE::ERROR) {
        ObjectValue* ov = new ObjectValue();
        info.entity = Entity(ov);
        ov->ot = (ObjectType*) n.type->clone();
        return std::make_unique<SemanticInfo>(info);
    }

    SNode* rvalue_snode = this->make_rvalue(exp_info_p->entity, exp_info_p->snode, *n.type);
    if (rvalue_snode == nullptr) {
        throw std::runtime_error("Cannot assign!");
    }
    sn->expression = rvalue_snode;
    ObjectValue* ov = new ObjectValue();
    info.entity = Entity(ov);
    ov->ot = (ObjectType*) n.type->clone();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_without_type(DeclarationNode& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    if (exp_info_p->entity.type == E_TYPE::ERROR) {
        std::cout << "Ignoring all subsequenct error involving variable " + n.identifier + " as type cannot be inferred"
                  << std::endl;
        return error_stub();
    }
    E_TYPE entity_type = exp_info_p->entity.type;
    if (entity_type != E_TYPE::CONST_FUNCTION && entity_type != E_TYPE::FUNCTION_VALUE &&
        entity_type != E_TYPE::OBJECT_VALUE) {
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
        info.entity = Entity(new FunctionValue());
        info.entity.function_value->ft = exp_info_p->entity.const_function->ft;

        if (info.entity.function_value->ft->is_generic()) {
            throw std::runtime_error("Error: you need to specialize the generic function of type " +
                                     info.entity.function_value->ft->to_string() +
                                     " to be able to use it without calling it");
        }
    }
    return std::make_unique<SemanticInfo>(info);
}