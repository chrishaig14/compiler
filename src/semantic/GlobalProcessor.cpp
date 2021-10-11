//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../ast/general/ObjectType.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/include/TypeFunction.h"
#include "../simple_nodes/common/include/Type.h"
#include "errors/include/ErrorRedeclared.h"
#include "errors/include/ErrorGlobalRedeclared.h"

const VectorOfStrings default_imports = {"libcore.libcore.String", "libcore.libcore.Integer", "libcore.libcore.List",
                                         "libcore.libcore.Double", "libcore.libcore.Boolean", "libcore.libcore.Float",
                                         "libcore.libcore.Option", "libcore.libcore.print", "libcore.libcore.Dict",
                                         "libcore.libcore.input", "libcore.libcore.File"};


void GlobalProcessor::visit_import(ast::Import& node) {
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

void GlobalProcessor::add_default_imports() {
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


void GlobalProcessor::visit_function(ast::Function& node) {
    ast::VectorOfTypes x;
    for (ast::Type& type_node: node.parameter_types) {
        this->module.fill_actual(type_node);
        x.emplace_back(type_node.clone());
    }
    ast::Type& p = *node.return_type;
    this->module.fill_actual(p);
    ast::FunctionType function_info(x, ast::UTypeNode(node.return_type->clone()));
    Path function_path = Path(this->module.path, node.identifier);
    auto const_function = std::make_unique<ConstFunction>(Path(this->module.path, node.identifier),
                                                          sem::UTypeFunction((sem::TypeFunction*) function_info.to_sem()));
    node.path = const_function->path;
    // node.const_function = const_function;

    this->module.add_func_definition(std::move(const_function));
}

std::unique_ptr<Enum> make_enum(ast::EnumNode& n, Path module_path) {
    auto enumm = std::make_unique<Enum>(n.id, Path(module_path, n.id), n.values);
    enumm->functions["__eq__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__eq__"), nullptr);
    enumm->functions["__ne__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__ne__"), nullptr);
    return enumm;
}

void GlobalProcessor::visit_root() {
    ast::Module& node = *this->module.ast;
    this->add_default_imports();
    this->check_duplicated_names(node);

    for (ast::Import& n: node.imports) {
        this->visit_import(n);
    }
    for (ast::Klass& n: node.classes) {
        auto class_info = std::make_unique<ConcreteClass>(n.class_name, Path(this->module.path, n.class_name));
        this->module.add_class_definition(std::move(class_info));
    }
    for (ast::EnumNode& n: node.enums) {
        auto enumm = make_enum(n, this->module.path);
        this->module.add_enum_definition(std::move(enumm));
    }

    for (ast::Klass& n: node.classes) {
        this->visit_class(n);
    }
    for (ast::Function& n: node.functions) {
        this->visit_function(n);
    }

}

void GlobalProcessor::check_duplicated_names(ast::Module& node) {
    std::map<std::string, void*> names;
    for (auto& np: node.all) {
        auto& n = *np;
        std::string name;
        if (n.ntype == TopNodeType::CLS) {
            name = ((ast::Klass&) n).class_name;
        } else if (n.ntype == TopNodeType::FUNC) {
            name = ((ast::Function&) n).identifier;
        } else if (n.ntype == TopNodeType::IMPORT) {
            if (((ast::Import&) n).has_alias) {
                name = ((ast::Import&) n).alias;
            } else {
                name = ((ast::Import&) n).path.back();
            }
        } else if (n.ntype == TopNodeType::ENUM) {
            name = ((ast::EnumNode&) (n)).id;
        }
        if (names.count(name) == 0) {
            names[name] = nullptr;
        } else {
            this->error_reporter.error(std::make_unique<ErrorGlobalRedeclared>(name));
        }
    }
}


void GlobalProcessor::visit_block(ast::Block& node) {
    for (auto& n: node.nodes) {
        this->dispatch(*n);
    }
}

void GlobalProcessor::visit_class(ast::Klass& node) {
    ConcreteClass* class_info = &this->module.members[node.class_name]->klass();
    //
    // if (this->imported_paths.count(node.class_name) == 1) {
    //     throw std::runtime_error("Name \"" + node.class_name + "\" already used as an alias for " +
    //                              this->imported_paths[node.class_name].as_str());
    // }
    class_info->type_params = node.type_parameters;
    for (const auto& mt: node.members) {
        this->module.fill_actual(*mt.second);
        // if (!mt->object().is_generic()) {
        //     mt->object().actual_base_path = this->get_actual_path(mt->object().id);
        // }
        class_info->member_names.push_back(mt.first);
        class_info->member_types.push_back(mt.second->clone());
        class_info->members[mt.first] = mt.second->clone();
        class_info->member_entities[mt.first] = std::make_unique<EntityNothing>();
    }
    for (const auto& mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& f: node.methods) {
        ast::Function& method = *f.second->method;

        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            this->module.fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p.to_sem());
        }
        this->module.fill_actual(*method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x,
                                                                                      sem::UType(method.return_type->to_sem())));
        method.path = cf->path;
        // f.second->method->const_function = cf;
        class_info->methods.insert(make_pair(f.first, std::move(cf)));
    }

    for (const auto& f: node.static_methods) {
        ast::Function& method = *f.second;
        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            this->module.fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p.to_sem());
        }
        this->module.fill_actual(*method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
                                                  std::make_unique<sem::TypeFunction>(x,
                                                                                      sem::UType(method.return_type->to_sem())));
        method.path = cf->path;
        // f.second->const_function = cf;
        class_info->static_methods.insert(make_pair(f.first, std::move(cf)));
    }

    // class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    // class_info->path = Path(this->module.path, class_info->class_name);
}

void GlobalProcessor::dispatch(ast::Statement& nod) {
    switch (nod.ntype) {
        case StatementType::CLS:
            this->visit_class((ast::Klass&) nod);
            break;
        case StatementType::FUNC:
            this->visit_function((ast::Function&) nod);
            break;
        case StatementType::IMPORT:
            this->visit_import((ast::Import&) nod);
            break;
        case StatementType::ALIAS:
            this->visit_alias((ast::Alias&) nod);
            break;
        default:
            return;
    }
}

void GlobalProcessor::visit_alias(ast::Alias& node) {
    this->module.fill_actual(*node.aliased_type);
    this->module.aliased_types[node.alias_id] = node.aliased_type;
}

void GlobalProcessor::visit_enum(ast::EnumNode& node) {

}

GlobalProcessor::GlobalProcessor(Module& module) : module(module), error_reporter(module.code_lines) {
}