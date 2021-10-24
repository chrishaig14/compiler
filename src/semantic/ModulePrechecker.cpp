//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "ModulePrechecker.h"
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <simple_nodes/common/include/Type.h>
#include "errors/include/ErrorRedeclared.h"
#include <units/entities/EntityNothing.h>
#include "errors/include/ErrorGlobalRedeclared.h"
#include <ast/top/Alias.h>
#include <ast/top/Import.h>
#include <ast/top/EnumNode.h>
#include <ast/top/TemplateClassDef.h>
#include <ast/top/ConcreteClassDef.h>
#include <ast/top/TypeclassAst.h>
#include <ast/top/Instance.h>
#include <units/infos/TemplateClass.h>

const VectorOfStrings default_imports = {"libcore.libcore.String", "libcore.libcore.Integer", "libcore.libcore.List",
                                         "libcore.libcore.Double", "libcore.libcore.Boolean", "libcore.libcore.Float",
                                         "libcore.libcore.Option", "libcore.libcore.print", "libcore.libcore.Dict",
                                         "libcore.libcore.input", "libcore.libcore.File"};


void ModulePrechecker::visit_import(ast::Import& node) {
    const Path& node_path = Path(node.path);
    if (node.has_alias) {
        if (this->module.imported_paths_with_alias.count(node.alias) != 0) {
            throw std::runtime_error("Import alias \"" + node.alias + "\" already defined for " +
                                     this->module.imported_paths_with_alias[node.alias].as_str());
        }
        this->module.imported_paths_with_alias[node.alias] = node_path;
        this->module.imported_paths_with_alias_v.emplace_back(node.alias, node_path);
    } else {
        if (this->module.imported_paths_with_alias.count(node.path.back()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + node_path.as_str() + " already imported!");
        }
        if (this->module.imported_paths_no_alias.count(node.path.back()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + node_path.as_str() + " already imported!");
        }
        this->module.imported_paths_no_alias[node.path.back()] = node_path;
        this->module.imported_paths_no_alias_v.emplace_back(node.path.back(), node_path);
    }
}

void ModulePrechecker::add_default_imports() {
    for (auto& import_path: default_imports) {
        Path path(import_path);
        if (this->module.imported_paths_with_alias.count(path.basname()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + path.as_str() + " already imported!");
        }
        if (this->module.imported_paths_no_alias.count(path.basname()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + path.as_str() + " already imported!");
        }
        this->module.imported_paths_no_alias[path.basname()] = path;
        this->module.imported_paths_no_alias_v.emplace_back(path.basname(), path);
    }
}


void ModulePrechecker::visit_function(ast::Function& node) {
    sem::VectorOfTypes x;
    for (ast::Type& type_node: node.parameter_types) {
        sem::Type* p_type = type_node.to_sem();
        this->module.fill_actual(*p_type);
        x.emplace_back(p_type);
    }
    auto rt = sem::UType(node.return_type->to_sem());
    this->module.fill_actual(*rt);
    auto function_info = std::make_unique<sem::TypeFunction>(std::move(x), std::move(rt));
    Path function_path = Path(this->module.path, node.identifier);
    auto const_function = std::make_unique<ConstFunction>(Path(this->module.path, node.identifier),
                                                          std::move(function_info));
    const_function->constraints = node.constraints;
    node.path = const_function->path;
    this->module.add_func_definition(std::move(const_function));
}

std::unique_ptr<Enum> make_enum(ast::EnumNode& n, Path module_path) {
    auto enumm = std::make_unique<Enum>(n.id, Path(module_path, n.id), n.values);
    enumm->functions["__eq__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__eq__"), nullptr);
    enumm->functions["__ne__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__ne__"), nullptr);
    return enumm;
}

void ModulePrechecker::visit_root() {
    ast::Module& node = *this->module.ast;
    this->add_default_imports();
    this->check_duplicated_names(node);

    for (ast::Import& n: node.imports) {
        this->visit_import(n);
    }
    for (ast::ConcreteClassDef& n: node.classes) {
        auto class_info = std::make_unique<ConcreteClass>(n.class_name, Path(this->module.path, n.class_name));
        this->module.add_class_definition(std::move(class_info));
    }

    for (ast::TemplateClassDef& n: node.template_classes) {
        auto class_info = std::make_unique<TemplateClassInfo>(n.class_name,
                                                              Path(this->module.path, n.class_name),
                                                              n.type_parameters);
        this->module.add_template_class_definition(std::move(class_info));
    }

    for (ast::EnumNode& n: node.enums) {
        auto enumm = make_enum(n, this->module.path);
        this->module.add_enum_definition(std::move(enumm));
    }

    for (ast::ConcreteClassDef& n: node.classes) {
        this->visit_class(n);
    }

    for (ast::Instance& n: node.instances) {
        this->visit_instance(n);
    }

    for (ast::TemplateClassDef& n: node.template_classes) {
        this->visit_template_class(n);
    }


    for (ast::TypeclassAst& n: node.typeclasses) {
        this->visit_typeclass(n);
    }

    for (ast::Function& n: node.functions) {
        this->visit_function(n);
    }

}

void ModulePrechecker::check_duplicated_names(ast::Module& node) {
    std::map<std::string, void*> names;
    for (auto& np: node.all) {
        auto& n = *np;
        std::string name;
        switch (n.ntype) {
            case TopNodeType::IMPORT:
                if (((ast::Import&) n).has_alias) {
                    name = ((ast::Import&) n).alias;
                } else {
                    name = ((ast::Import&) n).path.back();
                }
                break;
            case TopNodeType::FUNC:
                name = ((ast::Function&) n).identifier;
                break;
            case TopNodeType::ENUM:
                name = ((ast::EnumNode&) (n)).id;
                break;
            case TopNodeType::TYPECLASS:
                name = ((ast::TypeclassAst&) (n)).id;
                break;
            case TopNodeType::CONCRETE_CLS:
                name = ((ast::ConcreteClassDef&) n).class_name;
                break;
            case TopNodeType::TEMPLATE_CLS:
                name = ((ast::TemplateClassDef&) n).class_name;
                break;
            case TopNodeType::INSTANCE:
                continue;
        }
        assert(not name.empty());
        if (names.count(name) == 0) {
            names[name] = nullptr;
        } else {
            this->error_reporter.error(std::make_unique<ErrorGlobalRedeclared>(name));
        }
    }
}

void ModulePrechecker::visit_class(ast::ConcreteClassDef& node) {
    ConcreteClass* class_info = &this->module.members.at(node.class_name)->klass();
    for (const auto& mt: node.members) {
        // this->module.fill_actual(*mt.second);
        class_info->member_names.push_back(mt.first);
        class_info->member_types.push_back(mt.second->clone());
        class_info->members[mt.first] = mt.second->clone();
        class_info->member_entities[mt.first] = std::make_unique<EntityNothing>();
    }
    for (const auto& mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& f: node.methods) {
        ast::Function& method = *f.second;

        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = p.to_sem();
            this->module.fill_actual(*args);
            x.emplace_back(args);
        }
        sem::Type* p_type = method.return_type->to_sem();
        this->module.fill_actual(*p_type);
        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x, sem::UType(p_type)));
        method.path = cf->path;
        class_info->methods.insert(make_pair(f.first, std::move(cf)));
    }

    for (const auto& f: node.static_methods) {
        ast::Function& method = *f.second;
        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = p.to_sem();
            this->module.fill_actual(*args);
            x.emplace_back(args);
        }
        sem::Type* p_type = method.return_type->to_sem();
        this->module.fill_actual(*p_type);
        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x, sem::UType(p_type)));
        method.path = cf->path;
        class_info->static_methods.insert(make_pair(f.first, std::move(cf)));
    }
}


void ModulePrechecker::visit_template_class(ast::TemplateClassDef& node) {
    TemplateClassInfo* class_info = &this->module.members.at(node.class_name)->template_klass();
    for (const auto& mt: node.members) {
        // this->module.fill_actual(*mt.second);
        class_info->member_names.push_back(mt.first);
        class_info->member_types.push_back(mt.second->clone());
        class_info->members[mt.first] = mt.second->clone();
        class_info->member_entities[mt.first] = std::make_unique<EntityNothing>();
    }
    for (const auto& mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& f: node.methods) {
        ast::Function& method = *f.second;

        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = p.to_sem();
            this->module.fill_actual(*args);
            x.emplace_back(args);
        }
        sem::Type* p_type = method.return_type->to_sem();
        this->module.fill_actual(*p_type);
        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x, sem::UType(p_type)));
        method.path = cf->path;
        class_info->methods.insert(make_pair(f.first, std::move(cf)));
    }

    for (const auto& f: node.static_methods) {
        ast::Function& method = *f.second;
        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = p.to_sem();
            this->module.fill_actual(*args);
            x.emplace_back(args);
        }
        sem::Type* p_type = method.return_type->to_sem();
        this->module.fill_actual(*p_type);
        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x, sem::UType(p_type)));
        method.path = cf->path;
        class_info->static_methods.insert(make_pair(f.first, std::move(cf)));
    }
}

void ModulePrechecker::visit_alias(ast::Alias& node) {
    // this->module.fill_actual(*node.aliased_type);
    // this->module.aliased_types[node.alias_id] = node.aliased_type;
}

void ModulePrechecker::visit_enum(ast::EnumNode& node) {

}

ModulePrechecker::ModulePrechecker(Module& module, std::map<std::string, std::string>& instances)
        : module(module), error_reporter(module.code_lines), instances(instances) {
}


void ModulePrechecker::visit_typeclass(ast::TypeclassAst& typeclass) {
    auto tc = std::make_unique<TypeclassFoo>(typeclass.id, typeclass.base_type, Path(this->module.path, typeclass.id));
    for (auto& m : typeclass.methods) {
        ast::FunctionType& method = *m.second;

        sem::VectorOfTypes x;
        for (auto& p: method.param_types) {
            sem::Type* args = p->to_sem();
            this->module.fill_actual(*args);
            x.emplace_back(args);
        }
        sem::Type* p_type = method.return_type->to_sem();
        this->module.fill_actual(*p_type);
        auto cf = std::make_unique<sem::TypeFunction>(x, sem::UType(p_type));
        tc->methods[m.first] = std::move(cf);
    }

    this->module.add_typeclass_definition(std::move(tc));
}

void ModulePrechecker::visit_instance(ast::Instance& instance) {
    auto ot = instance.base_type->to_sem();
    this->module.fill_actual(*ot);
    this->instances[ot->object().data.actual_base_path.as_str()] = instance.id;
}
