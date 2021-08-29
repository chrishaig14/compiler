//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "errors/ErrorFunctionReturnLastStmt.h"
#include "errors/ErrorUnusedReturnValue.h"


SNode* make_for_snode(ast::For& node, USemanticInfo& binfo, USemanticInfo& exp_info_p, std::string loop_list_var_id,
                      std::string loop_index_var_id, std::string loop_list_len_var_id, SNode* update_loop_index_snode) {
    auto* bbn = new Block();

    USNode p_node(exp_info_p->snode);
    auto* dsn = new DeclarationSNode(loop_list_var_id, p_node);
    bbn->nodes.push_back(dsn);
    USNode init_idx = std::make_unique<IntegerSNode>("0");
    auto* lidx_decl = new DeclarationSNode(loop_index_var_id, init_idx);

    bbn->nodes.push_back(lidx_decl);

    auto* list_len_fn = new IdSNode("core.core.List.len");
    auto* list_sn = new IdSNode(loop_list_var_id);
    USNode call_list_len_sn = std::make_unique<Call>(list_len_fn, std::vector<SNode*>{list_sn});
    auto* lensn = new DeclarationSNode(loop_list_len_var_id, call_list_len_sn);
    bbn->nodes.push_back(lensn);


    auto* idxsn = new IdSNode(loop_index_var_id);
    auto* cmpfunsn = new IdSNode("core.core.Integer.__lt__");

    auto* llensn = new IdSNode(loop_list_len_var_id);


    auto cn = std::make_unique<Call>(cmpfunsn, std::vector<SNode*>{idxsn, llensn});

    auto* bn = (Block*) (binfo->snode);

    auto* list_subscript_n = new Call(new IdSNode("core.core.List.__get_item__"),
                                           {new IdSNode(loop_list_var_id), new IdSNode(loop_index_var_id)});

    USNode ul(list_subscript_n);
    auto* loop_elem_sn = new DeclarationSNode(node.var, ul);
    bn->nodes.insert(bn->nodes.begin(), loop_elem_sn);

    bn->nodes.push_back(update_loop_index_snode);
    auto* wsn = new While(std::move(cn), bn);
    bbn->nodes.push_back(wsn);
    return bbn;
}

USemanticInfo Checker::visit_enum(ast::EnumNode& p_node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    auto* esn = new EnumSNode();
    Enum* enumm = ((EntityEnum&) this->scope->get(p_node.id)).enumm;
    esn->id = enumm->path.as_str();
    esn->values = p_node.values;
    info.snode = esn;
    return info_u;
}

USemanticInfo Checker::visit_class(ast::Klass& node) {
    this->error_reporter.current_class = node.class_name;
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    auto* sn = new Block(true);
    info.snode = sn;
    this->add_this = true;
    VectorOfTypes tp;
    for (const auto& type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;

    for (const auto& mt: node.members) {
        TypeNode& t = *mt.second;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
    }

    Class* clazz = ((EntityClass&) this->scope->get(node.class_name)).clazz;
    auto* csn = new Klass(clazz->path.as_str(), node.members_ordered);
    sn->nodes.push_back(csn);
    sn->nodes.push_back(make_class_default_init(clazz->path.as_str(), node.members_ordered));

    for (const auto& sm: node.static_members) {
        USemanticInfo sm_exp_info = this->dispatch(*sm.second.second);
        if (*sm.second.first != *((EntityValue&) sm_exp_info->entity).value->type) {
            this->error_reporter.fail("Err: cannt initialize static member of type " + sm.second.first->to_string() +
                                      " with expression of type " +
                                      ((EntityValue&) sm_exp_info->entity).value->type->to_string());
        }
        if (!sm_exp_info->is_constant) {
            this->error_reporter.fail("Error: cannot initialize static member with non constant expression!");
        }
    }

    std::vector<SNode*> methods_snodes;
    std::vector<SNode*> static_methods_snodes;

    for (const auto& method: node.methods) {
        auto* vt = new ObjectType(node.class_name);
        vt->actual_base_path = clazz->path;
        auto val = std::make_unique<Value>(vt);
        this->add_this = true;
        this->this_entity = new EntityValue(std::move(val));
        val->metatype = Meta::CLASS;
        val->clazz = clazz;
        // method.second->path = clazz->path + "." + method.second->identifier;
        USemanticInfo method_info = this->visit_function(*method.second->method);
        methods_snodes.push_back(method_info->snode);
    }

    for (const auto& method: node.static_methods) {
        this->add_this = false;
        USemanticInfo method_info = this->visit_function(*method.second);
        static_methods_snodes.push_back(method_info->snode);
    }

    for (auto* m: methods_snodes) {
        sn->nodes.push_back(m);
    }
    for (auto* m: static_methods_snodes) {
        sn->nodes.push_back(m);
    }

    this->add_this = true;

    this->add_this = false;
    this->error_reporter.current_class = "";
    return info_u;
}

void Checker::init() {
    this->error_reporter.__file__ = this->module.abs_path;
    this->error_reporter.code_lines = this->module.code_lines;
    // Initialize module level Scope
    for (const auto& f: this->module.flirpins) {
        this->scope->set(f.first, map_flirpin_to_entity(f.second));
    }
}

USemanticInfo Checker::visit_root(ast::Block& node) {
    this->init();
    return this->visit_block(node);
}

USemanticInfo Checker::visit_block(ast::Block& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    auto* sn = new Block();
    info.snode = sn;
    VectorOfNodesU vn;
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
                if (sinfo_p->snode->type == SNodeType::BLOCK) {
                    if (((Block*) sinfo_p->snode)->unwrap) {
                        for (auto* nn : ((Block*) sinfo_p->snode)->nodes) {
                            sn->nodes.push_back(nn);
                        }
                    } else {
                        sn->nodes.push_back(sinfo_p->snode);
                    }

                } else {
                    sn->nodes.push_back(sinfo_p->snode);
                }
            }
        }
        // SemanticInfo& sinfo = *sinfo_p;
        // if (n->ntype == NodeType::CALL) {
        //     // it's a function call
        //     // if return value != NoneType, then force the return value
        //
        //     if (!sinfo.is_error() && sinfo_p->entity.type != E_TYPE::NOTHING) {
        //         this->error_reporter.error(ErrorUnusedReturnValue(n->start));
        //     }
        // }
    }
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
        this->scope->set("this", this->this_entity);
        params.insert(params.begin(), "this");
    }
    if (n.implicit != nullptr) {
        Class* clazz = new Class();
        clazz->class_name = n.implicit->type;
        ConstFunction* c = new ConstFunction(Path("implicit_a"), n.implicit->ft);
        this->module.fill_actual(c->ft);
        if (n.implicit->is_static) {
            clazz->static_methods[n.implicit->method] = c;
        } else {
            clazz->methods[n.implicit->method] = c;
        }
        Entity* generic_type = new EntityClass(clazz);
        this->scope->set(n.implicit->type, generic_type);
        params.push_back("implicit_a");
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        TypeNode* cl = type.clone();
        make_not_generic(cl);
        auto te = entity_from_type(*cl);
        this->fill_value(*((EntityValue*) te)->value);
        this->scope->set(n.parameter_names[i], te);
        // if (!param_type.is_generic()) {
        //     if (param_type.kind == Kind::OBJECT) {
        //         ObjectType& o_type = param_type.object();
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

    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    this->scope->set("__return__", entity_from_type(returnType));
    USemanticInfo body_info = this->visit_block(*n.body);
    Block* bn = (Block*) (body_info->snode);
    for (auto local_var: this->scope->table) {
        if (local_var.second->type == E_TYPE::VALUE) {
            bn->locals.push_back(local_var.first);
        }
    }
    this->leave_scope();
    auto* sn = new FunctionSNode(n.path.as_str(), params, bn);
    info.snode = sn;
    if (returnType != T_NONE) {
        if (!n.body->nodes.empty()) {
            ast::Node& last_node = *n.body->nodes.back();
            if (last_node.ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.error(ErrorFunctionReturnLastStmt(function_name, returnType, last_node.start));
                return error_stub();
            }
        } else {
            this->error_reporter.error(ErrorFunctionReturnLastStmt(function_name, returnType, n.start));
            return error_stub();
        }
    }
    return info_u;
}