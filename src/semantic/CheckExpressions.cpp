//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckExpressions.h"
#include "../simple_nodes/expressions/include/Ternary.h"
#include "../simple_nodes/expressions/include/CallExp.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/include/TypeFunction.h"

EntityValue& ModuleChecker::entity_value_from_actual_base_path_no_generic(const Path& p) {
    if (this->entity_values_no_generic.count(p.as_str()) == 0) {
        auto* ot = new sem::TypeObject(p.basname(), p);
        auto v = this->make_value(ot);
        auto& vv = *v;
        this->entity_values_no_generic[p.as_str()] = std::move(v);
        return vv;
    }
    return *this->entity_values_no_generic.at(p.as_str());
}

UExpressionInfo ModuleChecker::visit_id(ast::Id& n) {
    Entity& entity = this->scope->get(n._id);
    if (entity.is_notfound()) {
        this->error_reporter.error(std::make_unique<ErrorNotDeclared>(n));
        this->scope->set(n._id, EntityError());
        return exp_error_stub();
    }
    sem::UExp sn;
    if (entity.is_constfun()) {
        sn = std::make_unique<sem::ConstFunction>(entity.get_constfun().const_function.path);
    } else {
        std::string id = n._id;
        sn = std::make_unique<sem::Id>(id);
    }
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(entity.clone());
    info.exp_snode = std::move(sn);
    return info_u;
}

sem::UCommon ModuleChecker::visit_cast(ast::Cast& n) {
    return nullptr;
}

UExpressionInfo ModuleChecker::visit_unary(ast::UnaryOp& n) {
    UExpressionInfo exp_info = this->expect_rvalue_of_type(sem::TypeObject("Boolean"), *n.exp);
    if (exp_info->is_error()) {
        return exp_error_stub();
    }
    sem::UExp exp_snode = std::move(exp_info->exp_snode);

    EntityValue& entity_parent = exp_info->entity.get().get_value();
    ConcreteClass* cls = entity_parent.clazz;

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
    auto csn = std::make_unique<sem::CallExp>(std::move(fsn), std::move(v));

    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(this->make_value(rtype));
    info.exp_snode = std::move(csn);
    return info_u;
}

UExpressionInfo ModuleChecker::visit_binop(ast::BinaryOp& n) {
    UExpressionInfo left_info_p = this->dispatch_rvalue(n.left);
    if (left_info_p->is_error()) {
        return exp_error_stub();
    }
    Entity& l_entity = left_info_p->entity.get();
    if (l_entity.e_type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(l_entity, n.left));
        return exp_error_stub();
    }
    EntityValue& l_entity_v = l_entity.get_value();
    UExpressionInfo right_sinfo = this->expect_rvalue_of_type(l_entity_v.type, n.right);
    if (right_sinfo->is_error()) {
        return exp_error_stub();
    }
    auto& right_snode = right_sinfo->exp_snode;

    std::string fun = binoptype_to_str(n.op);
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    if (l_entity_v.metatype == Meta::ENUM) {
        std::vector<sem::UExp> v;
        v.push_back(std::move(left_info_p->exp_snode));
        v.push_back(std::move(right_sinfo->exp_snode));
        info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("libcore.libcore.Boolean")).clone());
        info.exp_snode = std::make_unique<sem::CallExp>(std::make_unique<sem::StaticMethod>(l_entity_v.enumm->path,
                                                                                            "__eq__"), std::move(v));
    } else {
        ConcreteClass* cls = l_entity_v.clazz;
        assert(cls != nullptr);
        auto operator_fun_it = cls->static_methods.find(fun);
        if (operator_fun_it == cls->static_methods.end()) {
            this->error_reporter.error(std::make_unique<ErrorClassNoMethodForOp>(cls->class_name, fun, n));
            return exp_error_stub();
        }
        ConstFunction& operator_fun = *operator_fun_it->second;
        std::vector<sem::UExp> vv;
        vv.push_back(std::move(left_info_p->exp_snode));
        vv.push_back(std::move(right_snode));
        auto sn = std::make_unique<sem::CallExp>(std::make_unique<sem::ConstFunction>(operator_fun.path),
                                                 std::move(vv));
        sem::Type* rettype = operator_fun.const_function_ft.return_type->clone();
        info.set_entity(this->make_value(rettype));
        info.exp_snode = std::move(sn);
    }
    return info_u;
}

std::unique_ptr<EntityValue> ModuleChecker::make_value(sem::Type* type) {
    if (type->kind != sem::Kind::OBJECT) {
        return nullptr;
    }
    sem::TypeObject& type_object = type->object();
    if (type_object.id.size() == 1) {
        std::cout << "found generic type " << type_object.id << std::endl;
        Entity& e = this->scope->get(type_object.id);
        ConcreteClass* clazz;
        if (e.is_notfound()) {
            clazz = new ConcreteClass(type_object.id, Path("core.generics" + type_object.id));
            if (not type_object.typeclass.empty()) {
                Entity& tc = this->scope->get(type_object.typeclass);
                if (tc.is_notfound()) {
                    throw std::runtime_error("did not find typeclass " + type_object.typeclass);
                } else {

                }
            }
        } else {
            clazz = &e.get_class().clazz;
        }
        return std::make_unique<EntityValue>(type, clazz);
    }
    ModuleMember* module_member_p = this->top_package.get(type_object.data.actual_base_path);
    if (module_member_p == nullptr) {
        throw std::runtime_error("module_member should not be nullptr");
    }
    ModuleMember& module_member = *module_member_p;
    if (module_member.is_enumm()) {
        return std::make_unique<EntityValue>(type, &module_member.enumm());
    }
    ConcreteClass* cls = &module_member.klass();
    if (not type_object.type_params.empty()) {
        auto instance = cls->generic_instances.find(type->actual_to_string());
        if (instance == cls->generic_instances.end()) {
            ast::UObjectType o(&type_object.to_ast()->object());
            ConcreteClass* bcls = cls;
            cls = instantiate_generic(*bcls, *o);
            bcls->generic_instances[type->actual_to_string()] = std::unique_ptr<ConcreteClass>(cls);
        } else {
            cls = instance->second.get();
        }
    }
    return std::make_unique<EntityValue>(type, cls);
}

UExpressionInfo ModuleChecker::visit_subscript(ast::Subscript& node) {
    UExpressionInfo parent_p = this->dispatch_rvalue(*node.parent);
    Entity& entity_parent = parent_p->entity;
    if (entity_parent.e_type != E_TYPE::VALUE || entity_parent.get_value().type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.fail("Error subscript of something that is not an object!");
        return exp_error_stub();
    }
    EntityValue& value = entity_parent.get_value();
    ConcreteClass* cls = value.clazz;
    if (cls == nullptr) {
        // its totally generic, fail
        this->error_reporter.error(std::make_unique<ErrorObjectNoSpecialMethod>(value.type, "__get_item__", node));
        return exp_error_stub();
    }
    assert(cls != nullptr);
    auto subscript_it = cls->methods.find("__get_item__");
    if (subscript_it == cls->methods.end()) {
        this->error_reporter.error(std::make_unique<ErrorObjectNoSpecialMethod>(value.type, "__get_item__", node));
        return exp_error_stub();
    }
    ConstFunction& subscript_fun = *subscript_it->second;
    std::string sub_fun_path = subscript_fun.path.as_str();

    if (node.child.size() > 1) {
        this->error_reporter.fail("Error subscript with more than one child!");
        return exp_error_stub();
    }
    UExpressionInfo child_sinfo = this->expect_rvalue_of_type(*subscript_fun.const_function_ft.param_types[0],
                                                              *node.child[0]);

    if (child_sinfo->is_error()) {
        return exp_error_stub();
    }
    auto& child_snode = child_sinfo->exp_snode;

    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;

    sem::Type& rtype = *subscript_fun.const_function_ft.return_type;
    info.set_entity(this->make_value(rtype.clone()));

    auto fsn = std::make_unique<sem::ObjectMethod>(std::move(parent_p->exp_snode), cls->path, "__get_item__");
    std::vector<sem::UExp> vv;
    vv.push_back(std::move(child_snode));
    auto csn = std::make_unique<sem::CallExp>(std::move(fsn), std::move(vv));
    info.exp_snode = std::move(csn);
    return info_u;
}

UExpressionInfo ModuleChecker::visit_ternary(ast::Ternary& node) {
    UExpressionInfo expression_info_p = this->dispatch_rvalue(*node.expression);
    ExpressionInfo& expression_info = *expression_info_p;
    Entity& p_entity = expression_info.entity;
    if (p_entity.e_type != E_TYPE::VALUE ||
        expression_info_p->entity.get().get_value().type.kind == sem::Kind::FUNCTION) {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Option",
                                                                                            {new sem::TypeObject("t")}),
                                                                       *node.expression,
                                                                       expression_info_p->entity));
        return exp_error_stub();
    }
    sem::TypeObject& expression_type = p_entity.get_value().type.object();

    if (expression_type.id != "Option") {
        this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*new sem::TypeObject("Option",
                                                                                            {new sem::TypeObject("t")}),
                                                                       *node.expression,
                                                                       expression_info_p->entity));
        return exp_error_stub();
    }
    this->enter_scope("true_case");
    sem::Type& inner_type = *expression_type.type_params[0];
    auto v = std::make_unique<EntityValue>(inner_type.clone(), (ConcreteClass*) nullptr);
    this->scope->set("it", *v);
    UExpressionInfo true_case_p = this->dispatch_rvalue(*node.true_case);
    ExpressionInfo& true_case = *true_case_p;
    this->leave_scope();
    EntityValue& true_value = true_case.entity.get().get_value();
    UExpressionInfo false_case_sinfo = this->expect_rvalue_of_type(true_value.type, *node.false_case);
    if (false_case_sinfo->is_error()) {
        return exp_error_stub();
    }
    auto false_case_snode = std::move(false_case_sinfo->exp_snode);
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(this->make_value(true_value.type.clone()));
    info.exp_snode = std::make_unique<sem::Ternary>(expression_info_p->exp_snode.release(),
                                                    true_case.exp_snode.release(),
                                                    false_case_snode.release());
    return info_u;
}