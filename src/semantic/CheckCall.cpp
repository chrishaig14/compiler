//
// Created by chris on 2/5/21.
//

#include "CheckCall.h"

USemanticInfo Checker::visit_call(CallNode& n, bool is_rvalue) {
    SemanticInfo retv;
    CallSNode* sn = new CallSNode();
    retv.snode = sn;
    // Logger::info("Checking CallNode");
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
    if (fun_info_p->entity.type == E_TYPE::ERROR) {
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
    } else if (fun_info.entity.type == E_TYPE::VALUE && fun_info.entity.value->type->kind == Kind::FUNCTION) {
        function_type = fun_info.entity.value->type->function().clone();
    } else {
        this->error_reporter.call_not_a_function(n.start);
    }
    // ok
    // const FunctionType& function_type = fun_info.type().function();
    if (n.arguments.size() != function_type->param_types.size()) {
        this->error_reporter.function_call_num_args(*function_type, n.start);
        if (!function_is_generic(*function_type)) {
            retv.entity = entity_from_type(*function_type->return_type);
            return std::make_unique<SemanticInfo>(retv);
        } else {
            return error_stub();
        }
    }
    VectorOfTypes arg_types;

    std::vector<Entity> arg_entities;

    for (auto& arg: n.arguments) {
        USemanticInfo arg_type_p = this->dispatch(arg);
        arg_entities.push_back(arg_type_p->entity);
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
    } else {
        retv.entity = entity_from_type(*function_type->return_type);
        int sni = fun_info_p->this_arg != nullptr;
        for (size_t i = 0; i < n.arguments.size(); i++) {
            const TypeNode& arg_type = *arg_types[i];
            const TypeNode& param_type = *function_type->param_types[i];

            SNode* arg_rvalue_snode = this->make_rvalue(arg_entities[i], sn->arguments[sni], param_type);
            if (arg_rvalue_snode == nullptr) {
                this->error_reporter.function_call_type_mismatch(param_type,
                                                                 arg_type,
                                                                 n.arguments[i]->start,
                                                                 n.arguments[i]->end);
                continue;
            }
            sn->arguments[sni] = arg_rvalue_snode;
            sni++;
        }
    }
    // for (auto x: arg_types) {
    //     delete x;
    // }
    // if (retv.entity.value->type->kind==Kind::OBJECT){
    //     if (retv.entity.value->type->object().actual_to_string() == ""){
    //         throw std::runtime_error("This should not be empty!");
    //     }
    // }
    if (retv.entity.type == E_TYPE::NOTHING) {
        if (is_rvalue) {
            this->error_reporter.fail("Cannot use function call as expression as it doesn't return a value!", n.start);
            return error_stub();
        }
    } else {
        if (retv.entity.value->type->kind == Kind::OBJECT) {
            if (retv.entity.value->type->object().id == ".None") {
                retv.entity = Entity(E_TYPE::NOTHING);
            } else {
                this->fill_value(retv.entity.value);
            }
        }
    }
    retv.is_constant = is_def_const && args_are_constant;
    return std::make_unique<SemanticInfo>(retv);
}