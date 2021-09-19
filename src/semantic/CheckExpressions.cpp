//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckExpressions.h"
#include "../simple_nodes/expressions/include/Ternary.h"
#include "../simple_nodes/expressions/include/CallExp.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/src/TypeFunction.h"
#include "errors/include/errors.h"

Value& Checker::entity_value_from_actual_base_path_no_generic(const Path& p) {
    if (this->entity_values_no_generic.count(p.as_str()) == 0) {
        auto* ot = new sem::TypeObject(p.as_vec().back(), p);
        // auto v = std::make_unique<Value>(ot);
        // this->fill_value(*v);
        auto v = this->make_value(ot);
        auto& vv = *v;
        this->entity_values_no_generic[p.as_str()] = std::move(v);
        return vv;
    }
    return *this->entity_values_no_generic.at(p.as_str());
}

USemanticInfo Checker::visit_id(ast::Id& n) {
    // Logger::info("Checking id node " + n._id);
    Entity& entity = this->scope->get(n._id);
    if (entity.type == E_TYPE::NOT_FOUND) {
        this->error_reporter.error(std::make_unique<ErrorNotDeclared>(n));
        this->scope->set(n._id, EntityError());
        return error_stub();
    }
    // std::string id =
    //         entity.type == E_TYPE::CONST_FUNCTION ? ((EntityConstFunction&) entity).const_function->path.as_str()
    //                                               : n._id;
    sem::UExp sn;
    if (entity.type == E_TYPE::CONST_FUNCTION) {
        sn = std::make_unique<sem::ConstFunction>(((EntityConstFunction&) entity).const_function.path);
    } else {
        std::string id = n._id;
        sn = std::make_unique<sem::Id>(id);
    }
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(entity.clone());
    info.exp_snode = std::move(sn);
    return info_u;
}

USemanticInfo Checker::visit_cast(ast::Cast& n) {
    // USemanticInfo exp_info = this->dispatch_rvalue(*n.exp);
    // ObjectType cast_type(n.as_type, {});
    // const ast::TypeNode& exp_type = *exp_info->entity.type;
    // if (exp_type == T_INT || exp_type == T_FLOAT || exp_type == T_DOUBLE || exp_type == T_BOOL) {
    //     if (cast_type != T_BOOL && cast_type != T_FLOAT && cast_type != T_DOUBLE && cast_type != T_INT) {
    //         this->error_reporter.fail("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
    //     }
    //     if (exp_type == cast_type) {
    //         this->error_reporter.fail("Casting to same type " + cast_type.to_string());
    //     }
    // } else {
    //     this->error_reporter.fail("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
    // }
    // n.exp_type = exp_type.clone();

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    return info_u;
}

// USemanticInfo Checker::visit_boolop(ast::BoolOp& n) {
//     USemanticInfo left_info_p = this->dispatch_rvalue(*n.left);
//     USemanticInfo right_info_p = this->dispatch_rvalue(*n.right);
//     if (left_info_p->is_error() || right_info_p->is_error()) {
//         return error_stub();
//     }
//     Entity& l_entity = left_info_p->entity;
//     Entity& r_entity = right_info_p->entity;
//     if (l_entity.type != E_TYPE::VALUE || r_entity.type != E_TYPE::VALUE) {
//
//         this->error_reporter.error(std::make_unique<ErrorBoolOp>(l_entity, r_entity, n.start));
//         // this->error_reporter.fail("Can't have binop between 2 non objects!");
//     }
//
//     const ast::TypeNode& ltype = *get_entity_type(l_entity);
//     const ast::TypeNode& rtype = *get_entity_type(r_entity);
//     if (ltype != rtype) {
//         this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(ltype, *n.right, r_entity));
//         // this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*left_info_p->entity.type, *n.right, right_info_p->entity);
//         // this->error_reporter.binop(left_info_p->entity, right_info_p->entity, n.start, n.left, n.right);
//         return error_stub();
//     }
//
//     if (ltype == T_NONE) {
//         // this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
//         return error_stub();
//     }
//
//     if (rtype == T_NONE) {
//         // this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
//         return error_stub();
//     }
//
//     SemanticInfo& left_info = *left_info_p;
//     SemanticInfo& right_info = *right_info_p;
//
//
//     if (left_info.is_error() || right_info.is_error()) {
//         return error_stub();
//     }
//
//     USemanticInfo info_u = std::make_unique<SemanticInfo>();
//     SemanticInfo& info = *info_u;
//
//     if (left_info_p->is_constant && right_info_p->is_constant) {
//         info.is_constant = true;
//     }
//     std::string fun = map_boolop_to_method_name(n.op);
//
//     Entity& entity = this->scope->get(ltype.object().id);
//     if (entity.type != E_TYPE::CLASS && entity.type != E_TYPE::ENUM) {
//         this->error_reporter.fail("This should be a CLASS/ENUM, but it's not!");
//     }
//     if (entity.type == E_TYPE::ENUM) {
//         if (fun != "__eq__" && fun != "__ne__") {
//             this->error_reporter.fail("Error: enum type doesnt support this operator");
//         }
//         // auto* ot = new ast::ObjectType("Boolean", {});
//         // ot->actual_base_path = Path("core.core.Boolean");
//         // TypeNode* rettype = ot;
//         //
//         // info.entity = new Value(std::make_unique<Value>(rettype));
//         info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.Boolean"));
//         ConstFunction* opfun = (((EntityEnum&) entity).enumm)->functions[fun];
//         info.snode = make_boolop_snode(opfun, left_info, right_info);
//
//         // IdSNode* function_id = new IdSNode(opfun->path.as_str());
//         // CallSNode* sn = new CallSNode();
//         // sn->function = function_id;
//         // sn->arguments = {left_info.snode, right_info.snode};
//         //
//         // info.snode = sn;
//     } else {
//         Class* cls = ((EntityClass&) entity).clazz;
//         auto operator_fun_it = cls->static_methods.find(fun);
//         if (operator_fun_it == cls->static_methods.end()) {
//             this->error_reporter.error(std::make_unique<ErrorClassNoMethodForOp>(cls->class_name, fun, n));
//             return error_stub();
//         }
//
//         ConstFunction* operator_fun = operator_fun_it->second;
//         TypeNode* rettype = operator_fun->ft->return_type->clone();
//
//         auto v = std::make_unique<Value>(rettype);
//         this->fill_value(*v);
//         info.entity = *new Value(std::move(v));
//         info.snode = make_boolop_snode(operator_fun, left_info, right_info);
//     }
//
//     return info_u;
// }

USemanticInfo Checker::visit_unary(ast::UnaryOp& n) {
    USemanticInfo exp_info = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), *n.exp);
    if (exp_info->is_error()) {
        return error_stub();
    }
    sem::UExp exp_snode = std::move(exp_info->exp_snode);

    Value& entity_parent = (Value&) exp_info->entity.get();
    Class* cls = entity_parent.clazz;

    auto subscript_it = cls->methods.find("__not__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.fail("Error class " + cls->class_name + " does not define the __not__ operator!");
    }

    ConstFunction& subscript_fun = *subscript_it->second;
    std::string sub_fun_path = subscript_fun.path.as_str();
    sem::Type* rtype = subscript_fun.const_function_ft.return_type->clone();

    auto fsn = std::make_unique<sem::Id>(sub_fun_path);
    std::vector<sem::UExp> v;
    v.emplace_back(std::move(exp_snode));
    auto csn = std::make_unique<sem::Call>(std::move(fsn), std::move(v));

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(std::make_unique<Value>(rtype).release());
    info.snode = std::move(csn);
    return info_u;
}

USemanticInfo Checker::visit_binop(ast::BinaryOp& n) {
    USemanticInfo left_info_p = this->dispatch_rvalue(n.left);
    if (left_info_p->is_error()) {
        return error_stub();
    }
    Entity& l_entity = left_info_p->entity.get();
    if (l_entity.type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(l_entity, n.left));
        return error_stub();
    }
    Value& l_entity_v = (Value&) l_entity;
    USemanticInfo right_sinfo = this->expect_rvalue_of_type(l_entity_v.type, n.right);
    if (right_sinfo->is_error()) {
        return error_stub();
    }
    auto& right_snode = right_sinfo->exp_snode;

    std::string fun = binoptype_to_str(n.op);

    // Entity entity(std::make_unique<Value>(left_info_p->entity.type->object().clone()));
    // this->fill_value(entity.value);
    Class* cls = l_entity_v.clazz;
    assert(cls != nullptr);
    auto operator_fun_it = cls->static_methods.find(fun);
    if (operator_fun_it == cls->static_methods.end()) {
        this->error_reporter.error(std::make_unique<ErrorClassNoMethodForOp>(cls->class_name, fun, n));
        return error_stub();
    }
    ConstFunction& operator_fun = *operator_fun_it->second;
    // auto function_id = std::make_unique<sem::Id>(operator_fun.path.as_str());
    std::vector<sem::UExp> vv;
    vv.push_back(std::move(left_info_p->exp_snode));
    vv.push_back(std::move(right_snode));
    auto sn = std::make_unique<sem::CallExp>(std::make_unique<sem::ConstFunction>(operator_fun.path), std::move(vv));
    sem::Type* rettype = operator_fun.const_function_ft.return_type->clone();

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    // auto v = std::make_unique<Value>(rettype);
    // this->fill_value(*v);
    auto v = this->make_value(rettype);
    info.set_entity(v.release());
    info.exp_snode = std::move(sn);
    return info_u;
}

std::unique_ptr<Value> Checker::make_value(sem::Type* type) {
    if (type->kind != sem::Kind::OBJECT) {
        return nullptr;
    }
    if (type->object().id.size() == 1) {
        Entity& e = this->scope->get(type->object().id);
        Class* clazz;
        if (e.type == E_TYPE::NOT_FOUND) {
            clazz = new Class(type->object().id, Path("core.generics" + type->object().id));
            // clazz->class_name = value.type->object().id;
        } else {
            clazz = ((EntityClass&) e).clazz;
        }
        // assert(e.type == E_TYPE::CLASS);
        // value.clazz = clazz;
        // value.metatype = Meta::CLASS;
        return std::make_unique<Value>(type, clazz);
    }
    ModuleMember module_member = this->top_package.get(type->object().data.actual_base_path);
    if (module_member.type == ModuleMemberType::ENUM) {
        auto value = std::make_unique<Value>(type);
        value->enumm = module_member.enumm;
        value->metatype = Meta::ENUM;
        return value;
    }
    Class* cls = module_member.clazz;
    if (!cls->type_params.empty()) {
        std::cout << "Instantiating type " << type->object().to_string() << std::endl;
        ast::UObjectType o(&type->object().to_ast()->object());
        cls = instantiate_generic(*cls, *o);
        std::cout << "Done instantiating" << std::endl;
    }
    // value.metatype = Meta::CLASS;
    // value.clazz = cls;
    return std::make_unique<Value>(type, cls);
}

void Checker::fill_value(Value& value) {
    if (value.type.kind != sem::Kind::OBJECT) {
        return;
    }
    if (value.type.object().id.size() == 1) {
        Entity& e = this->scope->get(value.type.object().id);
        Class* clazz;
        if (e.type == E_TYPE::NOT_FOUND) {
            clazz = new Class(value.type.object().id, Path("core.generics" + value.type.object().id));
            // clazz->class_name = value.type->object().id;
        } else {
            clazz = ((EntityClass&) e).clazz;
        }
        // assert(e.type == E_TYPE::CLASS);
        value.clazz = clazz;
        value.metatype = Meta::CLASS;
        return;
    }
    ModuleMember module_member = this->top_package.get(value.type.object().data.actual_base_path);
    if (module_member.type == ModuleMemberType::ENUM) {
        value.enumm = module_member.enumm;
        value.metatype = Meta::ENUM;
        return;
    }
    Class* cls = module_member.clazz;
    if (!cls->type_params.empty()) {
        std::cout << "Instantiating type " << value.type.object().to_string() << std::endl;
        ast::UObjectType o(&value.type.object().to_ast()->object());
        cls = instantiate_generic(*cls, *o);
        std::cout << "Done instantiating" << std::endl;
    }
    value.metatype = Meta::CLASS;
    value.clazz = cls;
}

USemanticInfo Checker::visit_subscript(ast::Subscript& node) {
    USemanticInfo parent_p = this->dispatch(*node.parent);
    Entity& entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::VALUE || ((Value&) entity_parent).type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return error_stub();
    }
    Value& value = ((Value&) entity_parent);
    Class* cls = value.clazz;
    if (cls == nullptr) {
        // its totally generic, fail
        this->error_reporter.error(std::make_unique<ErrorObjectNoSpecialMethod>(value.type, "__get_item__", node));
        return error_stub();
    }
    assert(cls != nullptr);
    auto subscript_it = cls->methods.find("__get_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(std::make_unique<ErrorObjectNoSpecialMethod>(value.type, "__get_item__", node));
        return error_stub();
    }
    ConstFunction& subscript_fun = *subscript_it->second;
    std::string sub_fun_path = subscript_fun.path.as_str();

    // VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
        return error_stub();
    }
    USemanticInfo child_sinfo = this->expect_rvalue_of_type(*subscript_fun.const_function_ft.param_types[0],
                                                            *node.child[0]);

    if (child_sinfo->is_error()) {
        return error_stub();
    }
    auto& child_snode = child_sinfo->exp_snode;

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;

    sem::Type& rtype = *subscript_fun.const_function_ft.return_type;
    // auto v = std::make_unique<Value>(rtype.clone());
    // this->fill_value(*v);
    auto v = this->make_value(rtype.clone());
    info.set_entity(v.release());

    auto fsn = std::make_unique<sem::Id>(sub_fun_path);
    std::vector<sem::UExp> vv;
    vv.push_back(std::move(parent_p->exp_snode));
    vv.push_back(std::move(child_snode));
    auto csn = std::make_unique<sem::Call>(std::move(fsn), std::move(vv));
    info.snode = std::move(csn);
    return info_u;
}

USemanticInfo Checker::visit_ternary(ast::Ternary& node) {
    USemanticInfo expression_info_p = this->dispatch_rvalue(*node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    Entity& p_entity = expression_info.entity;
    if (p_entity.type != E_TYPE::VALUE || ((Value&) expression_info_p->entity).type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Option",
                                                                                            {new sem::TypeObject("t")}),
                                                                       *node.expression,
                                                                       expression_info_p->entity));
        return error_stub();
    }
    sem::TypeObject& expression_type = ((Value&) p_entity).type.object();

    if (expression_type.id != "Option") {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Option",
                                                                                            {new sem::TypeObject("t")}),
                                                                       *node.expression,
                                                                       expression_info_p->entity));
        return error_stub();
    }
    this->enter_scope("true_case");
    sem::Type& inner_type = *expression_type.type_params[0];
    auto v = std::make_unique<Value>(inner_type.clone());
    this->scope->set("it", *v);
    USemanticInfo true_case_p = this->dispatch_rvalue(*node.true_case);
    SemanticInfo& true_case = *true_case_p;
    this->leave_scope();
    Value& true_value = (Value&) true_case.entity;
    USemanticInfo false_case_sinfo = this->expect_rvalue_of_type(true_value.type, *node.false_case);
    if (false_case_sinfo->is_error()) {
        return error_stub();
    }
    auto false_case_snode = std::move(false_case_sinfo->exp_snode);

    // auto rv = std::make_unique<Value>(true_value.type.clone());
    // this->fill_value(*rv);
    auto rv = this->make_value(true_value.type.clone());
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(rv.release());
    info.exp_snode = std::make_unique<sem::Ternary>(expression_info_p->exp_snode.release(),
                                                    true_case.exp_snode.release(),
                                                    false_case_snode.release());
    return info_u;
}