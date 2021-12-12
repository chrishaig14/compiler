//
// Created by chris on 17/1/21.
//

#include "ModuleChecker.h"
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <simple_nodes/expressions/include/CallExp.h>

std::unique_ptr<sem::EnumDef> ModuleChecker::visit_enum(ast::EnumNode& p_node) {
    sem::UCommon info_u;
    auto esn = std::make_unique<sem::EnumDef>(p_node.id, p_node.values);
    return esn;
}

std::unique_ptr<sem::KlassDef> ModuleChecker::visit_class(ast::ConcreteClassDef& node) {
    this->error_reporter.current_class = node.class_name;
    ast::VectorOfTypes tp;
    std::string cn = node.class_name;
    ast::VectorOfTypes members_ordered_types;
    VectorOfStrings member_types;
    for (const auto& mt: node.attributes) {
        ast::Type& t = *mt.type;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
        member_types.push_back(t.to_string());
    }
    auto sn = std::make_unique<sem::KlassDef>(node.class_name, node.attributes_ordered);
    sn->attribute_types = member_types;
    // this->this_entity = std::unique_ptr<Entity>(this->entity_value_from_actual_base_path_no_generic(Path(this->module.path,
    //                                                                                                      node.class_name)).clone());
    // this->fill_value(this->this_entity->get_value());
    Path p(this->module.path, node.class_name);
    sem::TypeObject ot(p.basname(), p);
    // auto v = std::make_unique<Value>(ot);
    // this->fill_value(*v);
    auto this_entity = this->make_entity_value(ot);

    for (auto& ast_meth: node.methods) {
        std::unique_ptr<sem::FunctionDef> ms;
        if (ast_meth->is_static) {
            // static method
            ms = this->visit_function(*ast_meth->func);
        } else {
            // normal method
            ms = this->visit_method(*ast_meth->func, std::move(this_entity));
        }
        sn->methods.push_back(*ms);
    }

    //
    // Class* clazz = ((EntityClass&) this->scope->get(node.class_name)).clazz;
    // auto csn = std::make_unique<sem::KlassDef>(clazz->path.as_str(), node.members_ordered);
    // sn->nodes.push_back(std::move(csn));
    // sn->nodes.push_back(USNode(make_class_default_init(clazz->path.as_str(), node.members_ordered)));
    //
    // for (const auto& sm: node.static_members) {
    //     sem::UCommon sm_exp_info = this->dispatch(*sm.second.second);
    //     if (*sm.second.first != *(ast::Type*) ((Value&) sm_exp_info->entity).type.to_ast()) {
    //         this->error_reporter.fail("Err: cannt initialize static member of type " + sm.second.first->to_string() +
    //                                   " with expression of type " + ((Value&) sm_exp_info->entity).type.to_string());
    //     }
    //     if (!sm_exp_info->is_constant) {
    //         this->error_reporter.fail("Error: cannot initialize static member with non constant expression!");
    //     }
    // }
    //
    // std::vector<sem::SNode*> methods_snodes;
    // std::vector<sem::SNode*> static_methods_snodes;
    //
    // for (const auto& method: node.methods) {
    //     auto* vt = new sem::TypeObject(node.class_name, clazz->path);
    //     auto val = std::make_unique<Value>(vt);
    //     this->add_this = true;
    //     this->this_entity = val.release();
    //     val->metatype = Meta::CLASS;
    //     val->clazz = clazz;
    //     // method.second->path = clazz->path + "." + method.second->identifier;
    //     sem::UCommon method_info = this->visit_function(*method.second->method);
    //     methods_snodes.push_back(method_info->snode.release());
    // }
    //
    // for (const auto& method: node.static_methods) {
    //     this->add_this = false;
    //     sem::UCommon method_info = this->visit_function(*method.second);
    //     static_methods_snodes.push_back(method_info->snode.release());
    // }
    //
    // for (auto* m: methods_snodes) {
    //     sn->nodes.push_back(USNode(m));
    // }
    // for (auto* m: static_methods_snodes) {
    //     sn->nodes.push_back(USNode(m));
    // }
    //
    // this->add_this = true;
    //
    // this->add_this = false;
    // this->error_reporter.current_class = "";
    return sn;
}

std::unique_ptr<sem::TemplateKlassDef> ModuleChecker::visit_template_class(ast::TemplateClassDef& node) {
    this->error_reporter.current_class = node.class_name;
    ast::VectorOfTypes tp;
    std::string cn = node.class_name;
    ast::VectorOfTypes members_ordered_types;
    VectorOfStrings member_types;
    for (const auto& mt: node.attributes) {
        ast::Type& t = *mt.type;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
        member_types.push_back(t.to_string());
    }
    auto sn = std::make_unique<sem::TemplateKlassDef>(node.class_name, node.aatributes_ordered);
    sn->attribute_types = member_types;
    // this->this_entity = std::unique_ptr<Entity>(this->entity_value_from_actual_base_path_no_generic(Path(this->module.path,
    //                                                                                                      node.class_name)).clone());
    // this->fill_value(this->this_entity->get_value());
    Path p(this->module.path, node.class_name);
    sem::VectorOfTypes tppp;
    for (auto& type_p: node.type_parameters) {
        tppp.push_back(new sem::TypeObject(type_p, Path("generics." + type_p)));
    }
    sem::TypeObject ot(p.basname(), tppp, p);
    // auto v = std::make_unique<Value>(ot);
    // this->fill_value(*v);
    auto this_entity = this->make_entity_value(ot);

    for (auto& ast_meth: node.methods) {
        std::unique_ptr<sem::FunctionDef> ms;
        if (ast_meth->is_static) {
            ms = this->visit_function(*ast_meth->func);
            sn->static_methods.push_back(*ms);
        } else {
            ms = this->visit_method(*ast_meth->func, std::move(this_entity));
        }
        sn->methods.push_back(*ms);
    }
    return sn;
}


void ModuleChecker::init() {
    // Initialize module level Scope
    for (const auto& f: this->module.members) {
        std::unique_ptr<Entity> e(map_module_member_to_entity(*f.second));
        this->scope->set(f.first, *e);
    }
}

std::unique_ptr<sem::Module> ModuleChecker::check_module() {
    this->init();
    auto sn = std::make_unique<sem::Module>();
    for (auto& n: this->module.ast->all) {
        // if (n->ntype == TopNodeType::TEMPLATE_CLS) {
        //     continue;
        // }
        if (n->ntype == TopNodeType::IMPORT) {
            continue;
        }
        std::unique_ptr<sem::Top> sinfo_p = this->dispatch_top(*n);
        if (sinfo_p != nullptr) {
            sn->nodes.push_back(std::move(sinfo_p));
        }
    }
    return sn;
}

std::unique_ptr<sem::Block> ModuleChecker::visit_block(const ast::Block& node) {
    auto sn = std::make_unique<sem::Block>();
    bool has_error = false;
    for (auto& n: node.nodes) {
        sem::UCommon sinfo_p = this->dispatch(*n);
        if (sinfo_p == nullptr) {
            has_error = true;
        }
        if (not has_error) {
            if (sinfo_p->type == sem::CommonType::BLOCK) {
                if (((std::unique_ptr<sem::Block>&) sinfo_p)->unwrap) {
                    for (auto& nn : ((std::unique_ptr<sem::Block>&) sinfo_p)->nodes) {
                        sn->nodes.push_back(std::move(nn));
                    }
                } else {
                    sn->nodes.push_back(std::move(sinfo_p));
                }

            } else {
                sn->nodes.push_back(std::move(sinfo_p));
            }
        }
    }
    if (has_error) {
        return nullptr;
    }
    return sn;
}

sem::Type* ModuleChecker::make_sem_type(const ast::Type& t) {
    auto s = t.to_sem();
    this->module.fill_actual(*s);
    return s;
}

std::unique_ptr<sem::FunctionDef>
ModuleChecker::visit_method(const ast::Function& n, std::unique_ptr<Entity> this_entity) {
    return this->check_function(n, std::move(this_entity));
}

std::unique_ptr<sem::FunctionDef> ModuleChecker::visit_function(const ast::Function& n) {
    return this->check_function(n, nullptr);
}

std::unique_ptr<sem::FunctionDef>
ModuleChecker::check_function(const ast::Function& n, std::unique_ptr<Entity> this_entity) {
    this->error_reporter.current_function = n.identifier.str;
    // Logger::info("Checking FunctionNode " + n.identifier);
    std::string function_name = n.identifier.str;
    this->enter_scope();
    this->scope->is_function = true;

    if (this_entity != nullptr) {
        this->scope->set("this", *this_entity);
    }
    // auto& e_const_function = this->scope->get(n.identifier);
    // auto& const_function = e_const_function.get_constfun().const_function;
    std::map<std::string, std::set<std::string>> n_instances;
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        // ast::Type& type = n.parameter_types[i];
        // ast::UTypeNode cl(type.clone());
        // make_not_generic(*cl);
        sem::Type* semt = this->make_sem_type(n.parameter_types[i]);
        for (auto& c: n.constraints) {
            for (auto& t: c.second) {
                add_typeclasses_to_generic_type(*semt, c.first, t);
                n_instances[Path(this->module.path, c.first).as_str()].insert(Path(this->module.path, t).as_str());
            }
        }
        auto te = this->make_entity_value(*semt);
        this->scope->set(n.parameter_names[i], *te);
    }
    ast::Type& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    std::unique_ptr<Entity> e = entity_from_type(returnType);
    this->scope->set("__return__", *e);
    auto bn = this->visit_block(*n.body);
    bool has_error = false;
    if (bn == nullptr) {
        has_error = true;
    }
    if (not has_error) {
        for (auto& local_var: this->scope->table) {
            if (local_var.second->is_value()) {
                bn->locals.push_back(local_var.first);
            }
        }
    }
    this->leave_scope();
    if (returnType != T_NONE) {
        if (!n.body->nodes.empty()) {
            ast::Statement& last_node = *n.body->nodes.back();
            if (last_node.ntype != StatementType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.error(std::make_unique<error::FunctionReturnLastStmt>(function_name,
                                                                                           *returnType.to_sem(),
                                                                                           last_node.start));
                has_error = true;
            }
        } else {
            this->error_reporter.error(std::make_unique<error::FunctionReturnLastStmt>(function_name,
                                                                                       *returnType.to_sem(),
                                                                                       n.start));
            has_error = true;
        }
    }
    if (has_error) {
        return nullptr;
    }
    return std::make_unique<sem::FunctionDef>(n.identifier.str, n.parameter_names, *bn, n_instances);
}