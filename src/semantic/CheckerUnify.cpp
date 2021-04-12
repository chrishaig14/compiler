//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"

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
            throw std::runtime_error("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, TypeNode*>(b.object().id, a.clone());
    }
    if (a.id != b.id) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    if (a.type_params.size() != b.type_params.size()) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    for (int i = 0; i < a.type_params.size(); i++) {
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
            for (int i = 0; i < t->object().type_params.size(); i++) {
                // auto old = c->object().type_params[i];
                c->object().type_params[i] = substitute(t->object().type_params[i], var, replacement);
                // delete old;
            }
            return c;
        }
    } else {
        TypeNode* c = t->clone();
        for (int i = 0; i < t->function().param_types.size(); i++) {
            c->function().param_types[i] = substitute(t->function().param_types[i], var, replacement);
        }
        c->function().return_type = substitute(c->function().return_type, var, replacement);
        return c;
    }
}

std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg) {
    if (a.param_types.size() != b.param_types.size()) {
        throw std::runtime_error(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (int i = 0; i < a.param_types.size(); i++) {
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
        this->error_call_bad_num_args();
        return;
    }

    for (int i = 0; i < args.size(); i++) {
        auto param = fun.param_types[i];
        auto arg = args[i];
        try {
            std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
            while (substitution != nullptr) {
                for (int j = 0; j < args.size(); j++) {
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


        } catch (...) {
            this->error_generic_call_mismatch(*param, *arg, i);
        }
    }
}

std::pair<std::string, TypeNode*>* Checker::get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg) {
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

USemanticInfo Checker::visit_import(ImportNode& node) {
    SemanticInfo info;
    return std::make_unique<SemanticInfo>(info);

    std::string name = node.path.back();
    std::string path = node.path[0];
    int current_index = 0;
    while (true) {
        auto current_package = this->global_packages->find(path);
        if (current_package == this->global_packages->end()) {
            // might be a module(.function/.class)
            auto current_module = this->global_modules->find(path);
            if (current_module == this->global_modules->end()) {
                std::cout << "Error: module/package " << path << " not found!" << std::endl;
                throw std::runtime_error("Import error");
            } else {
                // module found!
                if (current_index == node.path.size() - 1) {
                    // imported whole module
                    break;
                }
                std::string final_part = node.path[current_index + 1];
                // auto final_import = current_module->second->local_paths.find(final_part);
                // if (final_import == current_module->second->local_paths.end()) {
                //     std::cout << "Error: name " << final_part << " not found in module " << path << std::endl;
                //     throw std::runtime_error("Import error");
                // } else {
                //     // imported function/class from module!
                //     break;
                // }
            }
        } else {
            // package found!
            if (current_index == node.path.size() - 1) {
                // ok, import whole package!
                break;
            } else {
                path = path + "." + node.path[current_index + 1];
                current_index++;
            }
        }
    }
    // SemanticInfo info;
    // return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::class_member(Class* cls, std::string child) {
    SemanticInfo info;
    if (cls->methods.find(child) != cls->methods.end()) {
        info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=cls->methods[child]};
    } else if (cls->static_methods.find(child) != cls->static_methods.end()) {
        info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=cls->static_methods[child]};
    } else if (cls->static_members.find(child) != cls->static_members.end()) {
        info.entity = entity_from_type(*cls->static_members[child].first);
    } else {
        throw std::runtime_error("Class " + cls->class_name + " has no method/member " + child);
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::object_member(SNode* object_snode, ObjectValue* pValue, std::string child) {
    Class* clazz = this->scope->get(pValue->ot->id).clazz;
    SemanticInfo info;
    if (clazz->members.count(child)) {
        info.entity = entity_from_type(*clazz->members[child]);
        ObjectMemberSNode* omn = new ObjectMemberSNode();
        omn->class_path = clazz->full_path;
        omn->object = object_snode;
        omn->member_name = child;
        info.snode = omn;
    } else if (clazz->methods.count(child)) {
        IdSNode* idn = new IdSNode();
        idn->identifier = clazz->full_path + "." + child;
        if (this->is_call) {
            // method call
            info.this_arg = object_snode;
            info.snode = idn;
        } else {
            // return partial
        }
        info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=clazz->methods[child]};

    } else {
        throw std::runtime_error("Error member/method '" + child + "' not found in class '" + clazz->class_name + "'");
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::package_member(Package* package, std::string child) {
    if (package->units.count(child) == 0) {
        throw std::runtime_error("Error: package " + package->name + " has no member " + child);
    }
    Unit unit = package->units[child];
    SemanticInfo info;
    info.entity = map_flirpin_to_entity(map_unit_to_flirpin(unit));
    return std::make_unique<SemanticInfo>(info);
}

Entity map_flirpin_to_entity(Flirpin flirpin) {
    switch (flirpin.type) {
        case F_TYPE::CONST_FUNCTION:
            return Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=flirpin.const_function};
        case F_TYPE::CLASS:
            return Entity{.type=E_TYPE::CLASS, .clazz=flirpin.clazz};
        case F_TYPE::PACKAGE:
            return Entity{.type=E_TYPE::PACKAGE, .package=flirpin.package};
        case F_TYPE::MODULE:
            return Entity{.type=E_TYPE::MODULE, .module=flirpin.module};
    }
}

USemanticInfo Checker::module_member(Module* mod, std::string child) {
    if (mod->flirpins.count(child) == 0) {
        throw std::runtime_error("Error: module " + mod->name + " has no member " + child);
    }
    Flirpin flirpin = mod->flirpins[child];
    SemanticInfo info;
    info.entity = map_flirpin_to_entity(flirpin);
    if (flirpin.type == F_TYPE::CONST_FUNCTION) {
        IdSNode* idn = new IdSNode();
        idn->identifier = flirpin.const_function->full_path;
        info.snode = idn;
    }
    return std::make_unique<SemanticInfo>(info);
}
