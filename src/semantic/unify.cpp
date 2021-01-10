//
// Created by chris on 16/12/20.
//

#include "unify.h"
#include "../logging/logging.h"


std::pair<std::string, TypeNode*>*
get_first_substitution_object(ObjectTypeNode& a, ObjectTypeNode& b, bool is_top_level_arg) {
    if (is_variable(a) && is_variable(b) && a.object().identifier == b.object().identifier) {
        return nullptr;
    }
    if (is_variable(a)) {
        return new std::pair<std::string, TypeNode*>(a.object().identifier, b.clone());
    }
    if (is_variable(b)) {
        if (is_top_level_arg) {
            throw std::runtime_error("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, TypeNode*>(b.object().identifier, a.clone());
    }
    if (a.identifier != b.identifier) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    if (a.type_parameters.size() != b.type_parameters.size()) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    for (int i = 0; i < a.type_parameters.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.type_parameters[i],
                *b.type_parameters[i],
                is_top_level_arg
        );
        if (u != nullptr) {
            return u;
        }
    }
    return nullptr;
}

TypeNode* substitute(TypeNode* t, std::string var, TypeNode* replacement) {
    if (t->kind == Kind::OBJECT) {
        if (is_variable(t->object()) && t->object().identifier == var) {
            return replacement;
        } else {
            TypeNode* c = t->clone();
            for (int i = 0; i < t->object().type_parameters.size(); i++) {
                c->object().type_parameters[i] = substitute(t->object().type_parameters[i], var, replacement);
            }
            return c;
        }
    } else {
        TypeNode* c = t->clone();
        for (int i = 0; i < t->function().parameter_types.size(); i++) {
            c->function().parameter_types[i] = substitute(t->function().parameter_types[i], var, replacement);
        }
        c->function().return_type = substitute(c->function().return_type, var, replacement);
        return c;
    }
}


std::pair<std::string, TypeNode*>*
get_first_substitution_function(FunctionTypeNode& a, FunctionTypeNode& b, bool is_top_level_arg) {
    if (a.parameter_types.size() != b.parameter_types.size()) {
        throw std::runtime_error(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (int i = 0; i < a.parameter_types.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.parameter_types[i],
                *b.parameter_types[i],
                false
        );
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

std::string error_generic_call_mismatch(const TypeNode& expected, const TypeNode& actual, int i) {
    std::string msg =
            E_FMT("Error matching argument number " + std::to_string(i) + " expected ") + E_HLT(expected.to_string()) +
            E_FMT(" got ") + E_HLT(actual.to_string());
    return msg;
}

void unify_function_call(FunctionTypeNode& fun, VectorOfTypes& args) {
    if (args.size() != fun.parameter_types.size()) {
        throw std::runtime_error("Function call with wrong number of arguments!");
    }

    for (int i = 0; i < args.size(); i++) {
        auto param = fun.parameter_types[i];
        auto arg = args[i];
        try {
            std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
            while (substitution != nullptr) {
                for (int j = 0; j < args.size(); j++) {
                    // if (j == i) {
                    //     continue;
                    // }
                    fun.parameter_types[j] = substitute(
                            fun.parameter_types[j],
                            substitution->first,
                            substitution->second
                    );
                    args[j] = substitute(args[j], substitution->first, substitution->second);
                }
                fun.return_type = substitute(fun.return_type, substitution->first, substitution->second);
                std::cout << "Simple substitution: " << fun.to_string() << std::endl;
                param = fun.parameter_types[i];
                arg = args[i];
                substitution = get_first_substitution(*param, *arg, true);
            }
        } catch (...) {
            std::cout << error_generic_call_mismatch(*param, *arg, i);
            exit(1);
        }
    }
}

std::pair<std::string, TypeNode*>* get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg) {
    if (a.kind != b.kind) {
        throw std::runtime_error(
                "Error trying to unify types of different kind" + a.to_string() + " and " + b.to_string());
    }
    if (a.kind == Kind::FUNCTION) {
        return get_first_substitution_function(a.function(), b.function(), is_top_level_arg);
    } else {
        return get_first_substitution_object(a.object(), b.object(), is_top_level_arg);
    }
}

bool is_variable(const ObjectTypeNode& a) {
    return a.type_parameters.size() == 0 && islower(a.identifier[0]);
}

