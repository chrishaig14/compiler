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
        if (this->module->imported_paths_with_alias.count(node.alias)) {
            throw std::runtime_error("Import alias \"" + node.alias + "\" already defined for " +
                                     this->module->imported_paths_with_alias[node.alias].as_str());
        }
        this->module->imported_paths_with_alias[node.alias] = node.path;
        this->module->imported_paths_with_alias_v.push_back(std::make_pair(node.alias, node.path));
    } else {
        if (this->module->imported_paths_with_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + Path(node.path).as_str() + " already imported!");
        }
        if (this->module->imported_paths_no_alias.count(node.path.back()) != 0) {
            throw std::runtime_error("Path " + Path(node.path).as_str() + " already imported!");
        }
        this->module->imported_paths_no_alias[node.path.back()] = node.path;
        this->module->imported_paths_no_alias_v.push_back(std::make_pair(node.path.back(), node.path));
    }
}


void GlobalProcessor::visit(FunctionNode& node) {
    ConstFunction* const_function = this->module->flirpins[node.identifier].const_function;

    VectorOfTypes x;
    for (auto p: node.parameter_types) {
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
        } else if (n->ntype == NodeType::ALIAS) {
            name = ((AliasNode*) (n))->alias_id;
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
        } else if (n->ntype == NodeType::ENUM) {
            Enum* enumm = new Enum();
            enumm->enumm_name = ((EnumNode*) n)->id;
            enumm->values = ((EnumNode*) n)->values;
            enumm->path = this->module->path.as_str() + "." + enumm->enumm_name;
            ConstFunction* eqfun = new ConstFunction();
            eqfun->path = Path(enumm->path, "eq");
            ConstFunction* nefun = new ConstFunction();
            nefun->path = Path(enumm->path, "ne");
            enumm->functions["eq"] = eqfun;
            enumm->functions["ne"] = nefun;
            this->module->flirpins[enumm->enumm_name] = Flirpin{.type=F_TYPE::ENUM, .enumm=enumm};
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
        if (n->ntype == NodeType::ALIAS) {
            this->dispatch(n);
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
    class_info->type_params = node.type_parameters;
    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        this->module->fill_actual(mt);
        // if (!mt->object().is_generic()) {
        //     mt->object().actual_base_path = this->get_actual_path(mt->object().id);
        // }
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
            this->module->fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module->fill_actual(method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

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
            this->module->fill_actual(p);
            // p->object().actual_base_path = this->get_actual_path(p->object().id);
            x.emplace_back(p->clone());
        }
        this->module->fill_actual(method.return_type);
        // method.return_type->object().actual_base_path = this->get_actual_path(method.return_type->object().id);

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
        case NodeType::ALIAS:
            this->visit((AliasNode&) n);
            break;
        default:
            return;
    }
}

void GlobalProcessor::visit(AliasNode& node) {
    this->module->fill_actual(node.aliased_type);
    this->module->aliased_types[node.alias_id] = node.aliased_type;
}

void GlobalProcessor::visit(EnumNode& node) {

}

Path Module::get_actual_path(std::string id) {
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
    if (this->aliased_types.count(t->object().id)) {
        t->object().aliased_type = this->aliased_types[t->object().id];
        return;
    }
    if (t->kind == Kind::FUNCTION) {
        fill_actual(&t->function());
        return;
    }
    fill_actual(&t->object());
}

void Module::fill_actual(ObjectType* t) {
    if (t->is_generic_param) {
        return;
    }
    t->actual_base_path = this->get_actual_path(t->id);
    for (auto tp: t->type_params) {
        this->fill_actual(tp);
    }
}

void Module::fill_actual(FunctionType* t) {
    for (auto pt: t->param_types) {
        this->fill_actual(pt);
    }
    this->fill_actual(t->return_type);
}
