//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../units/FunctionValue.h"

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
        throw std::runtime_error(
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
            this->error_reporter.generic_call_mismatch(*param, *arg, i);
        }
    }
}

std::pair<std::string, TypeNode*>* Checker::get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg) {
    if (a.kind == Kind::FUNCTION && b.kind == Kind::OBJECT) {
        throw std::runtime_error(
                "Error trying to unify types of different kind" + a.to_string() + " and " + b.to_string());
    }
    if (a.kind == Kind::OBJECT && b.kind == Kind::FUNCTION) {
        if (!a.is_generic_param) {
            throw std::runtime_error("Error trying to unify " + a.to_string() + " and " + b.to_string());
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

USemanticInfo Checker::class_member(Class* cls, std::string child, MemberNode& n) {
    SemanticInfo info;
    if (cls->methods.find(child) != cls->methods.end()) {
        ConstFunction* bound_method = cls->methods[child];
        ConstFunction* unbound_method = new ConstFunction();
        unbound_method->path = bound_method->path;
        unbound_method->ft = bound_method->ft->clone();
        unbound_method->ft->param_types.insert(unbound_method->ft->param_types.begin(),
                                               new ObjectType(cls->class_name));
        info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=unbound_method};
        info.snode = new IdSNode(unbound_method->path.as_str());
    } else if (cls->static_methods.find(child) != cls->static_methods.end()) {
        info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=cls->static_methods[child]};
        info.snode = new IdSNode(cls->static_methods[child]->path.as_str());
    } else if (cls->static_members.find(child) != cls->static_members.end()) {
        info.entity = entity_from_type(*cls->static_members[child].first);
    } else {
        this->error_reporter.class_no_member(ObjectType(cls->class_name, {}), child, n.dot_pos);
        return error_stub();
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::object_member(SNode* object_snode, ObjectValue* pValue, std::string child, MemberNode& n) {
    if (pValue->ot->actual_base_path.as_str() == "") {
        // is a single type param, error
        throw std::runtime_error("Error: no member " + child + " in totally generic type " + pValue->ot->to_string());
    }
    Class* clazz = this->root_package->get(pValue->ot->actual_base_path).clazz;
    if (clazz->type_params.size() != 0) {
        clazz = instantiate_generic(clazz, *pValue->ot);
    }
    SemanticInfo info;
    if (clazz->members.count(child)) {
        info.entity = entity_from_type(*clazz->members[child]);
        ObjectMemberSNode* omn = new ObjectMemberSNode();
        omn->class_path = clazz->path;
        omn->object = object_snode;
        omn->member_name = child;
        info.snode = omn;
    } else if (clazz->methods.count(child)) {
        IdSNode* idn = new IdSNode();
        idn->identifier = Path(clazz->path, child).as_str();
        if (this->is_call) {
            // method call
            info.this_arg = object_snode;
            info.snode = idn;
            info.entity = Entity{.type=E_TYPE::CONST_FUNCTION, .const_function=clazz->methods[child]};
        } else {
            // return partial
            int npartial = clazz->methods[child]->ft->param_types.size();
            NewObjectSNode* non = new NewObjectSNode();
            non->class_name = "Partial" + std::to_string(npartial);
            IdSNode* method_snode = new IdSNode(clazz->methods[child]->path.as_str());
            non->args = {method_snode, object_snode};
            for (int i = 0; i < npartial; i++) {
                non->args.push_back(nullptr);
            }
            info.snode = non;
            FunctionValue* fv = new FunctionValue();
            fv->ft = clazz->methods[child]->ft->clone();
            info.entity = Entity{.type=E_TYPE::FUNCTION_VALUE, .function_value=fv};
        }

    } else {
        this->error_reporter.object_no_member(*pValue->ot, child, n.dot_pos);
        return error_stub();
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::package_member(Package* package, std::string child, MemberNode& n) {
    if (package->units.count(child) == 0) {
        this->error_reporter.package_no_member(package->path.as_str(), child, n.dot_pos);
        return error_stub();
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

USemanticInfo Checker::module_member(Module* mod, std::string child, MemberNode& n) {
    if (mod->flirpins.count(child) == 0) {
        this->error_reporter.module_no_member(mod->path.as_str(), child, n.dot_pos);
        return error_stub();
    }
    Flirpin flirpin = mod->flirpins[child];
    SemanticInfo info;
    info.entity = map_flirpin_to_entity(flirpin);
    if (flirpin.type == F_TYPE::CONST_FUNCTION) {
        IdSNode* idn = new IdSNode();
        idn->identifier = flirpin.const_function->path.as_str();
        info.snode = idn;
    }
    return std::make_unique<SemanticInfo>(info);
}
