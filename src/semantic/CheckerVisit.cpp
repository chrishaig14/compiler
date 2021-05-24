//
// Created by chris on 17/1/21.
//

#include "Checker.h"


SNode* Checker::make_for_snode(ForNode& node, USemanticInfo& binfo, USemanticInfo& exp_info_p) {
    BlockSNode* bbn = new BlockSNode();
    DeclarationSNode* dsn = new DeclarationSNode();
    WhileSNode* wsn = new WhileSNode();


    dsn->identifier = this->loop_list_var_id;
    dsn->expression = exp_info_p->snode;
    bbn->nodes.push_back(dsn);
    DeclarationSNode* lidx_decl = new DeclarationSNode();
    lidx_decl->identifier = this->loop_index_var_id;
    IntegerSNode* init_idx = new IntegerSNode(std::string());
    init_idx->str = "0";
    lidx_decl->expression = init_idx;

    bbn->nodes.push_back(lidx_decl);

    DeclarationSNode* lensn = new DeclarationSNode();
    lensn->identifier = this->loop_list_len_var_id;
    CallSNode* call_list_len_sn = new CallSNode();
    IdSNode* list_len_fn = new IdSNode("core.List.len");
    call_list_len_sn->function = list_len_fn;
    IdSNode* list_sn = new IdSNode(this->loop_list_var_id);
    call_list_len_sn->arguments = {list_sn};
    lensn->expression = call_list_len_sn;
    bbn->nodes.push_back(lensn);


    IdSNode* idxsn = new IdSNode(this->loop_index_var_id);
    CallSNode* cn = new CallSNode();
    IdSNode* cmpfunsn = new IdSNode("core.core.Integer.__lt__");

    IdSNode* llensn = new IdSNode(this->loop_list_len_var_id);


    cn->function = cmpfunsn;
    cn->arguments = {idxsn, llensn};

    wsn->condition = cn;

    bbn->nodes.push_back(wsn);


    BlockSNode* bn = (BlockSNode*) (binfo->snode);
    DeclarationSNode* loop_elem_sn = new DeclarationSNode();

    CallSNode* list_subscript_n = new CallSNode();
    list_subscript_n->function = new IdSNode("core.List.__get_item__");
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_list_var_id));
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_index_var_id));


    loop_elem_sn->expression = list_subscript_n;
    loop_elem_sn->identifier = node.var;
    bn->nodes.insert(bn->nodes.begin(), loop_elem_sn);

    bn->nodes.push_back(this->update_loop_index_snode);
    wsn->body = bn;
    return bbn;
}

USemanticInfo Checker::visit_enum(EnumNode& node) {
    SemanticInfo info;
    EnumSNode* esn = new EnumSNode();
    Enum* enumm = this->scope->get(node.id).enumm;
    esn->id = enumm->path.as_str();
    esn->values = node.values;
    info.snode = esn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_class(ClassNode& node) {
    this->error_reporter.current_class = node.class_name;
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;

    ClassSNode* csn = new ClassSNode();

    Class* clazz = this->scope->get(node.class_name).clazz;
    csn->identifier = clazz->path.as_str();
    sn->nodes.push_back(csn);
    sn->nodes.push_back(make_class_default_init(clazz->path.as_str(), node.members_ordered));

    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
        csn->members.push_back(mt);
    }

    for (auto sm: node.static_members) {
        USemanticInfo sm_exp_info = this->dispatch(sm.second.second);
        if (*sm.second.first != *sm_exp_info->entity.value->type) {
            this->error_reporter.fail("Err: cannt initialize static member of type " + sm.second.first->to_string() +
                                     " with expression of type " + sm_exp_info->entity.value->type->to_string());
        }
        if (!sm_exp_info->is_constant) {
            this->error_reporter.fail("Error: cannot initialize static member with non constant expression!");
        }
    }

    std::vector<SNode*> methods_snodes;
    std::vector<SNode*> static_methods_snodes;

    for (auto method: node.methods) {
        this->add_this = true;
        ObjectType* vt = new ObjectType(node.class_name);
        vt->actual_base_path = clazz->path;
        Value* val = new Value(vt);
        this->this_entity = Entity(val);
        val->metatype = Meta::CLASS;
        val->clazz = clazz;
        // method.second->path = clazz->path + "." + method.second->identifier;
        USemanticInfo method_info = this->visit_function(*method.second);
        methods_snodes.push_back(method_info->snode);
    }

    for (auto method: node.static_methods) {
        this->add_this = false;
        USemanticInfo method_info = this->visit_function(*method.second);
        static_methods_snodes.push_back(method_info->snode);
    }

    for (auto m: methods_snodes) {
        sn->nodes.push_back(m);
    }
    for (auto m: static_methods_snodes) {
        sn->nodes.push_back(m);
    }

    this->add_this = true;

    this->add_this = false;
    this->error_reporter.current_class = "";
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_root(BlockNode& node) {
    this->error_reporter.__file__ = this->__file__;
    this->error_reporter.code_lines = code_lines;
    // Initialize module level Scope
    for (auto f: this->module->flirpins) {
        this->scope->set(f.first, map_flirpin_to_entity(f.second));
    }
    // for (auto i: this->module->imports) {
    //     this->scope->set(i.first, i.second);
    // }

    USemanticInfo info = this->visit_block(node);
    this->root_snode = static_cast<BlockSNode*>(info->snode);
    SemanticInfo f;
    return std::make_unique<SemanticInfo>(f);
}

USemanticInfo Checker::visit_block(BlockNode& node) {
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    VectorOfNodes vn;
    for (auto& n: node.nodes) {
        USemanticInfo sinfo_p = this->dispatch(n);

        // sn->nodes.push_back(sinfo_p->snode);

        if (n->ntype == NodeType::BLOCK) {
            for (auto bnode: n->block().nodes) {
                vn.push_back(bnode);
            }
        } else {
            vn.push_back(n);
            if (sinfo_p->snode != nullptr) {
                if (sinfo_p->snode->type == SNodeType::BLOCK) {
                    for (auto nn : ((BlockSNode*) sinfo_p->snode)->nodes) {
                        sn->nodes.push_back(nn);
                    }
                } else {
                    sn->nodes.push_back(sinfo_p->snode);
                }
            }
        }
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value

            if (!sinfo.is_error() && sinfo_p->entity.type != E_TYPE::NOTHING) {
                this->error_reporter.unused_return_value(n->start);
            }
        }
    }
    node.nodes = vn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_function(FunctionNode& n) {
    this->error_reporter.current_function = n.identifier;
    SemanticInfo info;
    FunctionSNode* sn = new FunctionSNode();
    info.snode = sn;
    // Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    sn->identifier = n.path.as_str();
    sn->params = n.parameter_names;
    this->enter_scope(function_name);
    this->scope->is_function = true;
    if (this->add_this) {
        this->scope->set("this", this->this_entity);
        sn->params.insert(sn->params.begin(), "this");
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        TypeNode* cl = type.clone();
        make_not_generic(cl);
        auto te = entity_from_type(*cl);
        this->fill_value(te.value);
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
    sn->body = static_cast<BlockSNode*>(body_info->snode);

    if (returnType != T_NONE) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes.back();
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.function_return_last_stmt(function_name, returnType, last_node->start);
                return error_stub();
            }
        } else {
            this->error_reporter.function_return_last_stmt(function_name, returnType, n.start);
            return error_stub();
        }
    }
    this->leave_scope();
    return std::make_unique<SemanticInfo>(info);
}