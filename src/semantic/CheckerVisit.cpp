//
// Created by chris on 17/1/21.
//

#include "ModuleChecker.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/include/TypeFunction.h"
#include "../simple_nodes/expressions/include/CallExp.h"

sem::Common* make_for_snode(ast::For& node, std::unique_ptr<sem::Block>& binfo, UExpressionInfo& exp_info_p,
                            std::string loop_list_var_id, std::string loop_index_var_id,
                            std::string loop_list_len_var_id, sem::Common* update_loop_index_snode) {
    auto* bbn = new sem::Block();

    sem::UExp p_node = std::move(exp_info_p->exp_snode);
    auto dsn = std::make_unique<sem::Declaration>(loop_list_var_id, std::move(p_node));
    bbn->nodes.push_back(std::move(dsn));
    sem::UExp init_idx = std::make_unique<sem::Integer>("0");
    auto lidx_decl = std::make_unique<sem::Declaration>(loop_index_var_id, std::move(init_idx));

    bbn->nodes.push_back(std::move(lidx_decl));

    auto list_len_fn = std::make_unique<sem::Id>("libcore.libcore.List.len");
    auto list_sn = std::make_unique<sem::Id>(loop_list_var_id);
    std::vector<sem::UExp> v;
    v.emplace_back(std::move(list_sn));
    sem::UExp call_list_len_sn = std::make_unique<sem::CallExp>(std::move(list_len_fn), std::move(v));
    auto lensn = std::make_unique<sem::Declaration>(loop_list_len_var_id, std::move(call_list_len_sn));
    bbn->nodes.push_back(std::move(lensn));


    auto idxsn = std::make_unique<sem::Id>(loop_index_var_id);
    auto cmpfunsn = std::make_unique<sem::Id>("libcore.libcore.Integer.__lt__");

    auto llensn = std::make_unique<sem::Id>(loop_list_len_var_id);


    std::vector<sem::UExp> vv;
    vv.push_back(std::move(idxsn));
    vv.push_back(std::move(llensn));
    auto cn = std::make_unique<sem::CallExp>(std::move(cmpfunsn), std::move(vv));

    auto& bn = binfo;

    std::vector<sem::UExp> vvv;
    vvv.push_back(std::make_unique<sem::Id>(loop_list_var_id));
    vvv.push_back(std::make_unique<sem::Id>(loop_index_var_id));
    auto* list_subscript_n = new sem::CallExp(std::make_unique<sem::Id>("libcore.libcore.List.__get_item__"),
                                              std::move(vvv));

    sem::UExp ul(list_subscript_n);
    auto loop_elem_sn = std::make_unique<sem::Declaration>(node.var, std::move(ul));
    bn->nodes.insert(bn->nodes.begin(), std::move(loop_elem_sn));

    bn->nodes.push_back(sem::UCommon(update_loop_index_snode));
    auto wsn = std::make_unique<sem::While>(std::move(cn), std::move(bn));
    bbn->nodes.push_back(std::move(wsn));
    return bbn;
}

std::unique_ptr<sem::EnumDef> ModuleChecker::visit_enum(ast::EnumNode& p_node) {
    sem::UCommon info_u;
    auto esn = std::make_unique<sem::EnumDef>(p_node.id, p_node.values);
    return esn;
}

std::unique_ptr<sem::KlassDef> ModuleChecker::visit_class(ast::Klass& node) {
    this->error_reporter.current_class = node.class_name;
    ast::VectorOfTypes tp;
    std::string cn = node.class_name;
    for (const auto& type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    ast::VectorOfTypes members_ordered_types;
    for (const auto& mt: node.members) {
        ast::Type& t = *mt.second;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
    }
    auto sn = std::make_unique<sem::KlassDef>(node.class_name, node.members_ordered);
    this->add_this = true;
    // this->this_entity = std::unique_ptr<Entity>(this->entity_value_from_actual_base_path_no_generic(Path(this->module.path,
    //                                                                                                      node.class_name)).clone());
    // this->fill_value(this->this_entity->get_value());
    Path p(this->module.path, node.class_name);
    sem::TypeObject ot (p.basname(), p);
    // auto v = std::make_unique<Value>(ot);
    // this->fill_value(*v);
    this->this_entity = this->make_entity_value(ot);

    for (auto& m: node.methods) {
        auto ms = this->visit_function(*m.second->method);
        std::unique_ptr<sem::FunctionDef> sf((sem::FunctionDef*) ms.release());
        sn->methods.emplace_back(std::move(sf));
    }
    this->this_entity.reset();
    this->add_this = false;

    for (auto& m: node.static_methods) {
        auto ms = this->visit_function(*m.second);
        std::unique_ptr<sem::FunctionDef> sf((sem::FunctionDef*) ms.release());
        sn->static_methods.emplace_back(std::move(sf));
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

void ModuleChecker::init() {
    // Initialize module level Scope
    for (const auto& f: this->module.members) {
        std::unique_ptr<Entity> e(map_module_member_to_entity(*f.second));
        this->scope->set(f.first, *e);
    }
}

std::unique_ptr<sem::Module> ModuleChecker::visit_root(ast::Module& node) {
    this->init();
    auto sn = std::make_unique<sem::Module>();
    for (auto& n: node.all) {
        if (n->ntype == TopNodeType::IMPORT) {
            continue;
        }
        std::unique_ptr<sem::Top> sinfo_p = this->dispatch_top(*n);
        sn->nodes.push_back(std::move(sinfo_p));
    }
    return sn;
}

std::unique_ptr<sem::Block> ModuleChecker::visit_block(ast::Block& node) {
    auto sn = std::make_unique<sem::Block>();
    for (auto& n: node.nodes) {
        sem::UCommon sinfo_p = this->dispatch(*n);
        if (sinfo_p != nullptr) {
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
    return sn;
}

std::unique_ptr<sem::FunctionDef> ModuleChecker::visit_function(ast::Function& n) {
    this->error_reporter.current_function = n.identifier;
    // Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    this->enter_scope(function_name);
    this->scope->is_function = true;

    if (this->add_this) {
        this->scope->set("this", *this->this_entity);
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        ast::Type& type = n.parameter_types[i];
        ast::UTypeNode cl(type.clone());
        make_not_generic(*cl);
        sem::UType p_type(cl->to_sem());
        auto te = this->make_entity_value(*p_type);
        this->scope->set(n.parameter_names[i], *te);
    }
    ast::Type& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    std::unique_ptr<Entity> e = entity_from_type(returnType);
    this->scope->set("__return__", *e);
    auto bn = this->visit_block(*n.body);
    for (auto& local_var: this->scope->table) {
        if (local_var.second->is_value()) {
            bn->locals.push_back(local_var.first);
        }
    }
    this->leave_scope();
    if (returnType != T_NONE) {
        if (!n.body->nodes.empty()) {
            ast::Statement& last_node = *n.body->nodes.back();
            if (last_node.ntype != StatementType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.error(std::make_unique<ErrorFunctionReturnLastStmt>(function_name,
                                                                                         *returnType.to_sem(),
                                                                                         last_node.start));
                return nullptr;
            }
        } else {
            this->error_reporter.error(std::make_unique<ErrorFunctionReturnLastStmt>(function_name,
                                                                                     *returnType.to_sem(),
                                                                                     n.start));
            return nullptr;
        }
    }
    return std::make_unique<sem::FunctionDef>(n.path.basname(), n.parameter_names, std::move(bn));
}