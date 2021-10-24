//
// Created by chris on 2/5/21.
//

#include "CheckCall.h"
#include <simple_nodes/common/include/TypeObject.h>
#include <ast/expressions/include/CallExp.h>
#include <ast/statements/Call.h>
#include <simple_nodes/common/include/TypeFunction.h>

sem::UCommon ModuleChecker::visit_call(ast::Call& n) {
    auto s = this->analyze_call(n.function, n.arguments, false, n.start, n.end);
    if (s->is_error()) {
        return nullptr;
    }
    if (not s->entity.get().is_nothing()) {
        this->error_reporter.error(std::make_unique<ErrorUnusedReturnValue>(s->entity, n));
        return nullptr;
    }
    auto& w = (sem::CallExp&) *s->exp_snode;
    return w.to_call();
}

UExpressionInfo
ModuleChecker::analyze_call(const ast::ExpNode& function, std::vector<ast::RExpNode>& arguments, bool is_rvalue,
                            TextPosition start, TextPosition end) {
    auto retv_p = std::make_unique<ExpressionInfo>();
    auto& retv = *retv_p;
    UExpressionInfo fun_info_p = this->dispatch_rvalue(function);
    if (fun_info_p->is_error()) {
        return exp_error_stub();
    }

    bool is_def_const = function.ntype == ExpNodeType::DEF_CONST;
    bool args_are_constant = true;
    ExpressionInfo& fun_info = *fun_info_p;
    if ((fun_info.entity.get().e_type != E_TYPE::CONST_FUNCTION && fun_info_p->entity.get().e_type != E_TYPE::VALUE)) {
        // this->error_reporter.error(std::make_unique<ErrorNotAFunction>(n));
        return exp_error_stub();
    }
    const sem::TypeFunction& function_type = get_function_type(fun_info);
    // ok
    ast::UTypeNode rtype(function_type.return_type->to_ast());
    // retv.set_entity(entity_from_type(*rtype));
    if (rtype->kind == Kind::OBJECT and rtype->object().id == ".None") {
        retv.set_entity(std::make_unique<EntityNothing>());
    } else {
        retv.set_entity(this->make_value(rtype->to_sem()));
    }

    if (arguments.size() != function_type.param_types.size()) {
        this->error_reporter.error(std::make_unique<ErrorFunctionCallNumArgs>(&function_type, start));
        std::cout << function_type.to_string() << std::endl;
        if (!function_is_generic(function_type)) {
            return retv_p;
        } else {
            return exp_error_stub();
        }
    }

    std::vector<std::unique_ptr<Entity>> arg_entities;
    std::vector<sem::UExp> arguments_;
    bool has_error = check_arguments(arguments, arguments_, arg_entities);
    if (has_error) {
        return exp_error_stub();
    }
    ast::VectorOfTypes arg_types;
    for (auto& at: arg_entities) {
        sem::Type* p_type = at->get_value().type.clone();
        arg_types.push_back(p_type->to_ast());
    }
    std::map<std::string, ast::Type*> a;
    ast::FunctionType* ft = static_cast<ast::FunctionType*>(function_type.to_ast());
    if (ft->is_generic()) {
        retv_p = this->match_arguments_to_generic_function(*ft,
                                                           arg_types,
                                                           a,
                                                           fun_info.entity.get().get_constfun().const_function.constraints);
        retv_p->exp_snode = std::make_unique<sem::CallExp>(std::move(fun_info_p->exp_snode), std::move(arguments_));
        return retv_p;
    }
    this->process_function_arguments(arg_entities, arguments_, arguments, function_type, fun_info_p.get());
    retv.exp_snode = std::make_unique<sem::CallExp>(std::move(fun_info_p->exp_snode), std::move(arguments_));
    auto f = make_return_info(is_rvalue, std::move(retv_p), is_def_const, args_are_constant);
    return f;
}

UExpressionInfo ModuleChecker::visit_callexp(ast::CallExp& n, bool is_rvalue) {
    return this->analyze_call(n.function, n.arguments, is_rvalue, n.start, n.end);
}

const sem::TypeFunction& get_function_type(const ExpressionInfo& fun_info) {
    if (fun_info.entity.get().is_constfun()) {
        return fun_info.entity.get().get_constfun().const_function.const_function_ft;
    } else {
        // value & kind = function
        return (sem::TypeFunction&) (fun_info.entity.get().get_value()).type;
    }
}

UExpressionInfo
ModuleChecker::make_return_info(bool is_rvalue, UExpressionInfo retv_p, bool is_def_const, bool args_are_constant) {
    UExpressionInfo retvp = std::move(retv_p);
    auto& retv = *retvp;
    if (retv.entity.get().is_value()) {
        EntityValue& value = retv.entity.get().get_value();
        if (value.type.kind == sem::Kind::OBJECT) {
            if (value.type.object().id == ".None") {
                retv.set_entity(std::make_unique<EntityNothing>());
            }
        }
    }
    retv.is_constant = is_def_const && args_are_constant;
    return retvp;
}

bool ModuleChecker::check_arguments(std::vector<ast::RExpNode>& narguments, std::vector<sem::UExp>& arguments,
                                    std::vector<std::unique_ptr<Entity>>& arg_entities) {
    bool has_error;
    for (auto& arg: narguments) {
        UExpressionInfo arg_type_p = this->dispatch_rvalue(arg);
        if (arg_type_p->is_error()) {
            has_error = true;
            continue;
        }

        std::unique_ptr<Entity> x(arg_type_p->entity.get().clone());
        Entity& arg_entity = *x;
        arg_entities.push_back(std::move(x));
        arguments.push_back(std::move(arg_type_p->exp_snode));
        if (arg_entity.is_class() || arg_entity.is_package() || arg_entity.is_module() || arg_entity.is_enum() ||
            arg_entity.is_nothing()) {
            has_error = true;
            // this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(arg_entity, arg));
            throw std::runtime_error("Error, expected expression!");
            continue;
        }
    }
    return has_error;
}

void ModuleChecker::process_function_arguments(std::vector<std::unique_ptr<Entity>>& arg_entities,
                                               std::vector<sem::UExp>& arguments,
                                               std::vector<ast::RExpNode>& narguments,
                                               const sem::TypeFunction& function_type, ExpressionInfo* fun_info_p) {
    int sni = static_cast<int>(fun_info_p->this_arg != nullptr);
    for (size_t i = 0; i < narguments.size(); i++) {
        // const ast::TypeNode& arg_type = *arg_types[i];
        const sem::Type& param_type = *function_type.param_types[i];

        sem::UExp arg_rvalue_snode = this->make_rvalue(*arg_entities[i], std::move(arguments[sni]), param_type);
        if (arg_rvalue_snode == nullptr) {
            this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(param_type,
                                                                           narguments[i],
                                                                           *arg_entities[i]));
            sni++;
            continue;
        }
        arguments[sni] = std::move(arg_rvalue_snode);
        sni++;
    }
}