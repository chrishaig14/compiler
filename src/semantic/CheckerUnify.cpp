//
// Created by chris on 17/1/21.
//

#include "ModuleChecker.h"
#include "../simple_nodes/common/include/Match.h"
#include "../simple_nodes/expressions/include/NewObject.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/include/TypeFunction.h"
#include "../simple_nodes/expressions/include/EnumMember.h"
#include "../simple_nodes/expressions/include/Id.h"
#include "../simple_nodes/expressions/include/ObjectMember.h"

std::pair<std::string, ast::Type*>*
ModuleChecker::get_first_substitution_object(ast::ObjectType& a, ast::ObjectType& b, bool is_top_level_arg) {
    if (is_variable(a) && is_variable(b) && a.object().id == b.object().id) {
        return nullptr;
    }
    if (is_variable(a)) {
        return new std::pair<std::string, ast::Type*>(a.object().id, b.clone());
    }
    if (is_variable(b)) {
        if (is_top_level_arg) {
            this->error_reporter.fail("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, ast::Type*>(b.object().id, a.clone());
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
        std::pair<std::string, ast::Type*>* u = get_first_substitution(*a.type_params[i],
                                                                       *b.type_params[i],
                                                                       is_top_level_arg);
        if (u != nullptr) {
            return u;
        }
    }
    return nullptr;
}

ast::UTypeNode ModuleChecker::substitute(const ast::Type& t, const std::string& var, const ast::Type& replacement) {
    if (t.kind == Kind::OBJECT) {
        if (is_variable(t.object()) && t.object().id == var) {
            return ast::UTypeNode(replacement.clone());
        } else {
            ast::Type* c = t.clone();
            for (size_t i = 0; i < t.object().type_params.size(); i++) {
                // auto old = c->object().type_params[i];
                c->object().type_params[i] = substitute(*t.object().type_params[i], var, replacement).release();
                // delete old;
            }
            return ast::UTypeNode(c);
        }
    } else {
        ast::Type* c = t.clone();
        for (size_t i = 0; i < t.function().param_types.size(); i++) {
            c->function().param_types[i] = substitute(*t.function().param_types[i], var, replacement);
        }
        c->function().return_type = substitute(*c->function().return_type, var, replacement);
        return ast::UTypeNode(c);
    }
    return nullptr;
}

std::pair<std::string, ast::Type*>*
ModuleChecker::get_first_substitution_function(ast::FunctionType& a, ast::FunctionType& b, bool is_top_level_arg) {
    if (a.param_types.size() != b.param_types.size()) {
        this->error_reporter.fail(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (size_t i = 0; i < a.param_types.size(); i++) {
        std::pair<std::string, ast::Type*>* u = get_first_substitution(*a.param_types[i], *b.param_types[i], false);
        if (u != nullptr) {
            return u;
        }
    }
    std::pair<std::string, ast::Type*>* u = get_first_substitution(*a.return_type, *b.return_type, false);
    if (u != nullptr) {
        return u;
    }
    return nullptr;
}

std::unique_ptr<ast::FunctionType>
ModuleChecker::unify_function_call(const ast::FunctionType& f, ast::VectorOfTypes& args,
                                   std::map<std::string, ast::Type*>& all_substitutions) {
    ast::FunctionType& fun = *f.clone();
    if (args.size() != fun.param_types.size()) {
        this->error_reporter.error(std::make_unique<ErrorFunctionCallNumArgs>((sem::TypeFunction*) fun.to_sem(),
                                                                              TextPosition{1, 1}));
        return nullptr;
    }


    for (size_t i = 0; i < args.size(); i++) {
        auto& param = fun.param_types[i];
        auto* arg = args[i];
        std::pair<std::string, ast::Type*>* substitution = get_first_substitution(*param, *arg, true);
        while (substitution != nullptr) {
            for (size_t j = 0; j < args.size(); j++) {
                // if (j == i) {
                //     continue;
                // }
                // auto& old = fun.param_types[j];
                fun.param_types[j] = substitute(*fun.param_types[j], substitution->first, *substitution->second);
                // delete old;
                // old = std::move(args[j]->clone());
                args[j] = substitute(*args[j], substitution->first, *substitution->second).release();
                all_substitutions[substitution->first] = substitution->second->clone();
                // delete old;
            }
            fun.return_type = substitute(*fun.return_type, substitution->first, *substitution->second);
            arg = args[i];
            auto* old_s = substitution;
            substitution = get_first_substitution(*fun.param_types[i], *arg, true);
            delete old_s->second;
            delete old_s;
        }
    }
    return std::unique_ptr<ast::FunctionType>(&fun);
}

std::pair<std::string, ast::Type*>*
ModuleChecker::get_first_substitution(ast::Type& a, ast::Type& b, bool is_top_level_arg) {
    if (a.kind == Kind::FUNCTION && b.kind == Kind::OBJECT) {
        this->error_reporter.fail(
                "Error trying to unify types of different kind" + a.to_string() + " and " + b.to_string());
    }
    if (a.kind == Kind::OBJECT && b.kind == Kind::FUNCTION) {
        if (!a.is_generic_param) {
            this->error_reporter.fail("Error trying to unify " + a.to_string() + " and " + b.to_string());
        }
        return new std::pair<std::string, ast::Type*>(a.object().id, b.clone());
    }
    if (a.kind == Kind::FUNCTION) {
        return get_first_substitution_function(a.function(), b.function(), is_top_level_arg);
    } else {
        return get_first_substitution_object(a.object(), b.object(), is_top_level_arg);
    }
}

sem::UCommon ModuleChecker::visit_import(ast::Import& node) {
    sem::UCommon info_u;
    return info_u;
}

std::unique_ptr<Entity> map_module_member_to_entity(ModuleMember& module_member) {
    if (module_member.is_const_function()) {
        return std::make_unique<EntityConstFunction>(module_member.const_function());
    } else if (module_member.is_package()) {
        return std::make_unique<EntityPackage>(module_member.package());
    } else if (module_member.is_module()) {
        return std::make_unique<EntityModule>(module_member.module());
    } else if (module_member.is_klass()) {
        return std::make_unique<EntityClass>(module_member.klass());
    } else if (module_member.is_enumm()) {
        return std::make_unique<EntityEnum>(module_member.enumm());
    } else if (module_member.is_typeclass()) {
        return std::make_unique<EntityTypeclass>(module_member.typeclass());
    }
    throw std::runtime_error("Error: returning nullptr for map_module_member_to_entity");
    return nullptr;
}


sem::UCommon ModuleChecker::visit_alias(ast::Alias& p_node) {
    sem::UCommon info_u;
    return info_u;
}

UExpressionInfo ModuleChecker::enum_member(ast::Member& node, Enum& enumm) {
    std::string value = node.s_child;
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    for (size_t i = 0; i < enumm.values.size(); i++) {
        if (value == enumm.values[i]) {
            auto* otype = new sem::TypeObject(enumm.enumm_name, enumm.path);
            info.set_entity(std::make_unique<EntityValue>(otype, &enumm));
            // this->fill_value(info.entity.value);
            info.exp_snode = std::make_unique<sem::EnumMember>(enumm.path, value);
            return info_u;
        }
    }
    this->error_reporter.error(std::make_unique<ErrorEnumNoValue>(enumm.enumm_name, value, node, enumm));
    return exp_error_stub();
}

std::unique_ptr<Entity> ModuleChecker::entity_from_type(const ast::Type& type) {
    if (this->entities.count(type.to_string()) == 1) {
        return std::unique_ptr<Entity>(this->entities[type.to_string()]->clone());
    }
    if (type.kind == Kind::FUNCTION) {
        auto fv = EntityValue::function_value(type.to_sem());
        this->entities[type.to_string()] = std::unique_ptr<Entity>(fv->clone());
        return fv;
    }
    if (type.kind == Kind::OBJECT) {
        if (type.object().id == ".None") {
            this->entities[type.to_string()] = std::make_unique<EntityNothing>();
            return std::make_unique<EntityNothing>();
        }
    }
    auto fv = std::make_unique<EntityValue>(type.to_sem(), (ConcreteClass*) nullptr);
    this->entities[type.to_string()] = std::unique_ptr<Entity>(fv->clone());
    return fv;
}

UExpressionInfo ModuleChecker::value_member(ast::Member& n, UExpressionInfo parent_info, EntityValue& value) {
    if (value.type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.error(std::make_unique<ErrorNoMember>(value.get_constfun().const_function.const_function_ft,
                                                                   n));
        return exp_error_stub();
    }
    return this->object_member(std::move(parent_info->exp_snode), value, n.s_child, n);
}

UExpressionInfo
ModuleChecker::const_function_member(ast::Member& n, UExpressionInfo unique_ptr_1, ConstFunction& function) {
    this->error_reporter.error(std::make_unique<ErrorNoMember>(function.const_function_ft, n));
    return exp_error_stub();
}

std::unique_ptr<sem::Top> ModuleChecker::visit_typeclass(ast::TypeclassAst& typeclass) {
    return std::unique_ptr<sem::Top>();
}

void ModuleChecker::add_typeclasses_to_generic_type(sem::Type& type, std::string gen_type, std::string typeclass_name) {
    if (type.kind == sem::Kind::OBJECT and type.object().id.size() == 1) {
        if (type.object().id == gen_type) {
            type.object().add_typeclass(typeclass_name);
        }
    }
}
