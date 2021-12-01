//
// Created by chris on 6/4/21.
//

#include "Module.h"
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <simple_nodes/common/include/Type.h>
#include <ast/general/ObjectType.h>

Module::Module(Path path, std::string abs_path, bool is_lib)
        : name(path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), path(path) {
}

ModuleMember* Module::get(Path p) {
    VectorOfStrings pt = p.as_vec();
    return this->members.at(pt.back()).get();
}

void Module::add_class_definition(std::unique_ptr<ConcreteClass> p_class) {
    this->members[p_class->class_name] = std::make_unique<ClassModuleMember>(*p_class);
    this->classes.push_back(std::move(p_class));
}

void Module::add_template_class_definition(std::unique_ptr<TemplateClassInfo> p_class) {
    this->members[p_class->class_name] = std::make_unique<TemplateClassModuleMember>(*p_class);
    this->template_classes.push_back(std::move(p_class));
}


void Module::add_enum_definition(std::unique_ptr<Enum> enumm) {
    this->members[enumm->enumm_name] = std::make_unique<EnumModuleMember>(enumm.get());
    this->enums.push_back(std::move(enumm));
}

void Module::add_func_definition(const ConstFunction& const_function) {
    this->members[const_function.path.basname()] = std::make_unique<ConstFunctionModuleMember>(const_function);
    this->const_functions.push_back(const_function);
}

Path Module::get_actual_path(const std::string& id) {
    if (id == ".None") {
        return Path(VectorOfStrings({".None"}));
    }
    if (id == "Union") {
        return Path("libcore.libcore.Union");
    }
    if (id == "Tuple") {
        return Path("libcore.libcore.Tuple");
    }
    if (this->members.count(id) == 1) {
        if (this->members[id]->is_klass()) {
            return this->members[id]->klass().path;
        } else if (this->members[id]->is_enumm()) {
            return this->members[id]->enumm().path;
        } else if (this->members[id]->is_template_klass()) {
            return this->members[id]->template_klass().path;
        }
    }
    if (this->imported_paths_with_alias.count(id) == 1) {
        return this->imported_paths_with_alias[id];
    }
    if (this->imported_paths_no_alias.count(id) == 1) {
        return this->imported_paths_no_alias[id];
    }
    throw std::runtime_error("Error: type " + id + " not found");
}

// void Module::fill_actual(ast::Type& t) {
//     if (t.kind == Kind::OBJECT) {
//         // if (this->aliased_types.count(t.object().id) != 0) {
//         //     t.object().data.aliased_type = this->aliased_types[t.object().id];
//         //     return;
//         // }
//         this->fill_actual(t.object());
//     }
//     if (t.kind == Kind::FUNCTION) {
//         this->fill_actual(t.function());
//         return;
//     }
// }
//
// void Module::fill_actual(ast::ObjectType& t) {
//     if (t.id.size() == 1) {
//         return;
//     }
//     if (t.is_generic_param) {
//         return;
//     }
//     t.data.actual_base_path = this->get_actual_path(t.id);
//     for (auto* tp: t.type_params) {
//         this->fill_actual(*tp);
//     }
// }
//
// void Module::fill_actual(ast::FunctionType& t) {
//     for (auto& pt: t.param_types) {
//         this->fill_actual(*pt);
//     }
//     this->fill_actual(*t.return_type);
// }

void Module::fill_actual(sem::Type& t) {
    if (t.kind == sem::Kind::OBJECT) {
        if (this->aliased_types.count(t.object().id) != 0) {
            t.object().data.aliased_type = this->aliased_types[t.object().id]->to_sem();
            return;
        }
        this->fill_actual(t.object());
    }
    if (t.kind == sem::Kind::FUNCTION) {
        this->fill_actual(t.function());
        return;
    }
}

void Module::fill_actual(sem::TypeObject& t) {
    if (t.id.size() == 1) {
        return;
    }
    if (t.is_generic_param) {
        return;
    }
    t.data.actual_base_path = this->get_actual_path(t.id);
    for (auto* tp: t.type_params) {
        this->fill_actual(*tp);
    }
}

void Module::fill_actual(sem::TypeFunction& t) {
    for (auto& pt: t.param_types) {
        this->fill_actual(*pt);
    }
    this->fill_actual(*t.return_type);
}

void Module::add_typeclass_definition(std::unique_ptr<TypeclassFoo> typeclass) {
    this->members[typeclass->path.basname()] = std::make_unique<TypeclassModuleMember>(typeclass.get());
    this->typeclasses.push_back(std::move(typeclass));
}
