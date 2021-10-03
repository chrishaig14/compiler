//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckMember.h"
#include "../ast/general/ObjectType.h"
#include "../simple_nodes/expressions/include/ObjectMember.h"
#include "../simple_nodes/expressions/include/ObjectMethod.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/include/TypeFunction.h"

UExpressionInfo Checker::visit_member(ast::Member& n) {
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
    }
    return exp_error_stub();
}

UExpressionInfo Checker::module_member(ast::Member& n, Module& mod) {
    std::string child = n.s_child;
    if (mod.members.count(child) == 0) {
        // this->error_reporter.error(std::make_unique<ErrorNoMember>())
        // this->error_reporter.module_no_member(&mod,
        //                                       child,
        //                                       n.dot_pos,
        //                                       *n.parent,
        //                                       n.child_token.start,
        //                                       n.child_token.end_pos);
        return exp_error_stub();
    }
    ModuleMember& member = *mod.members[child];
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

UExpressionInfo
Checker::object_member(sem::UExp object_snode, EntityValue& p_value, const std::string& child, ast::Member& n) {
    Path object_type_path = p_value.type.object().data.actual_base_path;
    // if (object_type_path.as_str() == "") {
    //     // is a single type param, error
    //     this->error_reporter.object_no_member(*p_value.type, n);
    //     return error_stub();
    // }
    if (object_type_path.as_str() == "libcore.libcore.Union") {
        this->error_reporter.error(std::make_unique<ErrorNoMember>(p_value.type, n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return exp_error_stub();
    }
    if (p_value.metatype == Meta::ENUM) {
        this->error_reporter.error(std::make_unique<ErrorNoMember>(p_value.type, n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return exp_error_stub();
    }
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    if (p_value.type.kind == sem::Kind::OBJECT && p_value.type.object().id == "Tuple") {
        info.is_tuple_member = true;
    }
    Class* clazz = p_value.clazz;
    assert(clazz != nullptr);
    if (clazz->members.count(child) != 0) {
        info.set_entity(clazz->member_entities.at(child)->clone());
        if (info.entity.get().is_nothing()) {
            // auto eee = entity_from_type(*clazz->members.at(child));
            // this->fill_value(eee->get_value());
            sem::UType p_type(clazz->members.at(child)->to_sem());
            auto eee = this->make_entity_value(*p_type);
            info.set_entity(eee->clone());
            clazz->member_entities[child] = std::move(eee);
        }
        auto omn = std::make_unique<sem::ObjectMember>(std::move(object_snode), clazz->path, child);
        info.exp_snode = std::move(omn);
    } else if (clazz->methods.count(child) != 0) {
        // auto* idn = new sem::Id(clazz->methods[child]->path.as_str());
        info.exp_snode = std::make_unique<sem::ObjectMethod>(std::move(object_snode), clazz->path, child);
        info.set_entity(new EntityConstFunction(*clazz->methods[child]));
        // if (this->is_call) {
        // method call
        // info.this_arg = object_snode.release();
        // info.snode = idn;
        // info.snode = new sem::ObjectMethod(std::move(object_snode), clazz->path, child);
        // info.entity = *new EntityValue(std::make_unique<Value>(clazz->methods[child]->ft->clone()));
        // } else {
        // return partial
        // size_t npartial = clazz->methods[child]->ft->param_types.size();
        // auto* non = new sem::NewObject();
        // non->class_name = "Partial" + std::to_string(npartial);
        // auto* method_snode = new sem::Id(clazz->methods[child]->path.as_str());
        // non->args = {method_snode, object_snode.release()};
        // for (size_t i = 0; i < npartial; i++) {
        //     non->args.push_back(nullptr);
        // }
        // info.snode = non;
        // auto fv = std::make_unique<Value>(clazz->methods[child]->ft->clone());
        // info.entity = *new EntityValue(std::move(fv));
        // }

    } else {
        this->error_reporter.error(std::make_unique<ErrorNoMemberSuggestions>(p_value.type, n, *clazz));
        // this->error_reporter.object_no_member_with_suggestions(*p_value.type,
        //                                                        child,
        //                                                        n.dot_pos,
        //                                                        *n.parent,
        //                                                        add_one_col(n.dot_pos),
        //                                                        n.end,
        //                                                        clazz);

        return exp_error_stub();
    }
    return info_u;
}

UExpressionInfo Checker::package_member(ast::Member& n, Package& package) {
    std::string child = n.s_child;
    if (package.units.count(child) == 0) {
        // this->error_reporter.error(std::make_unique<ErrorPackageNoMember>(&package,
        //                                                                   child,
        //                                                                   n.dot_pos,
        //                                                                   n.parent,
        //                                                                   n.child_token.start,
        //                                                                   n.child_token.end_pos));
        throw std::runtime_error("Error package no member!");
        return exp_error_stub();
    }
    Unit* unit = package.units[child].get();
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(map_module_member_to_entity(*map_unit_to_module_member(*unit)));
    return info_u;
}

UExpressionInfo Checker::class_member(ast::Member& n, UExpressionInfo parent_info, Class& cls) {
    std::string child = n.s_child;
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    if (cls.methods.find(child) != cls.methods.end()) {
        ConstFunction& bound_method = *cls.methods[child];
        auto* unbound_method = new ConstFunction(bound_method.path,
                                                 sem::UTypeFunction((sem::TypeFunction*) bound_method.const_function_ft.clone()));
        ast::VectorOfTypes tp;
        for (auto tt: cls.type_params) {
            ast::ObjectType* t = new ast::ObjectType(tt);
            t->is_generic_param = true;
            tp.push_back(t);
        }
        ast::ObjectType* ot = new ast::ObjectType(cls.class_name, tp);
        unbound_method->const_function_ft.param_types.insert(unbound_method->const_function_ft.param_types.begin(),
                                                             sem::UType(ot->to_sem()));
        info.set_entity(new EntityConstFunction(*unbound_method));
        info.exp_snode = std::make_unique<sem::Id>(unbound_method->path.as_str());
    } else if (cls.static_methods.find(child) != cls.static_methods.end()) {
        info.set_entity(new EntityConstFunction(*cls.static_methods[child]));
        info.exp_snode = std::make_unique<sem::Id>(cls.static_methods[child]->path.as_str());
    } else if (cls.static_members.find(child) != cls.static_members.end()) {
        info.set_entity(entity_from_type(*cls.static_members[child].first));
    } else {
        throw std::runtime_error("Error class no member!");

        // this->error_reporter.error(std::make_unique<ErrorClassNoMember>(ast::ObjectType(cls->class_name, {}),
        //                                                                 child,
        //                                                                 n.dot_pos,
        //                                                                 n.parent,
        //                                                                 add_one_col(n.dot_pos),
        //                                                                 n.end));        return exp_error_stub();
    }
    return info_u;
}
