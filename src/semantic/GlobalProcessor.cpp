//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"

GlobalProcessor::GlobalProcessor() = default;

void GlobalProcessor::visit_import(ImportNode& node) {
    const Path& node_path = Path(node.path);
    if (node.has_alias) {
        if (this->module->imported_paths_with_alias.count(node.alias) != 0) {
            throw std::runtime_error("Import alias \"" + node.alias + "\" already defined for " +
                                     this->module->imported_paths_with_alias[node.alias].as_str());
        }
        this->module->imported_paths_with_alias[node.alias] = node_path;
        this->module->imported_paths_with_alias_v.emplace_back(node.alias, node_path);
    } else {
        if (this->module->imported_paths_with_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + node_path.as_str() + " already imported!");
        }
        if (this->module->imported_paths_no_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + node_path.as_str() + " already imported!");
        }
        this->module->imported_paths_no_alias[node.path.back()] = node_path;
        this->module->imported_paths_no_alias_v.emplace_back(node.path.back(), node_path);
    }
}


void GlobalProcessor::visit_function(FunctionNode& node) {
    std::cout << "Global-processing function " << node.identifier << " in module " << this->module->name << std::endl;
    ConstFunction* const_function = this->module->flirpins[node.identifier].const_function;

    VectorOfTypes x;
    for (auto* p: node.parameter_types) {
        this->module->fill_actual(p);
        x.emplace_back(p->clone());
    }
    TypeNode* p = node.return_type;
    this->module->fill_actual(p);
    FunctionType function_info(x, node.return_type->clone());
    Path function_path = Path(this->module->path, node.identifier);
    const_function->ft = function_info.clone();
    const_function->path = Path(this->module->path, node.identifier);
    node.path = const_function->path;
    node.const_function = const_function;
}

void GlobalProcessor::visit_root(BlockNode& node) {
    // process imports first
    // process classes second
    // finally process functions
    check_duplicated_names(node);

    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::IMPORT) {
            this->dispatch(n);
        }

    }
    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::CLS) {
            // this->dispatch(n);
            auto* class_info = new Class();
            this->module->flirpins[n->cls().class_name] = Flirpin{.type=F_TYPE::CLASS, .clazz=class_info};
            class_info->path = Path(this->module->path, n->cls().class_name);
        } else if (n->ntype == NodeType::ENUM) {
            Enum* enumm = new Enum();
            enumm->enumm_name = ((EnumNode*) n)->id;
            enumm->values = ((EnumNode*) n)->values;
            enumm->path = Path(this->module->path, enumm->enumm_name);
            auto* eqfun = new ConstFunction();
            eqfun->path = Path(enumm->path, "__eq__");
            auto* nefun = new ConstFunction();
            nefun->path = Path(enumm->path, "__ne__");
            enumm->functions["__eq__"] = eqfun;
            enumm->functions["__ne__"] = nefun;
            this->module->flirpins[enumm->enumm_name] = Flirpin{.type=F_TYPE::ENUM, .enumm=enumm};
        }
    }
    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::FUNC) {
            // this->dispatch(n);
            auto* const_function = new ConstFunction();
            this->module->flirpins[n->func().identifier] = Flirpin{.type=F_TYPE::CONST_FUNCTION, .const_function=const_function};
        }
    }
    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::ALIAS) {
            this->dispatch(n);
        }
    }

    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::CLS) {
            this->dispatch(n);
        }
    }
    for (auto* n: node.nodes) {
        if (n->ntype == NodeType::FUNC) {
            this->dispatch(n);
        }
    }

}

void GlobalProcessor::check_duplicated_names(BlockNode& node) const {
    std::map<std::string, void*> names;
    for (auto* n: node.nodes) {
        std::string name;
        if (n->ntype == NodeType::CLS) {
            name = n->cls().class_name;
        } else if (n->ntype == NodeType::FUNC) {
            name = n->func().identifier;
        } else if (n->ntype == NodeType::IMPORT) {
            if (n->import().has_alias) {
                name = n->import().alias;
            } else {
                name = n->import().path.back();
            }
        } else if (n->ntype == NodeType::ALIAS) {
            name = ((AliasNode*) (n))->alias_id;
        } else if (n->ntype == NodeType::ENUM) {
            name = ((EnumNode*) (n))->id;
        }
        if (names.count(name) == 0) {
            names[name] = nullptr;
        } else {
            throw std::runtime_error("Error: name \"" + name + "\" already defined");
        }
    }
}


void GlobalProcessor::visit_block(BlockNode& node) {
    for (auto* n: node.nodes) {
        this->dispatch(n);
    }
}

void GlobalProcessor::visit_class(ClassNode& node) {
    Class* class_info = this->module->flirpins[node.class_name].clazz;
    //
    // if (this->imported_paths.count(node.class_name) == 1) {
    //     throw std::runtime_error("Name \"" + node.class_name + "\" already used as an alias for " +
    //                              this->imported_paths[node.class_name].as_str());
    // }
    class_info->type_params = node.type_parameters;
    for (const auto& mn: node.members_ordered) {
        auto* mt = node.members[mn];
        this->module->fill_actual(mt);
        // if (!mt->object().is_generic()) {
        //     mt->object().actual_base_path = this->get_actual_path(mt->object().id);
        // }
        class_info->member_names.push_back(mn);
        class_info->member_types.push_back(mt);
        class_info->members[mn] = mt;
        class_info->member_entities[mn] = Entity(E_TYPE::NOTHING);
    }
    for (const auto& mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& f: node.methods) {
        FunctionNode& method = *f.second;

        VectorOfTypes x;
        for (auto* p: method.parameter_types) {
            this->module->fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module->fill_actual(method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto* cf = new ConstFunction();
        cf->path = Path(class_info->path, f.first);
        method.path = cf->path;
        cf->ft = new FunctionType(x, method.return_type->clone());
        f.second->const_function = cf;
        class_info->methods.insert(make_pair(f.first, cf));
    }

    for (const auto& f: node.static_methods) {
        FunctionNode& method = *f.second;
        VectorOfTypes x;
        for (auto* p: method.parameter_types) {
            this->module->fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module->fill_actual(method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        auto* cf = new ConstFunction();
        cf->path = Path(class_info->path, f.first);
        method.path = cf->path;
        cf->ft = new FunctionType(x, method.return_type->clone());
        f.second->const_function = cf;
        class_info->static_methods.insert(make_pair(f.first, cf));
    }

    VectorOfTypes tp;
    for (auto& type_parameter : node.type_parameters) {
        tp.push_back(new ObjectType(type_parameter));
    }
    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    class_info->path = Path(this->module->path, class_info->class_name);
}

void GlobalProcessor::dispatch(Node* nod) {
    auto& n = *nod;
    switch (n.ntype) {
        case NodeType::CLS:
            this->visit_class(n.cls());
            break;
        case NodeType::FUNC:
            this->visit_function(n.func());
            break;
        case NodeType::IMPORT:
            this->visit_import(n.import());
            break;
        case NodeType::ALIAS:
            this->visit_alias((AliasNode&) n);
            break;
        default:
            return;
    }
}

void GlobalProcessor::visit_alias(AliasNode& node) {
    this->module->fill_actual(node.aliased_type);
    this->module->aliased_types[node.alias_id] = node.aliased_type;
}

void GlobalProcessor::visit_enum(EnumNode& node) {

}

Path Module::get_actual_path(const std::string& id) {
    if (id == ".None") {
        return Path(VectorOfStrings({".None"}));
    }
    if (id == "Union") {
        return Path("core.Union");
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

void Module::fill_actual(TypeNode* t) {
    if (t->kind == Kind::OBJECT) {
        if (this->aliased_types.count(t->object().id) != 0) {
            t->object().aliased_type = this->aliased_types[t->object().id];
            return;
        }
        fill_actual(&t->object());
    }
    if (t->kind == Kind::FUNCTION) {
        fill_actual(&t->function());
        return;
    }
}

void Module::fill_actual(ObjectType* t) {
    if (t->id.size() == 1) {
        return;
    }
    if (t->is_generic_param) {
        return;
    }
    t->actual_base_path = this->get_actual_path(t->id);
    for (auto* tp: t->type_params) {
        this->fill_actual(tp);
    }
}

void Module::fill_actual(FunctionType* t) {
    for (auto* pt: t->param_types) {
        this->fill_actual(pt);
    }
    this->fill_actual(t->return_type);
}
