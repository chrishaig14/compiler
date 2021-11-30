//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckMember.h"
#include <ast/general/ObjectType.h>
#include <simple_nodes/expressions/include/ObjectMethodFromInstance.h>
#include <simple_nodes/expressions/include/StaticMethodFromInstance.h>
#include <simple_nodes/expressions/include/ObjectMember.h>
#include <simple_nodes/expressions/include/ObjectMethod.h>
#include <simple_nodes/expressions/include/StaticMethod.h>
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>

UExpressionInfo ModuleChecker::visit_member(const ast::Member& n) {
    UExpressionInfo parent_info = this->dispatch_rvalue(n.parent);
    Entity& parent_ent = parent_info->entity.get();
    switch (parent_ent.e_type) {
        case E_TYPE::CLASS:
            return this->class_member(n, std::move(parent_info), parent_ent.get_class().clazz);
        case E_TYPE::CONST_FUNCTION:
            return this->const_function_member(n, std::move(parent_info), parent_ent.get_constfun().const_function);
        case E_TYPE::VALUE:
            return this->value_member(n, std::move(parent_info), parent_ent.get_value());
        case E_TYPE::PACKAGE:
            return this->package_member(n, parent_ent.get_package().package);
        case E_TYPE::MODULE:
            return this->module_member(n, parent_ent.get_module().module);
        case E_TYPE::ENUM:
            return this->enum_member(n, parent_ent.get_enum().enumm);
        case E_TYPE::ERROR:
            break;
        case E_TYPE::NOT_FOUND:
            break;
        case E_TYPE::NOTHING:
            break;
        case E_TYPE::NONE:
            break;
        case E_TYPE::TYPECLASS:
            break;
        case E_TYPE::TEMPLATE_CLASS:
            break;
    }
    return exp_error_stub();
}

UExpressionInfo ModuleChecker::module_member(const ast::Member& n, const Module& mod) {
    std::string child = n.s_child;
    if (mod.members.count(child) == 0) {
        // this->error_reporter.error(std::make_unique<error::ErrorNoMember>())
        // this->error_reporter.module_no_member(&mod,
        //                                       child,
        //                                       n.dot_pos,
        //                                       *n.parent,
        //                                       n.child_token.start,
        //                                       n.child_token.end_pos);
        return exp_error_stub();
    }
    const ModuleMember& member = *mod.members.at(child);
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(map_module_member_to_entity(member));
    if (member.is_const_function()) {
        auto idn = std::make_unique<sem::ConstFunction>(member.const_function().path);
        info.exp_snode = std::move(idn);
    }
    return info_u;
}

TextPosition add_one_col(TextPosition t) {
    return {t.line, t.column + 1};
}

UExpressionInfo ModuleChecker::object_member(sem::UExp object_snode, EntityValue& p_value, const std::string& child,
                                             const ast::Member& n) {
    Path object_type_path = p_value.type.object().data.actual_base_path;
    // if (object_type_path.as_str() == "") {
    //     // is a single type param, error
    //     this->error_reporter.object_no_member(*p_value.type, n);
    //     return error_stub();
    // }
    if (object_type_path.as_str() == "libcore.libcore.Union") {
        std::cout << "p_value is union" << std::endl;
        this->error_reporter.error(std::make_unique<error::NoMember>(p_value.type, n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return exp_error_stub();
    }
    if (p_value.metatype == Meta::ENUM) {
        std::cout << "p_value is enum" << std::endl;
        this->error_reporter.error(std::make_unique<error::NoMember>(p_value.type, n));
        return exp_error_stub();
    }
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    if (p_value.type.kind == sem::Kind::OBJECT && p_value.type.object().id == "Tuple") {
        info.is_tuple_member = true;
    }
    const ConcreteClass* clazz = p_value.clazz;
    assert(clazz != nullptr);
    auto member_cat = clazz->get_member(child);
    switch (member_cat) {
        case ClassMemberCategory::attribute: {
            sem::Type* type = clazz->attributes.at(child)->to_sem();
            this->module.fill_actual(*type);
            info.set_entity(this->make_value(type));
            if (info.entity.get().is_nothing()) {
                throw std::runtime_error("This shouldnt be nothing!");
                // sem::UType p_type(clazz->members.at(child)->to_sem());
                // this->module.fill_actual(*p_type);
                // auto eee = this->make_entity_value(*p_type);
                // info.set_entity(eee->clone());
                // clazz->member_entities[child] = std::move(eee);
            }
            auto omn = std::make_unique<sem::ObjectMember>(std::move(object_snode), clazz->path, child);
            info.exp_snode = std::move(omn);
            break;
        }
        case ClassMemberCategory::method: {
            InstanceMethod& im = *clazz->methods.at(child);

            if (im.instance == Path("")) {
                std::cout << "found method " << child << " for class " << clazz->class_name << " from base class "
                          << std::endl;
                info.exp_snode = std::make_unique<sem::ObjectMethod>(std::move(object_snode), clazz->path, child);
                info.set_entity(std::make_unique<EntityConstFunction>(*im.base->func));
            } else {
                std::cout << "found method " << child << " for class " << clazz->class_name
                          << " from instance of typeclass " << im.instance.as_str() << std::endl;
                info.exp_snode = std::make_unique<sem::ObjectMethodFromInstance>(std::move(object_snode),
                                                                                 std::make_unique<sem::InstanceObject>(
                                                                                         clazz->path,
                                                                                         im.instance),
                                                                                 child);
                info.set_entity(std::make_unique<EntityConstFunction>(*im.base->func));
            }
            break;
        }
        default: {
            this->error_reporter.error(std::make_unique<error::NoMemberSuggestions>(p_value.type, n, *clazz));
            return exp_error_stub();
        }
    }

    return info_u;
}

UExpressionInfo ModuleChecker::package_member(const ast::Member& n, const Package& package) {
    std::string child = n.s_child;
    if (package.units.count(child) == 0) {
        throw std::runtime_error("Error package no member!");
        return exp_error_stub();
    }
    Unit* unit = package.units.at(child).get();
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(map_module_member_to_entity(*map_unit_to_module_member(*unit)));
    return info_u;
}

UExpressionInfo ModuleChecker::class_member(const ast::Member& n, UExpressionInfo parent_info, ConcreteClass& cls) {
    std::string child = n.s_child;
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    ClassMemberCategory member_cat = cls.get_member(child);
    switch (member_cat) {
        case ClassMemberCategory::not_found: {
            throw std::runtime_error("Error class has no member '" + child + "'");
        }
        case ClassMemberCategory::attribute: {
            throw std::runtime_error("Error class no member '" + child + "'(it's an instance variable)!");
        }
        case ClassMemberCategory::static_attribute: {
            info.set_entity(entity_from_type(*cls.static_attributes[child].first));
            break;
        }
        case ClassMemberCategory::method: {
            InstanceMethod& im = *cls.methods[child];
            if (im.base->is_static) {
                if (im.instance == Path("")) {
                    std::cout << "found method " << child << " for class " << cls.class_name << " from base class "
                              << std::endl;
                    info.exp_snode = std::make_unique<sem::StaticMethod>(cls.path, child);
                    info.set_entity(std::make_unique<EntityConstFunction>(*im.base->func));
                } else {
                    std::cout << "found method " << child << " for class " << cls.class_name
                              << " from instance of typeclass " << im.instance.as_str() << std::endl;
                    info.exp_snode = std::make_unique<sem::StaticMethodFromInstance>(std::make_unique<sem::InstanceObject>(
                            cls.path,
                            im.instance), child);
                    info.set_entity(std::make_unique<EntityConstFunction>(*im.base->func));
                }
            } else {
                ConstFunction& bound_method = *im.base->func;
                auto* unbound_method = new ConstFunction(bound_method.path, bound_method.const_function_ft);
                ast::VectorOfTypes tp;
                for (auto tt: cls.type_params) {
                    ast::ObjectType* t = new ast::ObjectType(tt);
                    t->is_generic_param = true;
                    tp.push_back(t);
                }
                ast::ObjectType* ot = new ast::ObjectType(cls.class_name, tp);
                unbound_method->const_function_ft.param_types.insert(unbound_method->const_function_ft.param_types.begin(),
                                                                     sem::UType(ot->to_sem()));
                info.set_entity(std::make_unique<EntityConstFunction>(*unbound_method));
                info.exp_snode = std::make_unique<sem::Id>(unbound_method->path.as_str());
            }
            break;
        }
    }
    return info_u;
}
