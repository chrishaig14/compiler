//
// Created by chris on 2/5/21.
//

#include "CheckCall.h"

USemanticInfo Checker::visit_call(CallNode& n) {
    SemanticInfo retv;
    CallSNode* sn = new CallSNode();
    retv.snode = sn;
    Logger::info("Checking CallNode");
    bool old_is_call = this->is_call;
    this->is_call = true;
    USemanticInfo fun_info_p = this->dispatch(n.function);
    this->is_call = old_is_call;
    if (fun_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (fun_info_p->this_arg != nullptr) {
        sn->arguments = {fun_info_p->this_arg};
    }
    bool is_def_const = n.function->ntype == NodeType::DEF_CONST;
    bool args_are_constant = true;
    if (fun_info_p->is_error) {
        return error_stub();
    }
    SemanticInfo& fun_info = *fun_info_p;
    // bool is_a_method = false;
    // Node* object_node;
    if (fun_info.is_class_method) {
        MemberNode& member_node = n.function->member();
        if (member_node.s_child == "init") {
            // const FunctionType& ftn = fun_info.type().function();
            // FunctionType& copy_ftn = ftn.clone()->function();
            // retv.set_type(*copy_ftn.clone());
            // object_node = member_node.parent;
        } else {
            // n.function = new IdNode(this->map[fun_info.class_info->class_name + "." + member_node.s_child],
            //                         POS_NONE,
            //                         POS_NONE);
            // const FunctionType& ftn = fun_info.type().function();
            // FunctionType& copy_ftn = ftn.clone()->function();
            // copy_ftn.param_types.insert(copy_ftn.param_types.begin(), TYPE(fun_info.class_info->class_name, {}));
            // retv.set_type(*copy_ftn.clone());
            // object_node = member_node.parent;
        }
    } else {
        sn->function = fun_info.snode;
    }
    FunctionType* function_type = nullptr;
    if (fun_info.entity.type == E_TYPE::CONST_FUNCTION) {
        function_type = fun_info.entity.const_function->ft->clone();
    } else if (fun_info.entity.type == E_TYPE::FUNCTION_VALUE) {
        function_type = fun_info.entity.function_value->ft->clone();
    } else {
        this->error_reporter.call_not_a_function(n.start);
    }
    // ok
    // const FunctionType& function_type = fun_info.type().function();
    if (n.arguments.size() != function_type->param_types.size()) {
        this->error_reporter.function_call_num_args(*function_type, n.start);
        if (!function_is_generic(*function_type)) {
            // retv.set_type(*function_type->return_type);
            return std::make_unique<SemanticInfo>(retv);
        } else {
            return error_stub();
        }
    }
    VectorOfTypes arg_types;
    for (auto& arg: n.arguments) {
        USemanticInfo arg_type_p = this->dispatch(arg);
        sn->arguments.push_back(arg_type_p->snode);
        Entity arg_entity = arg_type_p->entity;


        if (arg_entity.type == E_TYPE::CLASS || arg_entity.type == E_TYPE::PACKAGE ||
            arg_entity.type == E_TYPE::MODULE) {
            std::cout << ("Error can't pass as argument") << std::endl;
            return error_stub();
        }
        if (arg_entity.type == E_TYPE::ERROR) {
            return error_stub();
        }

        TypeNode& arg_type = *get_entity_type(arg_entity);
        // if (!arg_type_p->is_constant) {
        //     args_are_constant = false;
        // }
        arg_types.push_back(arg_type.clone());
        n.arg_types.push_back(arg_type.clone());
    }

    if (function_type->is_generic()) {
        // mangle the generic types in function_type to prevent collisions
        mangle_generic_names(function_type);
        retv.entity = match_arguments_to_generic_function(*function_type, arg_types).entity;
        // for(auto x: arg_types){
        //     delete x;
        // }
    } else {
        retv.entity = entity_from_type(*function_type->return_type);
        retv.entity.object_value->ot = (ObjectType*) function_type->return_type->clone();
        for (size_t i = 0; i < n.arguments.size(); i++) {
            const TypeNode& arg_type = *arg_types[i];
            const TypeNode& param_type = *function_type->param_types[i];
            if (param_type == arg_type) {
                continue;
            }
            const std::string& arg_path_as_str = arg_type.object().actual_base_path.as_str();
            const std::string& param_path_as_str = param_type.object().actual_base_path.as_str();
            if (arg_path_as_str != param_path_as_str) {
                // if (arg_type != param_type) {
                if (param_type.kind == Kind::OBJECT && param_type.object().id == "Union") {
                    int type_index = target_union_type(param_type.object(), arg_type);
                    if (type_index == -1) {
                        this->error_reporter.function_call_type_mismatch(param_type,
                                                                         arg_type,
                                                                         n.arguments[i]->start,
                                                                         n.arguments[i]->end);
                    } else {
                        int fixed_index = i + (fun_info_p->this_arg != nullptr);
                        sn->arguments[fixed_index] = make_union_wrapper(type_index, sn->arguments[fixed_index]);
                    }
                    return std::make_unique<SemanticInfo>(retv);
                } else if (arg_type.kind != Kind::UNKNOWN) {
                    this->error_reporter.function_call_type_mismatch(param_type,
                                                                     arg_type,
                                                                     n.arguments[i]->start,
                                                                     n.arguments[i]->end);
                }
                // }
            }
        }
    }
    for (auto x: arg_types) {
        delete x;
    }

    retv.is_constant = is_def_const && args_are_constant;
    return std::make_unique<SemanticInfo>(retv);
}