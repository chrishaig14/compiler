//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/src/TypeFunction.h"
#include "../simple_nodes/expressions/include/CallExp.h"
#include "errors/include/ErrorFunctionReturnLastStmt.h"
#include "errors/include/ErrorUnusedReturnValue.h"

sem::SNode*
make_for_snode(ast::For& node, USemanticInfoBlock& binfo, USemanticInfo& exp_info_p, std::string loop_list_var_id,
               std::string loop_index_var_id, std::string loop_list_len_var_id, sem::SNode* update_loop_index_snode) {
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

    auto& bn = (binfo->snode);

    std::vector<sem::UExp> vvv;
    vvv.push_back(std::make_unique<sem::Id>(loop_list_var_id));
    vvv.push_back(std::make_unique<sem::Id>(loop_index_var_id));
    auto* list_subscript_n = new sem::CallExp(std::make_unique<sem::Id>("libcore.libcore.List.__get_item__"),std::move(vvv));

    sem::UExp ul(list_subscript_n);
    auto loop_elem_sn = std::make_unique<sem::Declaration>(node.var, std::move(ul));
    bn->nodes.insert(bn->nodes.begin(), std::move(loop_elem_sn));

    bn->nodes.push_back(sem::USNode(update_loop_index_snode));
    auto wsn = std::make_unique<sem::While>(std::move(cn), std::move(bn));
    bbn->nodes.push_back(std::move(wsn));
    return bbn;
}

USemanticInfo Checker::visit_enum(ast::EnumNode& p_node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    Enum* enumm = ((EntityEnum&) this->scope->get(p_node.id)).enumm;
    auto esn = std::make_unique<sem::EnumDef>(enumm->path.as_str(), p_node.values);
    info.snode = std::move(esn);
    return info_u;
}

USemanticInfo Checker::visit_class(ast::Klass& node) {
    this->error_reporter.current_class = node.class_name;
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    this->add_this = true;
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
    this->add_this = false;
    auto sn = std::make_unique<sem::KlassDef>(node.class_name, node.members_ordered);
    for (auto& m: node.methods) {
        sem::USNode ms = std::move(this->visit_function(*m.second->method)->snode);
        std::unique_ptr<sem::FunctionDef> sf((sem::FunctionDef*) ms.release());
        sn->methods.emplace_back(std::move(sf));
    }
    //
    // Class* clazz = ((EntityClass&) this->scope->get(node.class_name)).clazz;
    // auto csn = std::make_unique<sem::KlassDef>(clazz->path.as_str(), node.members_ordered);
    // sn->nodes.push_back(std::move(csn));
    // sn->nodes.push_back(USNode(make_class_default_init(clazz->path.as_str(), node.members_ordered)));
    //
    // for (const auto& sm: node.static_members) {
    //     USemanticInfo sm_exp_info = this->dispatch(*sm.second.second);
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
    //     USemanticInfo method_info = this->visit_function(*method.second->method);
    //     methods_snodes.push_back(method_info->snode.release());
    // }
    //
    // for (const auto& method: node.static_methods) {
    //     this->add_this = false;
    //     USemanticInfo method_info = this->visit_function(*method.second);
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
    info.snode = std::move(sn);
    return info_u;
}

void Checker::init() {
    // Initialize module level Scope
    for (const auto& f: this->module.members) {
        std::unique_ptr<Entity> e(map_module_member_to_entity(f.second));
        this->scope->set(f.first, *e);
    }
}

USemanticInfoBlock Checker::visit_root(ast::Module& node) {
    this->init();
    // for (auto& import: node.imports) {
    //     this->visit_import(import);
    // }
    // for (auto& klass: node.classes) {
    //     this->visit_class(klass);
    // }
    // for (auto& enumm : node.enums) {
    //     this->visit_enum(enumm);
    // }
    // for (auto& function: node.functions) {
    //     this->visit_function(function);
    // }

    USemanticInfoBlock info_u = std::make_unique<SemanticInfoBlock>();
    SemanticInfoBlock& info = *info_u;
    auto sn = std::make_unique<sem::Block>();
    for (auto& n: node.all) {
        USemanticInfo sinfo_p = this->dispatch(*n);

        // sn->nodes.push_back(sinfo_p->snode);

        if (n->ntype == NodeType::BLOCK) {
        } else {
            if (sinfo_p->snode != nullptr) {
                if (sinfo_p->snode->type == sem::SNodeType::BLOCK) {
                    if (((std::unique_ptr<sem::Block>&) sinfo_p->snode)->unwrap) {
                        for (auto& nn : ((std::unique_ptr<sem::Block>&) sinfo_p->snode)->nodes) {
                            sn->nodes.push_back(std::move(nn));
                        }
                    } else {
                        sn->nodes.push_back(std::move(sinfo_p->snode));
                    }

                } else {
                    sn->nodes.push_back(std::move(sinfo_p->snode));
                }
            }
        }
        // SemanticInfo& sinfo = *sinfo_p;
        // if (n->ntype == NodeType::CALL) {
        //     // it's a function call
        //     // if return value != NoneType, then force the return value
        //
        //     if (!sinfo.is_error() && sinfo_p->entity.type != E_TYPE::NOTHING) {
        //         this->error_reporter.error(std::make_unique<ErrorUnusedReturnValue>(n->start));
        //     }
        // }
    }
    info.snode = std::move(sn);
    // node.nodes = std::move(vn);
    return info_u;
}

USemanticInfoBlock Checker::visit_block(ast::Block& node) {
    USemanticInfoBlock info_u = std::make_unique<SemanticInfoBlock>();
    SemanticInfoBlock& info = *info_u;
    auto sn = std::make_unique<sem::Block>();
    ast::VectorOfNodesU vn;
    for (auto& n: node.nodes) {
        USemanticInfo sinfo_p = this->dispatch(*n);

        // sn->nodes.push_back(sinfo_p->snode);

        if (n->ntype == NodeType::BLOCK) {
            for (auto& bnode: ((std::unique_ptr<ast::Block>&) n)->nodes) {
                vn.push_back(std::move(bnode));
            }
        } else {
            vn.push_back(std::move(n));
            if (sinfo_p->snode != nullptr) {
                if (sinfo_p->snode->type == sem::SNodeType::BLOCK) {
                    if (((std::unique_ptr<sem::Block>&) sinfo_p->snode)->unwrap) {
                        for (auto& nn : ((std::unique_ptr<sem::Block>&) sinfo_p->snode)->nodes) {
                            sn->nodes.push_back(std::move(nn));
                        }
                    } else {
                        sn->nodes.push_back(std::move(sinfo_p->snode));
                    }

                } else {
                    sn->nodes.push_back(std::move(sinfo_p->snode));
                }
            }
        }
        // SemanticInfo& sinfo = *sinfo_p;
        // if (n->ntype == NodeType::CALL) {
        //     // it's a function call
        //     // if return value != NoneType, then force the return value
        //
        //     if (!sinfo.is_error() && sinfo_p->entity.type != E_TYPE::NOTHING) {
        //         this->error_reporter.error(std::make_unique<ErrorUnusedReturnValue>(n->start));
        //     }
        // }
    }
    info.snode = std::move(sn);
    node.nodes = std::move(vn);
    return info_u;
}

USemanticInfo Checker::visit_function(ast::Function& n) {
    this->error_reporter.current_function = n.identifier;
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    // Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    this->enter_scope(function_name);
    this->scope->is_function = true;

    VectorOfStrings params = n.parameter_names;
    if (this->add_this) {
        this->scope->set("this", *this->this_entity);
        params.insert(params.begin(), "this");
    }
    if (n.implicit != nullptr) {
        Class* clazz = new Class(n.implicit->type, Path("core.implicits." + n.implicit->type));
        // clazz->class_name = ;
        auto c = std::make_unique<ConstFunction>(Path("implicit_a"),
                                                 sem::UTypeFunction((sem::TypeFunction*) n.implicit->ft->to_sem()));
        this->module.fill_actual(c->const_function_ft);
        if (n.implicit->is_static) {
            clazz->static_methods[n.implicit->method] = std::move(c);
        } else {
            clazz->methods[n.implicit->method] = std::move(c);
        }
        Entity* generic_type = new EntityClass(clazz);
        this->scope->set(n.implicit->type, *generic_type);
        params.push_back("implicit_a");
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        ast::Type& type = n.parameter_types[i];
        ast::UTypeNode cl(type.clone());
        make_not_generic(*cl);
        auto te = entity_from_type(*cl);
        this->fill_value(*((std::unique_ptr<Value>&) te));
        this->scope->set(n.parameter_names[i], *te);
        // if (!param_type.is_generic()) {
        //     if (param_type.kind == Kind::OBJECT) {
        //         ast::ObjectType& o_type = param_type.object();
        //         Entity pt = this->scope->get(o_type.id);
        //         o_type.actual_base_path = pt.clazz->path;
        //         if (type.kind == Kind::OBJECT) {
        //             std::cout << "START" << std::endl;
        //             this->assert_type_exists(type, n.start);
        //             std::cout << "END" << std::endl;
        //         }
        //         this->scope->set(n.parameter_names[i], entity_from_type(*cl));
        //     } else {
        //         this->scope->set(n.parameter_names[i], entity_from_type(type));
        //     }
        // } else {
        //     this->scope->set(n.parameter_names[i], entity_from_type(param_type));
        // }
    }
    // std::cout << "FINISH " << std::endl;

    ast::Type& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    std::cout << "here" << std::endl;
    std::unique_ptr<Entity> e = entity_from_type(returnType);
    std::cout << "there" << std::endl;
    this->scope->set("__return__", *e);
    USemanticInfoBlock body_info = this->visit_block(*n.body);
    auto& bn = body_info->snode;
    for (auto& local_var: this->scope->table) {
        if (local_var.second->type == E_TYPE::VALUE) {
            bn->locals.push_back(local_var.first);
        }
    }
    this->leave_scope();
    auto sn = std::make_unique<sem::FunctionDef>(n.path.as_vec().back(), params, std::move(bn));
    info.snode = std::move(sn);
    if (returnType != T_NONE) {
        if (!n.body->nodes.empty()) {
            ast::Node& last_node = *n.body->nodes.back();
            if (last_node.ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.error(std::make_unique<ErrorFunctionReturnLastStmt>(function_name,
                                                                                         *returnType.to_sem(),
                                                                                         last_node.start));
                return error_stub();
            }
        } else {
            this->error_reporter.error(std::make_unique<ErrorFunctionReturnLastStmt>(function_name,
                                                                                     *returnType.to_sem(),
                                                                                     n.start));
            return error_stub();
        }
    }
    return info_u;
}