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
#include "errors/ErrorFunctionCallNumArgs.h"
#include "errors/ErrorEnumNoValue.h"

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

UTypeNode Checker::substitute(const TypeNode& t, const std::string& var, const TypeNode& replacement) {
    if (t.kind == Kind::OBJECT) {
        if (is_variable(t.object()) && t.object().id == var) {
            return UTypeNode(replacement.clone());
        } else {
            TypeNode* c = t.clone();
            for (size_t i = 0; i < t.object().type_params.size(); i++) {
                // auto old = c->object().type_params[i];
                c->object().type_params[i] = substitute(*t.object().type_params[i], var, replacement).release();
                // delete old;
            }
            return UTypeNode(c);
        }
    } else {
        TypeNode* c = t.clone();
        for (size_t i = 0; i < t.function().param_types.size(); i++) {
            c->function().param_types[i] = substitute(*t.function().param_types[i], var, replacement).release();
        }
        c->function().return_type = substitute(*c->function().return_type, var, replacement).release();
        return UTypeNode(c);
    }
    return nullptr;
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

std::unique_ptr<FunctionType> Checker::unify_function_call(const FunctionType& f, VectorOfTypes& args,
                                                           std::map<std::string, TypeNode*>& all_substitutions) {
    FunctionType& fun = *f.clone();
    if (args.size() != fun.param_types.size()) {
        this->error_reporter.error(ErrorFunctionCallNumArgs(fun, {1, 1}));
        return nullptr;
    }


    for (size_t i = 0; i < args.size(); i++) {
        auto* param = fun.param_types[i];
        auto* arg = args[i];
        std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
        while (substitution != nullptr) {
            for (size_t j = 0; j < args.size(); j++) {
                // if (j == i) {
                //     continue;
                // }
                auto* old = fun.param_types[j];
                fun.param_types[j] = substitute(*fun.param_types[j],
                                                substitution->first,
                                                *substitution->second).release();
                delete old;
                old = args[j];
                args[j] = substitute(*args[j], substitution->first, *substitution->second).release();
                all_substitutions[substitution->first] = substitution->second->clone();
                delete old;
            }
            auto* old = fun.return_type;
            fun.return_type = substitute(*fun.return_type, substitution->first, *substitution->second).release();
            delete old;
            std::cout << "Simple substitution: " << fun.to_string() << std::endl;
            param = fun.param_types[i];
            arg = args[i];
            auto* old_s = substitution;
            substitution = get_first_substitution(*param, *arg, true);
            delete old_s->second;
            delete old_s;
        }
    }
    return std::unique_ptr<FunctionType>(&fun);
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

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    return info_u;
}

Entity* map_flirpin_to_entity(Flirpin flirpin) {
    switch (flirpin.type) {
        case F_TYPE::CONST_FUNCTION:
            return new EntityConstFunction(flirpin.const_function);
        case F_TYPE::CLASS:
            return new EntityClass(flirpin.clazz);
        case F_TYPE::PACKAGE:
            return new EntityPackage(flirpin.package);
        case F_TYPE::MODULE:
            return new EntityModule(flirpin.module);
        case F_TYPE::ENUM:
            return new EntityEnum(flirpin.enumm);
    }
    return nullptr;
}


USemanticInfo Checker::visit_alias(AliasNode& p_node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    return info_u;
}

USemanticInfo Checker::enum_member(Enum* enumm, const std::string& value, MemberNode& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    for (size_t i = 0; i < enumm->values.size(); i++) {
        if (value == enumm->values[i]) {
            auto* otype = new ObjectType(enumm->enumm_name, {});
            otype->actual_base_path = enumm->path;
            auto ov = std::make_unique<Value>(otype);
            info.entity = *new EntityValue(std::move(ov));
            // this->fill_value(info.entity.value);
            info.snode = new EnumMemberSNode(enumm->path.as_str(), value);
            return info_u;
        }
    }
    this->error_reporter.error(ErrorEnumNoValue(enumm->enumm_name, value, node, enumm));
    return error_stub();
}


