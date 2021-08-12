//
// Created by chris on 2/5/21.
//

#include "CheckCall.h"
#include "errors/ErrorTypeMismatch.h"

USemanticInfo Checker::visit_call(CallNode& n, bool is_rvalue) {
    SemanticInfo retv;
    auto* sn = new CallSNode(nullptr, std::vector<SNode*>());
    retv.snode = sn;
    // Logger::info("Checking CallNode");
    bool old_is_call = this->is_call;
    this->is_call = true;
    USemanticInfo fun_info_p = this->dispatch(n.function);
    this->is_call = old_is_call;
    if (fun_info_p->is_error()) {
        return error_stub();
    }
    if (fun_info_p->this_arg != nullptr) {
        sn->arguments = {fun_info_p->this_arg};
    }
    bool is_def_const = n.function->ntype == NodeType::DEF_CONST;
    bool args_are_constant = true;
    SemanticInfo& fun_info = *fun_info_p;
    // bool is_a_method = false;
    // Node* object_node;
    sn->function = fun_info.snode;
    FunctionType* function_type = nullptr;
    if (fun_info.entity.type == E_TYPE::CONST_FUNCTION) {
        function_type = fun_info.entity.const_function->ft->clone();
    } else if (fun_info.entity.type == E_TYPE::VALUE && fun_info.entity.value->type->kind == Kind::FUNCTION) {
        function_type = fun_info.entity.value->type->function().clone();
    } else {
        this->error_reporter.call_not_a_function(n);
        return error_stub();
    }
    // ok
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
    bool has_error = check_arguments(n, sn, arg_types, arg_entities);
    if (has_error) {
        return error_stub();
    }
    std::cout << "Calling function of type: " << function_type->to_string() << std::endl;
    if (function_type->is_generic()) {
        // mangle the generic types in function_type to prevent collisions
        mangle_generic_names(function_type);
        std::cout << "Mangled function of type: " << function_type->to_string() << std::endl;
        std::map<std::string, TypeNode*> all_substitutions;
        VectorOfTypes copy_arg_types;
        for (auto x: arg_types) {
            copy_arg_types.push_back(x->clone());
        }
        USemanticInfo inf = this->match_arguments_to_generic_function(*function_type,
                                                                      copy_arg_types,
                                                                      all_substitutions);
        for (auto s: all_substitutions) {
            std::cout << "substitution: " << s.first << " -> " << s.second->to_string() << std::endl;
        }
        for (auto x: arg_types) {
            std::cout << "ARG: " << x->to_string() << std::endl;
        }
        std::cout << "DONE" << std::endl;
        if (fun_info.entity.type == E_TYPE::CONST_FUNCTION) {
            ConstFunction* full_function = fun_info.entity.const_function;
            if (full_function->implicit != nullptr) {
                ObjectType* it = new ObjectType(full_function->implicit->type);
                it->is_generic_param = true;
                mangle_generic_names(it);
                TypeNode* tt = all_substitutions.at(it->id);
                Entity e = entity_from_type(*tt);
                assert(e.type == E_TYPE::VALUE);
                this->fill_value(e.value);
                std::cout << "calling function with implicit: " << full_function->implicit->type << "."
                          << full_function->implicit->method << " : " << full_function->implicit->ft->to_string()
                          << std::endl;
                ConstFunction* implicit_arg = nullptr;
                if (full_function->implicit->is_static) {
                    auto it = e.value->clazz->static_methods.find(full_function->implicit->method);
                    if (it == e.value->clazz->static_methods.end()) {
                        std::cout << "ERROR class has no implicit STATIC method: " << full_function->implicit->method
                                  << std::endl;
                        exit(1);
                    }
                    implicit_arg = it->second;
                } else {
                    auto it = e.value->clazz->methods.find(full_function->implicit->method);
                    if (it == e.value->clazz->methods.end()) {
                        std::cout << "ERROR class has no implicit method: " << full_function->implicit->method
                                  << std::endl;
                        exit(1);
                    }
                    // implicit_arg = e.value->clazz->methods.at(full_function->implicit->method);
                    implicit_arg = it->second;
                }
                FunctionType* implicit_param = full_function->implicit->ft;

                std::cout << "FULL FUNCTION: " << full_function->ft->to_string() << std::endl;
                std::cout << "IMPLICIT FUNCTION PARAM: " << implicit_param->to_string() << std::endl;
                std::cout << "IMPLICIT FUNCTION ARG: " << implicit_arg->ft->to_string() << std::endl;

                FunctionType* with_implicit_param = full_function->ft->clone();
                with_implicit_param->param_types.push_back(full_function->implicit->ft);
                VectorOfTypes with_implicit_arg;
                for (auto x: arg_types) {
                    std::cout << "ARG: " << x->to_string() << std::endl;
                    with_implicit_arg.push_back(x->clone());
                }
                with_implicit_arg.push_back(implicit_arg->ft->clone());
                USemanticInfo inf__ = this->match_arguments_to_generic_function(*with_implicit_param,
                                                                                with_implicit_arg,
                                                                                all_substitutions);
                // USemanticInfo inf__ = this->match_arguments_to_generic_function(*with_implicit_param,
                //                                                                 implicit_arg->ft->param_types,
                //                                                                 all_substitutions);
                if (inf__->is_error()) {
                    std::cout << "Error passing implicit method!" << std::endl;
                    exit(1);
                }

                sn->arguments.push_back(new IdSNode(implicit_arg->path.as_str()));
                // assert(*(fff->ft) == *cf->ft);
            }
        }
        if (inf->is_error()) {
            std::cout << "ERRORR CANNOT CALL " << std::endl;
        }
        retv.entity = inf->entity;
    } else {
        this->process_function_arguments(retv, arg_entities, sn, n, function_type, fun_info_p.get());

    }
    return make_return_info(n, is_rvalue, retv, is_def_const, args_are_constant);
}

USemanticInfo Checker::make_return_info(const CallNode& n, bool is_rvalue, SemanticInfo& retv, bool is_def_const,
                                        bool args_are_constant) {
    if (retv.entity.type == E_TYPE::NOTHING) {
        if (is_rvalue) {
            error_reporter.expected_expression(retv.entity, n);
            return error_stub();
        }
    } else if (retv.entity.type == E_TYPE::VALUE) {
        if (retv.entity.value->type->kind == Kind::OBJECT) {
            if (retv.entity.value->type->object().id == ".None") {
                retv.entity = Entity(E_TYPE::NOTHING);
            } else {
                fill_value(retv.entity.value);
            }
        }
    }
    retv.is_constant = is_def_const && args_are_constant;
    return std::make_unique<SemanticInfo>(retv);
}

bool Checker::check_arguments(CallNode& n, CallSNode* sn, VectorOfTypes& arg_types, std::vector<Entity>& arg_entities) {
    bool has_error;
    for (auto& arg: n.arguments) {
        USemanticInfo arg_type_p = dispatch(arg);
        if (arg_type_p->is_error()) {
            has_error = true;
            continue;
        }

        arg_entities.push_back(arg_type_p->entity);
        sn->arguments.push_back(arg_type_p->snode);
        Entity arg_entity = arg_type_p->entity;

        if (arg_entity.type == E_TYPE::CLASS || arg_entity.type == E_TYPE::PACKAGE ||
            arg_entity.type == E_TYPE::MODULE || arg_entity.type == E_TYPE::ENUM ||
            arg_entity.type == E_TYPE::NOTHING) {
            has_error = true;
            error_reporter.expected_expression(arg_entity, *arg);
            continue;
        }


        TypeNode& arg_type = *get_entity_type(arg_entity);
        arg_types.push_back(arg_type.clone());
        n.arg_types.push_back(arg_type.clone());
    }
    return has_error;
}

void
Checker::process_function_arguments(SemanticInfo& retv, std::vector<Entity>& arg_entities, CallSNode* sn, CallNode& n,
                                    FunctionType* function_type, SemanticInfo* fun_info_p) {
    retv.entity = entity_from_type(*function_type->return_type);
    int sni = static_cast<int>(fun_info_p->this_arg != nullptr);
    for (size_t i = 0; i < n.arguments.size(); i++) {
        // const TypeNode& arg_type = *arg_types[i];
        const TypeNode& param_type = *function_type->param_types[i];

        SNode* arg_rvalue_snode = this->make_rvalue(arg_entities[i], sn->arguments[sni], param_type);
        if (arg_rvalue_snode == nullptr) {
            this->error_reporter.error(ErrorTypeMismatch(param_type, *n.arguments[i], arg_entities[i]));
            continue;
        }
        sn->arguments[sni] = arg_rvalue_snode;
        sni++;
    }
}