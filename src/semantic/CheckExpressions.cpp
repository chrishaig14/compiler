//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckExpressions.h"
#include "../simple_nodes/TernarySNode.h"
#include "../nodes/UnaryOpNode.h"

USemanticInfo Checker::visit_id(IdNode& n) {
    // Logger::info("Checking id node " + n._id);
    SemanticInfo info;
    IdSNode* sn = new IdSNode();
    info.snode = sn;
    Entity entity = this->scope->get(n._id);
    if (entity.type == E_TYPE::NOT_FOUND) {
        this->error_reporter.variable_not_declared(n._id, n.start);
        this->scope->set(n._id, Entity(E_TYPE::ERROR));
        return error_stub();
    }
    if (entity.type == E_TYPE::CONST_FUNCTION) {
        sn->identifier = entity.const_function->path.as_str();
    } else {
        sn->identifier = n._id;
    }
    info.entity = entity;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_cast(CastNode& n) {
    USemanticInfo exp_info = this->dispatch_rvalue(n.exp);
    SemanticInfo info;
    ObjectType cast_type(n.as_type, {});
    const TypeNode& exp_type = *exp_info->entity.value->type;
    if (exp_type == T_INT || exp_type == T_FLOAT || exp_type == T_DOUBLE || exp_type == T_BOOL) {
        if (cast_type != T_BOOL && cast_type != T_FLOAT && cast_type != T_DOUBLE && cast_type != T_INT) {
            this->error_reporter.fail("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
        }
        if (exp_type == cast_type) {
            this->error_reporter.fail("Casting to same type " + cast_type.to_string());
        }
    } else {
        this->error_reporter.fail("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
    }
    n.exp_type = exp_type.clone();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_boolop(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch_rvalue(n.left);
    USemanticInfo right_info_p = this->dispatch_rvalue(n.right);
    SemanticInfo info;
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::VALUE || right_info_p->entity.type != E_TYPE::VALUE) {
        this->error_reporter.bool_op(left_info_p->entity, right_info_p->entity, n.start);
        // this->error_reporter.fail("Can't have binop between 2 non objects!");
    }

    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        this->error_reporter.binop(left_info_p->entity, right_info_p->entity, n.start);
        return error_stub();
    }

    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        return error_stub();
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;


    if (left_info.entity.type == E_TYPE::ERROR || right_info.entity.type == E_TYPE::ERROR) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    std::string fun = map_boolop_to_method_name(n.op);

    Entity entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS && entity.type != E_TYPE::ENUM) {
        this->error_reporter.fail("This should be a CLASS/ENUM, but it's not!");
    }
    if (entity.type == E_TYPE::ENUM) {
        if (fun != "__eq__" && fun != "__ne__") {
            this->error_reporter.fail("Error: enum type doesnt support this operator");
        }
        ObjectType* ot = new ObjectType("Boolean", {});
        ot->actual_base_path = Path("core.Boolean");
        TypeNode* rettype = ot;
        info.entity = Entity(new Value(rettype));

        ConstFunction* opfun = entity.enumm->functions[fun];

        info.snode = make_boolop_snode(opfun, left_info, right_info);

        // IdSNode* function_id = new IdSNode(opfun->path.as_str());
        // CallSNode* sn = new CallSNode();
        // sn->function = function_id;
        // sn->arguments = {left_info.snode, right_info.snode};
        //
        // info.snode = sn;
    } else {
        Class* cls = entity.clazz;
        auto operator_fun_it = cls->static_methods.find(fun);
        if (operator_fun_it == cls->static_methods.end()) {
            this->error_reporter.class_no_method(cls->class_name, fun, n.start);
            return error_stub();
        }

        ConstFunction* operator_fun = operator_fun_it->second;
        TypeNode* rettype = operator_fun->ft->return_type->clone();
        info.entity = Entity(new Value(rettype));

        info.snode = make_boolop_snode(operator_fun, left_info, right_info);
    }

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_unary(UnaryOpNode& n) {
    SemanticInfo info;
    USemanticInfo exp_info = this->dispatch_rvalue(n.exp);
    if (exp_info->entity.type != E_TYPE::VALUE) {
        this->error_reporter.fail("ERROR EXPECTED A BOOLEAN");
    }
    if (*exp_info->entity.value->type != T_BOOL) {
        this->error_reporter.fail("ERROR EXPECTED A BOOLEAN");
    }
    Value* v = new Value(new ObjectType("Boolean"));
    info.entity = Entity(v);

    USemanticInfo parent_p = this->dispatch_rvalue(n.exp);
    Entity entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::VALUE) {
        this->error_reporter.fail("Error unary of something that is not an object!");
    }
    Entity class_entity = this->scope->get(entity_parent.value->type->object().id);
    if (class_entity.type != E_TYPE::CLASS) {
        this->error_reporter.fail("Error this should be a CLASS, but it's not!");
    }
    Class* cls = class_entity.clazz;

    if (cls->type_params.size() != 0) {
        cls = instantiate_generic(cls, entity_parent.value->type->object());
    }

    auto subscript_it = cls->methods.find("__not__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.fail("Error class " + cls->class_name + " does not define the __not__ operator!");
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    info.entity = Entity(new Value(rtype));
    CallSNode* csn = new CallSNode();
    IdSNode* fsn = new IdSNode();
    fsn->identifier = sub_fun_path;
    csn->function = fsn;
    csn->arguments.push_back(parent_p->snode);
    info.snode = csn;

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_binop(BinopNode& n) {

    CallSNode* sn = new CallSNode();
    IdSNode* function_id = new IdSNode();
    sn->function = function_id;
    function_id->identifier = "";//mangle_path(this->module->imported_paths.at("Integer.add"));
    SemanticInfo info;
    info.snode = sn;

    // Logger::info("Checking binop node");
    USemanticInfo left_info_p = this->dispatch_rvalue(n.left);
    USemanticInfo right_info_p = this->dispatch_rvalue(n.right);
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::VALUE || right_info_p->entity.type != E_TYPE::VALUE) {
        this->error_reporter.binop(left_info_p->entity, right_info_p->entity, n.start);
        return error_stub();
    }
    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        this->error_reporter.binop(left_info_p->entity, right_info_p->entity, n.start);
        return error_stub();
        // this->error_reporter.fail("Binary operation between values of different types: " + ltype.to_string() + " and " +
        //                          rtype.to_string());
    }
    bool err = false;
    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        err = true;
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        err = true;
    }
    if (err) {
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    sn->arguments.push_back(left_info.snode);
    sn->arguments.push_back(right_info.snode);

    if (left_info.entity.type == E_TYPE::ERROR || right_info.entity.type == E_TYPE::ERROR) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    TypeNode* rettype;
    std::string fun = binoptype_to_str(n.op);

    // ;this->scope->get(ltype.object().id);
    Entity entity(new Value(ltype.object().clone()));
    this->fill_value(entity.value);
    Class* cls = entity.value->clazz;
    assert(cls != nullptr);
    auto operator_fun_it = cls->static_methods.find(fun);
    if (operator_fun_it == cls->static_methods.end()) {
        this->error_reporter.class_no_method(cls->class_name, fun, n.start);
        return error_stub();
    }
    ConstFunction* operator_fun = operator_fun_it->second;
    function_id->identifier = operator_fun->path.as_str();
    rettype = operator_fun->ft->return_type->clone();
    // n.ltype = left.clone();
    info.entity = Entity(new Value(rettype));
    this->fill_value(info.entity.value);
    return std::make_unique<SemanticInfo>(info);
}

void Checker::fill_value(Value* value) {
    if (value->type->kind != Kind::OBJECT) {
        return;
    }
    Flirpin flirpin = this->root_package->get(value->type->object().actual_base_path);
    if (flirpin.type == F_TYPE::ENUM) {
        value->enumm = flirpin.enumm;
        value->metatype = Meta::ENUM;
        return;
    }
    Class* cls = flirpin.clazz;
    if (cls->type_params.size() != 0) {
        cls = instantiate_generic(cls, value->type->object());
    }
    value->metatype = Meta::CLASS;
    value->clazz = cls;
}

USemanticInfo Checker::visit_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    Entity entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::VALUE || entity_parent.value->type->kind == Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
    }

    Class* cls = entity_parent.value->clazz;
    if (cls == nullptr) {
        // its totally generic, fail
        this->error_reporter.fail(
                "Error, accessing subscript of totally generic type: " + entity_parent.value->type->to_string(),
                node.start);
        return error_stub();
    }
    assert(cls != nullptr);
    auto subscript_it = cls->methods.find("__get_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.fail("Error class " + cls->class_name + " does not define the __get_item__ operator!");
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();


    VectorOfTypes children;
    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
    }
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch_rvalue(c);
    Entity child_entity = ct->entity;
    if (child_entity.type != E_TYPE::VALUE) {
        this->error_reporter.fail("Error using something that's not an object as a subscript!");
    }
    if (*child_entity.value->type != *subscript_fun->ft->param_types[0]) {
        this->error_reporter.fail(
                "Error subscript type is " + child_entity.value->type->to_string() + " but should be " +
                subscript_fun->ft->param_types[0]->to_string());
    }
    SemanticInfo info;

    info.entity = Entity(new Value(rtype));
    this->fill_value(info.entity.value);

    CallSNode* csn = new CallSNode();
    IdSNode* fsn = new IdSNode();
    fsn->identifier = sub_fun_path;
    csn->function = fsn;
    csn->arguments.push_back(parent_p->snode);
    csn->arguments.push_back(ct->snode);
    info.snode = csn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_ternary(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch_rvalue(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.entity.type != E_TYPE::VALUE || expression_info_p->entity.value->type->kind == Kind::FUNCTION) {
        this->error_reporter.fail("Unexpected non-object");
    }
    ObjectType& expression_type = expression_info.entity.value->type->object();

    if (expression_type.id != "Option") {
        this->error_reporter.fail("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    // TypeNode& type = *expression_type.type_params[0];
    // semanticInfo.set_type(type);
    this->enter_scope("true_case");
    TypeNode*& inner_type = expression_type.type_params[0];
    Value* v = new Value(inner_type);
    this->scope->set("it", Entity(v));
    USemanticInfo true_case_p = this->dispatch_rvalue(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch_rvalue(node.false_case);
    SemanticInfo& false_case = *false_case_p;
    if (*false_case.entity.value->type != *true_case.entity.value->type) {
        this->error_reporter.fail(
                "True case and false case type don't match: " + true_case.entity.value->type->to_string() + " != " +
                false_case.entity.value->type->to_string());
    }
    Value* rv = new Value(true_case.entity.value->type->clone());
    semanticInfo.entity = Entity(rv);
    semanticInfo.snode = new TernarySNode(expression_info_p->snode, true_case.snode, false_case.snode);
    return std::make_unique<SemanticInfo>(semanticInfo);
}