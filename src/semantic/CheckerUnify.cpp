//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/MatchSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../units/FunctionValue.h"
#include "../simple_nodes/EnumMemberSNode.h"

std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_object(ObjectType& a, ObjectType& b, bool is_top_level_arg) {
    if (is_variable(a) && is_variable(b) && a.object().id == b.object().id) {
        return nullptr;
    }
    if (is_variable(a)) {
        return new std::pair<std::string, TypeNode*>(a.object().id, b.clone());
    }
    if (is_variable(b)) {
        if (is_top_level_arg) {
            this->error_reporter.fail("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, TypeNode*>(b.object().id, a.clone());
    }
    if (a.id != b.id) {
        throw std::runtime_error("Error trying to unify object types" + a.to_string() + " and " + b.to_string());
        // this->error_reporter.fail("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    if (a.type_params.size() != b.type_params.size()) {
        throw std::runtime_error("Error trying to unify object types" + a.to_string() + " and " + b.to_string());
        // this->error_reporter.fail("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    for (size_t i = 0; i < a.type_params.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(*a.type_params[i],
                                                                      *b.type_params[i],
                                                                      is_top_level_arg);
        if (u != nullptr) {
            return u;
        }
    }
    return nullptr;
}

TypeNode* Checker::substitute(TypeNode* t, std::string var, TypeNode* replacement) {
    if (t->kind == Kind::OBJECT) {
        if (is_variable(t->object()) && t->object().id == var) {
            return replacement->clone();
        } else {
            TypeNode* c = t->clone();
            for (size_t i = 0; i < t->object().type_params.size(); i++) {
                // auto old = c->object().type_params[i];
                c->object().type_params[i] = substitute(t->object().type_params[i], var, replacement);
                // delete old;
            }
            return c;
        }
    } else {
        TypeNode* c = t->clone();
        for (size_t i = 0; i < t->function().param_types.size(); i++) {
            c->function().param_types[i] = substitute(t->function().param_types[i], var, replacement);
        }
        c->function().return_type = substitute(c->function().return_type, var, replacement);
        return c;
    }
}

std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg) {
    if (a.param_types.size() != b.param_types.size()) {
        this->error_reporter.fail(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (size_t i = 0; i < a.param_types.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(*a.param_types[i], *b.param_types[i], false);
        if (u != nullptr) {
            return u;
        }
    }
    std::pair<std::string, TypeNode*>* u = get_first_substitution(*a.return_type, *b.return_type, false);
    if (u != nullptr) {
        return u;
    }
    return nullptr;
}

void Checker::unify_function_call(FunctionType& fun, VectorOfTypes& args) {
    if (args.size() != fun.param_types.size()) {
        this->error_reporter.call_bad_num_args();
        return;
    }

    for (size_t i = 0; i < args.size(); i++) {
        auto param = fun.param_types[i];
        auto arg = args[i];
        std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
        while (substitution != nullptr) {
            for (size_t j = 0; j < args.size(); j++) {
                // if (j == i) {
                //     continue;
                // }
                auto old = fun.param_types[j];
                fun.param_types[j] = substitute(fun.param_types[j], substitution->first, substitution->second);
                delete old;
                old = args[j];
                args[j] = substitute(args[j], substitution->first, substitution->second);
                delete old;
            }
            auto old = fun.return_type;
            fun.return_type = substitute(fun.return_type, substitution->first, substitution->second);
            delete old;
            std::cout << "Simple substitution: " << fun.to_string() << std::endl;
            param = fun.param_types[i];
            arg = args[i];
            auto old_s = substitution;
            substitution = get_first_substitution(*param, *arg, true);
            delete old_s->second;
            delete old_s;
        }
    }
}

std::pair<std::string, TypeNode*>* Checker::get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg) {
    if (a.kind == Kind::FUNCTION && b.kind == Kind::OBJECT) {
        this->error_reporter.fail(
                "Error trying to unify types of different kind" + a.to_string() + " and " + b.to_string());
    }
    if (a.kind == Kind::OBJECT && b.kind == Kind::FUNCTION) {
        if (!a.is_generic_param) {
            this->error_reporter.fail("Error trying to unify " + a.to_string() + " and " + b.to_string());
        }
        return new std::pair<std::string, TypeNode*>(a.object().id, b.clone());
    }
    if (a.kind == Kind::FUNCTION) {
        return get_first_substitution_function(a.function(), b.function(), is_top_level_arg);
    } else {
        return get_first_substitution_object(a.object(), b.object(), is_top_level_arg);
    }
}

USemanticInfo Checker::visit_import(ImportNode& node) {

    SemanticInfo info;
    return std::make_unique<SemanticInfo>(info);
}

Entity map_flirpin_to_entity(Flirpin flirpin) {
    switch (flirpin.type) {
        case F_TYPE::CONST_FUNCTION:
            return Entity(flirpin.const_function);
        case F_TYPE::CLASS:
            return Entity(flirpin.clazz);
        case F_TYPE::PACKAGE:
            return Entity(flirpin.package);
        case F_TYPE::MODULE:
            return Entity(flirpin.module);
        case F_TYPE::ENUM:
            return Entity(flirpin.enumm);
    }
}


USemanticInfo Checker::visit_alias(AliasNode* pNode) {
    SemanticInfo info;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::enum_member(Enum* enumm, std::string value, MemberNode& node) {
    SemanticInfo info;
    for (size_t i = 0; i < enumm->values.size(); i++) {
        if (value == enumm->values[i]) {
            ObjectType* otype = new ObjectType(enumm->enumm_name, {});
            otype = otype;
            otype->actual_base_path = enumm->path;
            Value* ov = new Value(otype);
            info.entity = Entity(ov);
            // this->fill_value(info.entity.value);
            info.snode = new EnumMemberSNode(enumm->path.as_str(), value);
            return std::make_unique<SemanticInfo>(info);
        }
    }
    this->error_reporter.enum_no_value(enumm->enumm_name, value, node, enumm);
    return error_stub();
}


