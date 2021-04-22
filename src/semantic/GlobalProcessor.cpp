//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../units/Class.h"
#include "../logging/logging.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "util.h"
#include "../units/ConstFunction.h"

GlobalProcessor::GlobalProcessor() {
}

std::string join_path(VectorOfStrings path) {
    std::string o;
    for (auto p: path) {
        o += p + ".";
    }
    o = o.substr(0, o.size() - 1);
    return o;
}

void GlobalProcessor::visit(ImportNode& node) {
    if (node.has_alias) {
        if (this->imported_paths_with_alias.count(node.alias)) {
            throw std::runtime_error("Import alias \"" + node.alias + "\" already defined for " +
                                     this->imported_paths_with_alias[node.alias].as_str());
        }
        this->imported_paths_with_alias[node.alias] = node.path;
        this->module->imported_paths_with_alias_v.push_back(std::make_pair(node.alias, node.path));
    } else {
        if (this->imported_paths_with_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + Path(node.path).as_str() + " already imported!");
        }
        if (this->imported_paths_no_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + Path(node.path).as_str() + " already imported!");
        }
        this->imported_paths_no_alias[node.path.back()] = node.path;
        this->module->imported_paths_no_alias_v.push_back(std::make_pair(node.path.back(), node.path));
    }
    // this->module->inverted[Path(node.path).as_str()] = node.alias;
    // for (auto imported_name: node.path) {
    //     (*this->module_mappings[this->module_name])[imported_name] = (*this->module_mappings[node.module_name])[imported_name];
    // }
}

void GlobalProcessor::visit(FunctionNode& node) {
    ConstFunction* const_function = this->module->flirpins[node.identifier].const_function;

    VectorOfTypes x;
    for (auto p: node.parameter_types) {
        if (!p->object().is_generic()) {
            p->object().actual_base_path = this->get_actual_path({p->object().id});
        }
        x.emplace_back(p->clone());
    }
    if (!node.return_type->object().is_generic()) {
        node.return_type->object().actual_base_path = this->get_actual_path(node.return_type->object().id);
    }
    FunctionType function_info(x, node.return_type->clone());
    Path function_path = Path(module_dotted_path, node.identifier);
    const_function->ft = function_info.clone();
    const_function->path = Path(this->module->path, node.identifier);
    node.path = const_function->path;
    node.const_function = const_function;
}

void GlobalProcessor::visit_root(BlockNode& node) {
    // process imports first
    // process classes second
    // finally process functions
    std::map<std::string, void*> names;
    for (auto n: node.nodes) {
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
        }
        if (names.count(name) == 0) {
            names[name] = nullptr;
        } else {
            throw std::runtime_error("Error: name \"" + name + "\" already defined");
        }
    }
    for (auto n: node.nodes) {
        if (n->ntype == NodeType::IMPORT) {
            this->dispatch(n);
        }
    }
    for (auto n: node.nodes) {
        if (n->ntype == NodeType::CLS) {
            // this->dispatch(n);
            Class* class_info = new Class();
            this->module->flirpins[n->cls().class_name] = Flirpin{.type=F_TYPE::CLASS, .clazz=class_info};
            class_info->path = Path(this->module->path, n->cls().class_name);
        }
    }
    for (auto n: node.nodes) {
        if (n->ntype == NodeType::FUNC) {
            // this->dispatch(n);
            ConstFunction* const_function = new ConstFunction();
            this->module->flirpins[n->func().identifier] = Flirpin{.type=F_TYPE::CONST_FUNCTION, .const_function=const_function};
        }
    }
    for (auto n: node.nodes) {
        if (n->ntype == NodeType::CLS) {
            this->dispatch(n);
        }
    }
    for (auto n: node.nodes) {
        if (n->ntype == NodeType::FUNC) {
            this->dispatch(n);
        }
    }
}


void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        this->dispatch(n);
    }
}

void GlobalProcessor::visit(ClassNode& node) {
    Class* class_info = this->module->flirpins[node.class_name].clazz;
    //
    // if (this->imported_paths.count(node.class_name) == 1) {
    //     throw std::runtime_error("Name \"" + node.class_name + "\" already used as an alias for " +
    //                              this->imported_paths[node.class_name].as_str());
    // }

    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        mt->object().actual_base_path = this->get_actual_path(mt->object().id);
        class_info->member_names.push_back(mn);
        class_info->member_types.push_back(mt);
        class_info->members[mn] = mt;
    }
    for (auto mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    bool has_init = false;
    for (auto f: node.methods) {
        FunctionNode& method = *f.second;
        has_init = f.first == "init";

        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        ConstFunction* cf = new ConstFunction();
        cf->path = class_info->path.as_str() + "." + f.first;
        method.path = cf->path;
        cf->ft = new FunctionType(x, method.return_type->clone());
        f.second->const_function = cf;
        class_info->methods.insert(make_pair(f.first, cf));
    }

    for (auto f: node.static_methods) {
        FunctionNode& method = *f.second;
        has_init = f.first == "init";

        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

        ConstFunction* cf = new ConstFunction();
        cf->path = class_info->path.as_str() + "." + f.first;
        method.path = cf->path;
        cf->ft = new FunctionType(x, method.return_type->clone());
        f.second->const_function = cf;
        class_info->static_methods.insert(make_pair(f.first, cf));
    }

    VectorOfTypes tp;
    for (size_t i = 0; i < node.type_parameters.size(); i++) {
        tp.push_back(new ObjectType(node.type_parameters[i]));
    }
    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    class_info->path = this->module->path.as_str() + "." + class_info->class_name;
}

void GlobalProcessor::dispatch(Node* nod) {
    auto& n = *nod;
    switch (n.ntype) {
        case NodeType::CLS:
            this->visit(n.cls());
            break;
        case NodeType::FUNC:
            this->visit(n.func());
            break;
        case NodeType::IMPORT:
            this->visit(n.import());
            break;
        default:
            return;
    }
}

Path GlobalProcessor::get_actual_path(std::string id) {
    if (id == ".None") {
        return Path(VectorOfStrings({".None"}));
    }
    if (this->module->flirpins.count(id) == 1) {
        return this->module->flirpins[id].clazz->path;
    }
    if (this->imported_paths_with_alias.count(id) == 1) {
        return this->imported_paths_with_alias[id];
    }
    if (this->imported_paths_no_alias.count(id) == 1) {
        return this->imported_paths_no_alias[id];
    }
    throw std::runtime_error("Error: type " + id + " not found");
}
