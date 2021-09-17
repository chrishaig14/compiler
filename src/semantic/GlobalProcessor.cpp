//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../ast/ObjectType.h"
#include "../simple_nodes/TypeObject.h"
#include "../simple_nodes/TypeFunction.h"
#include "../simple_nodes/Type.h"

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
    std::vector<Path> default_paths = {Path("libcore.libcore.String"), Path("libcore.libcore.Integer"), Path("libcore.libcore.List"),
                                       Path("libcore.libcore.Double"), Path("libcore.libcore.Boolean"), Path("libcore.libcore.Float"),
                                       Path("libcore.libcore.Option"), Path("libcore.libcore.print"), Path("libcore.libcore.Dict")};
    for (auto path: default_paths) {
        if (this->module.imported_paths_with_alias.count(path.as_vec().back()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + path.as_str() + " already imported!");
        }
        if (this->module.imported_paths_no_alias.count(path.as_vec().back()) != 0) {
            std::cout << this->module.abs_path << std::endl;
            throw std::runtime_error("Path " + path.as_str() + " already imported!");
        }
        this->module.imported_paths_no_alias[path.as_vec().back()] = path;
        this->module.imported_paths_no_alias_v.emplace_back(path.as_vec().back(), path);
    }
}


void GlobalProcessor::visit_function(ast::Function& node) {
    std::cout << "Global-processing function " << node.identifier << " in module " << this->module.name << std::endl;


    ast::VectorOfTypes x;
    for (ast::Type& type_node: node.parameter_types) {
        this->module.fill_actual(type_node);
        x.emplace_back(type_node.clone());
    }
    ast::Type& p = *node.return_type;
    this->module.fill_actual(p);
    ast::FunctionType function_info(x, ast::UTypeNode(node.return_type->clone()));
    Path function_path = Path(this->module.path, node.identifier);
    ConstFunction* const_function = new ConstFunction(Path(this->module.path, node.identifier),
                                                      sem::UTypeFunction((sem::TypeFunction*) function_info.to_sem()));
    if (node.implicit != nullptr) {
        const_function->implicit = node.implicit;
        this->module.fill_actual(*node.implicit->ft);
    }
    node.path = const_function->path;
    // node.const_function = const_function;

    this->module.add_func_definition(const_function);
}

Enum* make_enum(ast::EnumNode& n, Path module_path) {
    Enum* enumm = new Enum(n.id, Path(module_path, n.id), n.values);
    enumm->functions["__eq__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__eq__"), nullptr);
    enumm->functions["__ne__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__ne__"), nullptr);
    return enumm;
}

void GlobalProcessor::visit_root() {
    ast::Module& node = *this->module.ast;
    this->add_default_imports();
    check_duplicated_names(node);

    for (ast::Import& n: node.imports) {
        this->visit_import(n);
    }
    for (ast::Klass& n: node.classes) {
        auto* class_info = new Class(n.class_name, Path(this->module.path, n.class_name));
        this->module.add_class_definition(class_info);
    }
    for (ast::EnumNode& n: node.enums) {
        Enum* enumm = make_enum(n, this->module.path);
        this->module.add_enum_definition(enumm);
    }

    for (ast::Klass& n: node.classes) {
        this->visit_class(n);
    }
    for (ast::Function& n: node.functions) {
        this->visit_function(n);
    }

}

void GlobalProcessor::check_duplicated_names(ast::Module& node) const {
    std::map<std::string, void*> names;
    for (auto& np: node.all) {
        auto& n = *np;
        std::string name;
        if (n.ntype == NodeType::CLS) {
            name = ((ast::Klass&) n).class_name;
        } else if (n.ntype == NodeType::FUNC) {
            name = ((ast::Function&) n).identifier;
        } else if (n.ntype == NodeType::IMPORT) {
            if (((ast::Import&) n).has_alias) {
                name = ((ast::Import&) n).alias;
            } else {
                name = ((ast::Import&) n).path.back();
            }
        } else if (n.ntype == NodeType::ALIAS) {
            name = ((ast::Alias&) (n)).alias_id;
        } else if (n.ntype == NodeType::ENUM) {
            name = ((ast::EnumNode&) (n)).id;
        }
        if (names.count(name) == 0) {
            names[name] = nullptr;
        } else {
            throw std::runtime_error("Error: name \"" + name + "\" already defined");
        }
    }
}


void GlobalProcessor::visit_block(ast::Block& node) {
    for (auto& n: node.nodes) {
        this->dispatch(*n);
    }
}

void GlobalProcessor::visit_class(ast::Klass& node) {
    Class* class_info = this->module.members[node.class_name].clazz;
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

        auto* cf = new ConstFunction(Path(class_info->path, f.first),
                                     std::make_unique<sem::TypeFunction>(x, sem::UType(method.return_type->to_sem())));
        method.path = cf->path;
        cf->implicit = f.second->method->implicit;
        // f.second->method->const_function = cf;
        class_info->methods.insert(make_pair(f.first, cf));
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

        auto* cf = new ConstFunction(Path(class_info->path, f.first),
                                     std::make_unique<sem::TypeFunction>(x, sem::UType(method.return_type->to_sem())));
        method.path = cf->path;
        // f.second->const_function = cf;
        class_info->static_methods.insert(make_pair(f.first, cf));
    }

    // class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    // class_info->path = Path(this->module.path, class_info->class_name);
}

void GlobalProcessor::dispatch(ast::Node& nod) {
    switch (nod.ntype) {
        case NodeType::CLS:
            this->visit_class((ast::Klass&) nod);
            break;
        case NodeType::FUNC:
            this->visit_function((ast::Function&) nod);
            break;
        case NodeType::IMPORT:
            this->visit_import((ast::Import&) nod);
            break;
        case NodeType::ALIAS:
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

GlobalProcessor::GlobalProcessor(Module& module) : module(module) {
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
        if (this->members[id].type == ModuleMemberType::CLASS) {
            return this->members[id].clazz->path;
        } else if (this->members[id].type == ModuleMemberType::ENUM) {
            return this->members[id].enumm->path;
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

void Module::fill_actual(ast::Type& t) {
    if (t.kind == Kind::OBJECT) {
        if (this->aliased_types.count(t.object().id) != 0) {
            t.object().data.aliased_type = this->aliased_types[t.object().id];
            return;
        }
        this->fill_actual(t.object());
    }
    if (t.kind == Kind::FUNCTION) {
        this->fill_actual(t.function());
        return;
    }
}

void Module::fill_actual(ast::ObjectType& t) {
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

void Module::fill_actual(ast::FunctionType& t) {
    for (auto& pt: t.param_types) {
        this->fill_actual(*pt);
    }
    this->fill_actual(*t.return_type);
}

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
