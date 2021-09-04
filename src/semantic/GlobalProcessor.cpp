//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../ast/ObjectType.h"

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
    std::vector<Path> default_paths = {Path("core.core.String"), Path("core.core.Integer"), Path("core.core.List"),
                                       Path("core.core.Double"), Path("core.core.Boolean"), Path("core.core.Float"),
                                       Path("core.core.Option"), Path("core.core.print"), Path("core.core.Dict")};
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
    ConstFunction* const_function = this->module.flirpins[node.identifier].const_function;

    ast::VectorOfTypes x;
    for (auto& p: node.parameter_types) {
        ast::TypeNode& type_node = *p;
        this->module.fill_actual(type_node);
        x.emplace_back(p->clone());
    }
    ast::TypeNode& p = *node.return_type;
    this->module.fill_actual(p);
    ast::FunctionType function_info(x, ast::UTypeNode(node.return_type->clone()));
    Path function_path = Path(this->module.path, node.identifier);
    const_function->ft = function_info.clone();
    const_function->path = Path(this->module.path, node.identifier);
    if (node.implicit != nullptr) {
        const_function->implicit = node.implicit;
        this->module.fill_actual(*node.implicit->ft);
    }
    node.path = const_function->path;
    node.const_function = const_function;
}

void GlobalProcessor::visit_root() {
    // process imports first
    // process classes second
    // finally process functions
    ast::Block& node = *this->module.ast;
    check_duplicated_names(node);

    this->add_default_imports();

    for (auto& n: node.nodes) {
        if (n->ntype == NodeType::IMPORT) {
            this->dispatch(*n);
        }

    }
    for (auto& np: node.nodes) {
        auto& n = *np;
        if (n.ntype == NodeType::CLS) {
            // this->dispatch(n);
            auto* class_info = new Class();
            this->module.flirpins[((ast::Klass&) n).class_name] = Flirpin{.type=F_TYPE::CLASS, .clazz=class_info};
            class_info->path = Path(this->module.path, ((ast::Klass&) n).class_name);
        } else if (n.ntype == NodeType::ENUM) {
            Enum* enumm = new Enum();
            enumm->enumm_name = ((ast::EnumNode&) n).id;
            enumm->values = ((ast::EnumNode&) n).values;
            enumm->path = Path(this->module.path, enumm->enumm_name);
            enumm->functions["__eq__"] = new ConstFunction(Path(enumm->path, "__eq__"), nullptr);;
            enumm->functions["__ne__"] = new ConstFunction(Path(enumm->path, "__ne__"), nullptr);
            this->module.flirpins[enumm->enumm_name] = Flirpin{.type=F_TYPE::ENUM, .enumm=enumm};
        }
    }
    for (auto& np: node.nodes) {
        auto& n = *np;
        if (n.ntype == NodeType::FUNC) {
            // this->dispatch(n);
            auto* const_function = new ConstFunction(Path(), nullptr);
            this->module.const_functions.push_back(std::unique_ptr<ConstFunction>(const_function));
            this->module.flirpins[((ast::Function&) n).identifier] = Flirpin{.type=F_TYPE::CONST_FUNCTION, .const_function=const_function};
        }
    }
    for (auto& n: node.nodes) {
        if (n->ntype == NodeType::ALIAS) {
            this->dispatch(*n);
        }
    }

    for (auto& n: node.nodes) {
        if (n->ntype == NodeType::CLS) {
            this->dispatch(*n);
        }
    }
    for (auto& n: node.nodes) {
        if (n->ntype == NodeType::FUNC) {
            this->dispatch(*n);
        }
    }

}

void GlobalProcessor::check_duplicated_names(ast::Block& node) const {
    std::map<std::string, void*> names;
    for (auto& np: node.nodes) {
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
    Class* class_info = this->module.flirpins[node.class_name].clazz;
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
        class_info->member_entities[mt.first] = new EntityNothing();
    }
    for (const auto& mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& f: node.methods) {
        ast::Function& method = *f.second->method;

        ast::VectorOfTypes x;
        for (auto& p: method.parameter_types) {
            this->module.fill_actual(*p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module.fill_actual(*method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto* cf = new ConstFunction(Path(class_info->path, f.first),
                                     new ast::FunctionType(x, ast::UTypeNode(method.return_type->clone())));
        method.path = cf->path;
        cf->implicit = f.second->method->implicit;
        f.second->method->const_function = cf;
        class_info->methods.insert(make_pair(f.first, cf));
    }

    for (const auto& f: node.static_methods) {
        ast::Function& method = *f.second;
        ast::VectorOfTypes x;
        for (auto& p: method.parameter_types) {
            this->module.fill_actual(*p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module.fill_actual(*method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto* cf = new ConstFunction(Path(class_info->path, f.first),
                                     new ast::FunctionType(x, ast::UTypeNode(method.return_type->clone())));
        method.path = cf->path;
        f.second->const_function = cf;
        class_info->static_methods.insert(make_pair(f.first, cf));
    }

    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    class_info->path = Path(this->module.path, class_info->class_name);
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
        return Path("core.core.Union");
    }
    if (this->flirpins.count(id) == 1) {
        if (this->flirpins[id].type == F_TYPE::CLASS) {
            return this->flirpins[id].clazz->path;
        } else if (this->flirpins[id].type == F_TYPE::ENUM) {
            return this->flirpins[id].enumm->path;
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

void Module::fill_actual(ast::TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        if (this->aliased_types.count(t.object().id) != 0) {
            t.object().aliased_type = this->aliased_types[t.object().id];
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
    t.actual_base_path = this->get_actual_path(t.id);
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
