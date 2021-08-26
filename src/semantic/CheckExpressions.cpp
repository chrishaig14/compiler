//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckExpressions.h"
#include "../simple_nodes/TernarySNode.h"
#include "errors/ErrorNotDeclared.h"
#include "errors/ErrorTypeMismatch.h"
#include "errors/ErrorExpectedExpression.h"
#include "errors/ErrorBoolOp.h"
#include "errors/ErrorObjectNoSpecialMethod.h"
#include "errors/ErrorClassNoMethodForOp.h"

EntityValue& Checker::entity_value_from_actual_base_path_no_generic(const Path& p) {
    if (this->entity_values_no_generic.count(p.as_str()) == 0) {
        auto* ot = new ObjectType(p.as_vec().back());
        ot->actual_base_path = p;
        auto v = std::make_unique<Value>(ot);
        this->fill_value(*v);
        auto entity = std::make_unique<EntityValue>(std::move(v));
        auto& e = *entity;
        this->entity_values_no_generic[p.as_str()] = std::move(entity);
        return e;
    }
    return *this->entity_values_no_generic.at(p.as_str());
}

USemanticInfo Checker::visit_id(IdNode& n) {
    // Logger::info("Checking id node " + n._id);
    Entity& entity = this->scope->get(n._id);
    if (entity.type == E_TYPE::NOT_FOUND) {
        this->error_reporter.error(ErrorNotDeclared(n));
        this->scope->set(n._id, new EntityError());
        return error_stub();
    }
    std::string id =
            entity.type == E_TYPE::CONST_FUNCTION ? ((EntityConstFunction&) entity).const_function->path.as_str()
                                                  : n._id;
    auto* sn = new IdSNode(id);
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.entity = entity;
    info.snode = sn;
    return info_u;
}

USemanticInfo Checker::visit_cast(CastNode& n) {
    // USemanticInfo exp_info = this->dispatch_rvalue(*n.exp);
    // ObjectType cast_type(n.as_type, {});
    // const TypeNode& exp_type = *exp_info->entity.value->type;
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

USemanticInfo Checker::visit_boolop(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch_rvalue(*n.left);
    USemanticInfo right_info_p = this->dispatch_rvalue(*n.right);
    if (left_info_p->is_error() || right_info_p->is_error()) {
        return error_stub();
    }
    Entity& l_entity = left_info_p->entity;
    Entity& r_entity = right_info_p->entity;
    if (l_entity.type != E_TYPE::VALUE || r_entity.type != E_TYPE::VALUE) {

        this->error_reporter.error(ErrorBoolOp(l_entity, r_entity, n.start));
        // this->error_reporter.fail("Can't have binop between 2 non objects!");
    }

    const TypeNode& ltype = *get_entity_type(l_entity);
    const TypeNode& rtype = *get_entity_type(r_entity);
    if (ltype != rtype) {
        this->error_reporter.error(ErrorTypeMismatch(ltype, *n.right, r_entity));
        // this->error_reporter.error(ErrorTypeMismatch(*left_info_p->entity.value->type, *n.right, right_info_p->entity);
        // this->error_reporter.binop(left_info_p->entity, right_info_p->entity, n.start, n.left, n.right);
        return error_stub();
    }

    if (ltype == T_NONE) {
        // this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        return error_stub();
    }

    if (rtype == T_NONE) {
        // this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;


    if (left_info.is_error() || right_info.is_error()) {
        return error_stub();
    }

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    std::string fun = map_boolop_to_method_name(n.op);

    Entity& entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS && entity.type != E_TYPE::ENUM) {
        this->error_reporter.fail("This should be a CLASS/ENUM, but it's not!");
    }
    if (entity.type == E_TYPE::ENUM) {
        if (fun != "__eq__" && fun != "__ne__") {
            this->error_reporter.fail("Error: enum type doesnt support this operator");
        }
        // auto* ot = new ObjectType("Boolean", {});
        // ot->actual_base_path = Path("core.core.Boolean");
        // TypeNode* rettype = ot;
        //
        // info.entity = new EntityValue(std::make_unique<Value>(rettype));
        info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.Boolean"));
        ConstFunction* opfun = (((EntityEnum&) entity).enumm)->functions[fun];
        info.snode = make_boolop_snode(opfun, left_info, right_info);

        // IdSNode* function_id = new IdSNode(opfun->path.as_str());
        // CallSNode* sn = new CallSNode();
        // sn->function = function_id;
        // sn->arguments = {left_info.snode, right_info.snode};
        //
        // info.snode = sn;
    } else {
        Class* cls = ((EntityClass&) entity).clazz;
        auto operator_fun_it = cls->static_methods.find(fun);
        if (operator_fun_it == cls->static_methods.end()) {
            this->error_reporter.error(ErrorClassNoMethodForOp(cls->class_name, fun, n));
            return error_stub();
        }

        ConstFunction* operator_fun = operator_fun_it->second;
        TypeNode* rettype = operator_fun->ft->return_type->clone();

        auto v = std::make_unique<Value>(rettype);
        this->fill_value(*v);
        info.entity = *new EntityValue(std::move(v));
        info.snode = make_boolop_snode(operator_fun, left_info, right_info);
    }

    return info_u;
}

USemanticInfo Checker::visit_unary(UnaryOpNode& n) {
    USemanticInfo exp_info = this->expect_rvalue_of_type(T_BOOL, *n.exp);
    if (exp_info->is_error()) {
        return error_stub();
    }
    SNode* exp_snode = exp_info->snode;

    EntityValue& entity_parent = (EntityValue&) exp_info->entity.get();
    Class* cls = entity_parent.value->clazz;

    auto subscript_it = cls->methods.find("__not__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.fail("Error class " + cls->class_name + " does not define the __not__ operator!");
    }

    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    auto* fsn = new IdSNode(sub_fun_path);
    auto* csn = new CallSNode(fsn, {exp_snode});

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.entity = *new EntityValue(std::make_unique<Value>(rtype));
    info.snode = csn;
    return info_u;
}

USemanticInfo Checker::visit_binop(ast::BinopNode& n) {
    USemanticInfo left_info_p = this->dispatch_rvalue(*n.left);
    if (left_info_p->is_error()) {
        return error_stub();
    }
    Entity& l_entity = left_info_p->entity.get();
    if (l_entity.type != E_TYPE::VALUE) {
        this->error_reporter.error(ErrorExpectedExpression(l_entity, *n.left));
        return error_stub();
    }
    EntityValue& l_entity_v = (EntityValue&) l_entity;
    USemanticInfo right_sinfo = this->expect_rvalue_of_type(*l_entity_v.value->type, *n.right);
    if (right_sinfo->is_error()) {
        return error_stub();
    }
    SNode* right_snode = right_sinfo->snode;

    std::string fun = binoptype_to_str(n.op);

    // Entity entity(std::make_unique<Value>(left_info_p->entity.value->type->object().clone()));
    // this->fill_value(entity.value);
    Class* cls = l_entity_v.value->clazz;
    assert(cls != nullptr);
    auto operator_fun_it = cls->static_methods.find(fun);
    if (operator_fun_it == cls->static_methods.end()) {
        this->error_reporter.error(ErrorClassNoMethodForOp(cls->class_name, fun, n));
        return error_stub();
    }
    ConstFunction* operator_fun = operator_fun_it->second;
    auto* function_id = new IdSNode(operator_fun->path.as_str());
    auto* sn = new CallSNode(function_id, {left_info_p->snode, right_snode});
    TypeNode* rettype = operator_fun->ft->return_type->clone();

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    auto v = std::make_unique<Value>(rettype);
    this->fill_value(*v);
    info.entity = *new EntityValue(std::move(v));
    info.snode = sn;
    return info_u;
}

void Checker::fill_value(Value& value) {
    if (value.type->kind != Kind::OBJECT) {
        return;
    }
    if (value.type->object().id.size() == 1) {
        Entity& e = this->scope->get(value.type->object().id);
        Class* clazz;
        if (e.type == E_TYPE::NOT_FOUND) {
            clazz = new Class();
            clazz->class_name = value.type->object().id;
        } else {
            clazz = ((EntityClass&) e).clazz;
        }
        // assert(e.type == E_TYPE::CLASS);
        value.clazz = clazz;
        value.metatype = Meta::CLASS;
        return;
    }
    Flirpin flirpin = this->top_package.get(value.type->object().actual_base_path);
    if (flirpin.type == F_TYPE::ENUM) {
        value.enumm = flirpin.enumm;
        value.metatype = Meta::ENUM;
        return;
    }
    Class* cls = flirpin.clazz;
    if (!cls->type_params.empty()) {
        std::cout << "Instantiating type " << value.type->object().to_string() << std::endl;
        cls = instantiate_generic(cls, value.type->object());
        std::cout << "Done instantiating" << std::endl;
    }
    value.metatype = Meta::CLASS;
    value.clazz = cls;
}

USemanticInfo Checker::visit_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(*node.parent);
    Entity& entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::VALUE || ((EntityValue&) entity_parent).value->type->kind == Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return error_stub();
    }
    Value& value = *((EntityValue&) entity_parent).value;
    Class* cls = value.clazz;
    if (cls == nullptr) {
        // its totally generic, fail
        this->error_reporter.error(ErrorObjectNoSpecialMethod(*value.type, "__get_item__", node));
        return error_stub();
    }
    assert(cls != nullptr);
    auto subscript_it = cls->methods.find("__get_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(ErrorObjectNoSpecialMethod(*value.type, "__get_item__", node));
        return error_stub();
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    // VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
        return error_stub();
    }
    USemanticInfo child_sinfo = this->expect_rvalue_of_type(*subscript_fun->ft->param_types[0], *node.child[0]);
    if (child_sinfo->is_error()) {
        return error_stub();
    }
    SNode* child_snode = child_sinfo->snode;

    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;

    auto v = std::make_unique<Value>(rtype);
    this->fill_value(*v);
    info.entity = *new EntityValue(std::move(v));

    auto* fsn = new IdSNode(sub_fun_path);
    auto* csn = new CallSNode(fsn, {parent_p->snode, child_snode});
    info.snode = csn;
    return info_u;
}

USemanticInfo Checker::visit_ternary(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch_rvalue(*node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    Entity& p_entity = expression_info.entity;
    if (p_entity.type != E_TYPE::VALUE ||
        ((EntityValue&) expression_info_p->entity).value->type->kind == Kind::FUNCTION) {
        this->error_reporter.error(ErrorTypeMismatch(*new ObjectType("Option", {new ObjectType("t")}),
                                                     *node.expression,
                                                     expression_info_p->entity));
        return error_stub();
    }
    ObjectType& expression_type = ((EntityValue&) p_entity).value->type->object();

    if (expression_type.id != "Option") {
        this->error_reporter.error(ErrorTypeMismatch(*new ObjectType("Option", {new ObjectType("t")}),
                                                     *node.expression,
                                                     expression_info_p->entity));
        return error_stub();
    }
    this->enter_scope("true_case");
    TypeNode*& inner_type = expression_type.type_params[0];
    auto v = std::make_unique<Value>(inner_type);
    this->scope->set("it", new EntityValue(std::move(v)));
    USemanticInfo true_case_p = this->dispatch_rvalue(*node.true_case);
    SemanticInfo& true_case = *true_case_p;
    this->leave_scope();
    EntityValue& true_value = (EntityValue&) true_case.entity;
    USemanticInfo false_case_sinfo = this->expect_rvalue_of_type(*true_value.value->type, *node.false_case);
    if (false_case_sinfo->is_error()) {
        return error_stub();
    }
    SNode* false_case_snode = false_case_sinfo->snode;

    auto rv = std::make_unique<Value>(true_value.value->type->clone());
    this->fill_value(*rv);
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.entity = *new EntityValue(std::move(rv));
    info.snode = new TernarySNode(expression_info_p->snode, true_case.snode, false_case_snode);
    return info_u;
}