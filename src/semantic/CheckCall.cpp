//
// Created by chris on 2/5/21.
//

#include "CheckCall.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../ast/expressions/include/CallExp.h"
#include "../ast/statements/Call.h"
#include "../simple_nodes/common/src/TypeFunction.h"

USemanticInfo Checker::visit_call(ast::Call& n) {
    // auto s = this->visit_callexp(n, false);
    // UExpressionInfo u = std::make_unique<ExpressionInfo>();
    // u->exp_snode = std::move(s->exp_snode);
    // return u;
    return nullptr;
}

UExpressionInfo Checker::visit_callexp(ast::CallExp& n, bool is_rvalue) {
    auto retv_p = std::make_unique<ExpressionInfo>();
    auto& retv = *retv_p;
    bool old_is_call = this->is_call;
    this->is_call = true;
    UExpressionInfo fun_info_p = this->dispatch_rvalue(n.function);
    this->is_call = old_is_call;
    if (fun_info_p->is_error()) {
        return exp_error_stub();
    }

    bool is_def_const = n.function.ntype == ExpNodeType::DEF_CONST;
    bool args_are_constant = true;
    ExpressionInfo& fun_info = *fun_info_p;
    // bool is_a_method = false;
    // Node* object_node;
    if ((fun_info.entity.get().e_type != E_TYPE::CONST_FUNCTION && fun_info_p->entity.get().e_type != E_TYPE::VALUE)) {
        // this->error_reporter.error(std::make_unique<ErrorNotAFunction>(n));
        return exp_error_stub();
    }
    const sem::TypeFunction& function_type = get_function_type(fun_info);
    // ok
    if (n.arguments.size() != function_type.param_types.size()) {
        this->error_reporter.error(std::make_unique<ErrorFunctionCallNumArgs>(&function_type, n.start));
        if (!function_is_generic(function_type)) {
            retv.set_entity(entity_from_type(*function_type.return_type->to_ast()));
            return retv_p;
        } else {
            return exp_error_stub();
        }
    }

    std::vector<std::unique_ptr<Entity>> arg_entities;
    std::vector<sem::UExp> arguments;
    bool has_error = check_arguments(n, arguments, arg_entities);
    if (has_error) {
        return exp_error_stub();
    }
    std::cout << "Calling function of type: " << function_type.to_string() << std::endl;


    /*
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
        if (fun_info.entity.get().type == E_TYPE::CONST_FUNCTION) {
            ConstFunction* full_function = ((EntityConstFunction&) fun_info.entity.get()).const_function;
            if (full_function->implicit != nullptr) {
                ast::ObjectType* it = new ast::ObjectType(full_function->implicit->type);
                it->is_generic_param = true;
                mangle_generic_names(it);
                TypeNode* tt = all_substitutions.at(it->id);
                Entity* ep = entity_from_type(*tt);
                assert(ep->type == E_TYPE::VALUE);
                EntityValue& e = (EntityValue&) *ep;
                this->fill_value(*e.value);
                std::cout << "calling function with implicit: " << full_function->implicit->type << "."
                          << full_function->implicit->method << " : " << full_function->implicit->ft->to_string()
                          << std::endl;
                ConstFunction* implicit_arg = nullptr;
                if (full_function->implicit->is_static) {
                    auto it2 = e.value->clazz->static_methods.find(full_function->implicit->method);
                    if (it2 == e.value->clazz->static_methods.end()) {
                        std::cout << "ERROR class has no implicit STATIC method: " << full_function->implicit->method
                                  << std::endl;
                        exit(1);
                    }
                    implicit_arg = it2->second;
                } else {
                    auto it3 = e.value->clazz->methods.find(full_function->implicit->method);
                    if (it3 == e.value->clazz->methods.end()) {
                        std::cout << "ERROR class has no implicit method: " << full_function->implicit->method
                                  << std::endl;
                        exit(1);
                    }
                    // implicit_arg = e.value->clazz->methods.at(full_function->implicit->method);
                    implicit_arg = it3->second;
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

                sn->arguments.push_back(new sem::Id(implicit_arg->path.as_str()));
                // assert(*(fff->ft) == *cf->ft);
            }
        }
        if (inf->is_error()) {
            std::cout << "ERRORR CANNOT CALL " << std::endl;
        }
        retv.entity = inf->entity;
    } else {*/
    this->process_function_arguments(retv, arg_entities, arguments, n, function_type, fun_info_p.get());
    // if (fun_info_p->exp_snode->type == sem::ExpType::OBJECT_METHOD) {
    //     auto& om = (std::unique_ptr<sem::ObjectMethod>&) fun_info_p->snode;
    //     retv.exp_snode = std::make_unique<sem::ObjectMethodCallExp>(std::move(om->object),
    //                                                             om->class_path,
    //                                                             om->method_name,
    //                                                             std::move(arguments));
    // } else if (fun_info_p->exp_snode->type == sem::ExpType::CONST_FUNCTION) {
    //     auto& om = (std::unique_ptr<sem::ConstFunction>&) fun_info_p->snode;
    //     retv.snode = std::make_unique<sem::ConstFunctionCall>(om->path, std::move(arguments));
    // } else if (fun_info_p->exp_snode->type == sem::ExpType::OBJECT_CONSTRUCTOR) {
    //     auto& om = (std::unique_ptr<sem::ObjectConstructor>&) fun_info_p->snode;
    //     retv.exp_snode = std::make_unique<sem::ObjectConstructorCall>(om->class_path, std::move(arguments));
    // }
    // }
    retv.exp_snode = std::make_unique<sem::CallExp>(std::move(fun_info_p->exp_snode), std::move(arguments));
    auto f = make_return_info(n, is_rvalue, std::move(retv_p), is_def_const, args_are_constant);
    return f;
}

const sem::TypeFunction& get_function_type(const ExpressionInfo& fun_info) {
    if (fun_info.entity.get().e_type == E_TYPE::CONST_FUNCTION) {
        return fun_info.entity.get().get_constfun().const_function.const_function_ft;
    } else {
        // value & kind = function
        return (sem::TypeFunction&) (fun_info.entity.get().get_value()).type;
    }
}

UExpressionInfo
Checker::make_return_info(const ast::CallExp& n, bool is_rvalue, UExpressionInfo retv_p, bool is_def_const,
                          bool args_are_constant) {
    UExpressionInfo retvp = std::move(retv_p);
    auto& retv = *retvp;
    if (retv.entity.get().e_type == E_TYPE::NOTHING) {
        if (is_rvalue) {
            // this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(retv.entity, n));
            throw std::runtime_error("Error expected expression!");
            return exp_error_stub();
        }
    } else if (retv.entity.get().e_type == E_TYPE::VALUE) {
        EntityValue& value = retv.entity.get().get_value();
        if (value.type.kind == sem::Kind::OBJECT) {
            if (value.type.object().id == ".None") {
                retv.set_entity(new EntityNothing());
            } else {
                fill_value(value);
            }
        }
    }
    retv.is_constant = is_def_const && args_are_constant;
    return retvp;
}

bool Checker::check_arguments(ast::CallExp& n, std::vector<sem::UExp>& arguments,
                              std::vector<std::unique_ptr<Entity>>& arg_entities) {
    bool has_error;
    for (auto& arg: n.arguments) {
        UExpressionInfo arg_type_p = this->dispatch_rvalue(arg);
        if (arg_type_p->is_error()) {
            has_error = true;
            continue;
        }

        std::unique_ptr<Entity> x(arg_type_p->entity.get().clone());
        Entity& arg_entity = *x;
        arg_entities.push_back(std::move(x));
        arguments.push_back(std::move(arg_type_p->exp_snode));
        if (arg_entity.e_type == E_TYPE::CLASS || arg_entity.e_type == E_TYPE::PACKAGE ||
            arg_entity.e_type == E_TYPE::MODULE || arg_entity.e_type == E_TYPE::ENUM ||
            arg_entity.e_type == E_TYPE::NOTHING) {
            has_error = true;
            // this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(arg_entity, arg));
            throw std::runtime_error("Error, expected expression!");
            continue;
        }
    }
    return has_error;
}

void Checker::process_function_arguments(ExpressionInfo& retv, std::vector<std::unique_ptr<Entity>>& arg_entities,
                                         std::vector<sem::UExp>& arguments, ast::CallExp& n,
                                         const sem::TypeFunction& function_type, ExpressionInfo* fun_info_p) {
    ast::UTypeNode rtype(function_type.return_type->to_ast());
    retv.set_entity(entity_from_type(*rtype));
    int sni = static_cast<int>(fun_info_p->this_arg != nullptr);
    for (size_t i = 0; i < n.arguments.size(); i++) {
        // const ast::TypeNode& arg_type = *arg_types[i];
        const sem::Type& param_type = *function_type.param_types[i];

        sem::UExp arg_rvalue_snode = this->make_rvalue(*arg_entities[i], std::move(arguments[sni]), param_type);
        if (arg_rvalue_snode == nullptr) {
            this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(param_type,
                                                                           n.arguments[i],
                                                                           *arg_entities[i]));
            sni++;
            continue;
        }
        arguments[sni] = std::move(arg_rvalue_snode);
        sni++;
    }
}