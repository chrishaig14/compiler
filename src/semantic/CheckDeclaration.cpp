//
// Created by chris on 2/5/21.
//

#include "CheckDeclaration.h"

USemanticInfo Checker::visit_declaration(DeclarationNode& n) {
    Logger::info("Checking DeclarationNode for var: " + n.identifier);
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
    if (n.type->kind == Kind::FUNCTION) {
        // it's a function
        // if (*n.type != exp_info.type()) {
        //     this->error_reporter.assignment(*n.type, exp_info.type(), n.start);
        // }
    } else {
        SemanticInfo expression_info = exp_info;
        const ObjectType& actual_type = n.type->object();
        const TypeNode& exp_type = *expression_info.entity.object_value->ot;
        if (*n.type != exp_type) {
            if (actual_type.id == "Option") {
                if (*actual_type.type_params[0] != exp_type) {
                    auto foo = exp_type.object();
                    if (foo.id != "NoneType") {
                        this->error_reporter.assignment(*n.type, exp_type, n.start);
                    }
                }
            } else if (actual_type.id == "Union") {
                int type_index = target_union_type(actual_type, exp_type);
                if (type_index == -1) {
                    this->error_reporter.assignment(*orig_type, exp_type, n.start);
                }
                SNode* union_wrapper = make_union_wrapper(type_index, sn->expression);
                sn->expression = union_wrapper;

            } else {
                this->error_reporter.assignment(*n.type, exp_type, n.start);
            }
        }
    }
    // info.set_type(n_type);
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
    // if (exp_info_p->type() == T_NONE) {
    //     this->error_reporter.function_doesnt_return_a_value(n.expression->start, nullptr);
    //     USemanticInfo error_t = error_stub();
    //     this->scope->set(n.identifier, entity_from_type(error_t->type()));
    //     return error_t;
    // }
    // n.type = exp_info_p->type().clone();
    // info.set_type(exp_info.type());
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