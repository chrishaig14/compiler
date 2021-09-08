//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckMember.h"
#include "../ast/ObjectType.h"
#include "../simple_nodes/with_unique/ObjectMember.h"
#include "../simple_nodes/with_unique/ObjectMethodCall.h"
#include "../simple_nodes/with_unique/ObjectMethod.h"
#include "../simple_nodes/TypeObject.h"
#include "../simple_nodes/TypeFunction.h"
#include "errors/ErrorNoMember.h"
#include "errors/ErrorNoMemberSuggestions.h"
#include "errors/ErrorClassNoMember.h"
#include "errors/ErrorPackageNoMember.h"

USemanticInfo Checker::visit_member(ast::Member& n) {
    USemanticInfo parent_info = this->dispatch(n.parent);
    Entity& parent_entity = parent_info->entity.get();
    switch (parent_entity.type) {
        case E_TYPE::CLASS:
            return this->class_member(((EntityClass&) parent_entity).clazz, n.s_child, n);
        case E_TYPE::CONST_FUNCTION:
            this->error_reporter.error(std::make_unique<ErrorNoMember>(*((EntityConstFunction&) parent_entity).const_function->const_function_ft.to_ast(),
                                                                       n));
            // this->error_reporter.object_no_member(*parent_entity.const_function->ft, n);
            break;
        case E_TYPE::VALUE:
            if (((Value&) parent_entity).type.kind == sem::Kind::FUNCTION) {
                this->error_reporter.error(std::make_unique<ErrorNoMember>(*((EntityConstFunction&) parent_entity).const_function->const_function_ft.to_ast(),
                                                                           n));
                // this->error_reporter.object_no_member(*parent_entity.value->type, n);
                return error_stub();
            }
            return this->object_member(std::move(parent_info->snode), ((Value&) parent_entity), n.s_child, n);
        case E_TYPE::PACKAGE:
            return this->package_member(*((EntityPackage&) parent_entity).package, n.s_child, n);
        case E_TYPE::MODULE:
            return this->module_member(*((EntityModule&) parent_entity).module, n.s_child, n);
        case E_TYPE::ENUM:
            return this->enum_member(((EntityEnum&) parent_entity).enumm, n.s_child, n);
        default:
            break;
    }
    return error_stub();
}

USemanticInfo Checker::module_member(Module& mod, const std::string& child, ast::Member& n) {
    if (mod.flirpins.count(child) == 0) {
        // this->error_reporter.error(std::make_unique<ErrorNoMember>())
        // this->error_reporter.module_no_member(&mod,
        //                                       child,
        //                                       n.dot_pos,
        //                                       *n.parent,
        //                                       n.child_token.start,
        //                                       n.child_token.end_pos);
        return error_stub();
    }
    Flirpin flirpin = mod.flirpins[child];
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(map_flirpin_to_entity(flirpin));
    if (flirpin.type == F_TYPE::CONST_FUNCTION) {
        auto idn = std::make_unique<sem::Id>(flirpin.const_function->path.as_str());
        info.snode = std::move(idn);
    }
    return info_u;
}

TextPosition add_one_col(TextPosition t) {
    return {t.line, t.column + 1};
}

USemanticInfo Checker::object_member(USNode object_snode, Value& p_value, const std::string& child, ast::Member& n) {
    Path object_type_path = p_value.type.object().data.actual_base_path;
    // if (object_type_path.as_str() == "") {
    //     // is a single type param, error
    //     this->error_reporter.object_no_member(*p_value.type, n);
    //     return error_stub();
    // }
    if (object_type_path.as_str() == "core.core.Union") {
        this->error_reporter.error(std::make_unique<ErrorNoMember>(*p_value.type.to_ast(), n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return error_stub();
    }
    if (p_value.metatype == Meta::ENUM) {
        this->error_reporter.error(std::make_unique<ErrorNoMember>(*p_value.type.to_ast(), n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return error_stub();
    }
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    if (p_value.type.kind == sem::Kind::OBJECT && p_value.type.object().id == "Tuple") {
        info.is_tuple_member = true;
    }
    Class* clazz = p_value.clazz;
    assert(clazz != nullptr);
    if (clazz->members.count(child) != 0) {
        info.set_entity(clazz->member_entities.at(child));
        if (info.entity.get().type == E_TYPE::NOTHING) {
            info.set_entity(entity_from_type(*clazz->members.at(child)));
            clazz->member_entities[child] = &info.entity.get();
            Value& ev = (Value&) info.entity.get();
            this->fill_value(ev);
        }
        auto omn = std::make_unique<sem::ObjectMember>(std::move(object_snode), clazz->path, child);
        info.snode = std::move(omn);
    } else if (clazz->methods.count(child) != 0) {
        // auto* idn = new sem::Id(clazz->methods[child]->path.as_str());
        info.snode = std::make_unique<sem::ObjectMethod>(std::move(object_snode), clazz->path, child);
        info.set_entity(new EntityConstFunction(clazz->methods[child]));
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
        this->error_reporter.error(std::make_unique<ErrorNoMemberSuggestions>(*p_value.type.to_ast(), n, *clazz));
        // this->error_reporter.object_no_member_with_suggestions(*p_value.type,
        //                                                        child,
        //                                                        n.dot_pos,
        //                                                        *n.parent,
        //                                                        add_one_col(n.dot_pos),
        //                                                        n.end,
        //                                                        clazz);

        return error_stub();
    }
    return info_u;
}

USemanticInfo Checker::package_member(Package& package, const std::string& child, ast::Member& n) {
    if (package.units.count(child) == 0) {
        this->error_reporter.error(std::make_unique<ErrorPackageNoMember>(&package,
                                                                          child,
                                                                          n.dot_pos,
                                                                          n.parent,
                                                                          n.child_token.start,
                                                                          n.child_token.end_pos));
        return error_stub();
    }
    Unit unit = package.units[child];
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(map_flirpin_to_entity(map_unit_to_flirpin(unit)));
    return info_u;
}

USemanticInfo Checker::class_member(Class* cls, const std::string& child, ast::Member& n) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    if (cls->methods.find(child) != cls->methods.end()) {
        ConstFunction* bound_method = cls->methods[child];
        auto* unbound_method = new ConstFunction(bound_method->path, bound_method->const_function_ft.clone());
        ast::VectorOfTypes tp;
        for (auto tt: cls->type_params) {
            ast::ObjectType* t = new ast::ObjectType(tt);
            t->is_generic_param = true;
            tp.push_back(t);
        }
        ast::ObjectType* ot = new ast::ObjectType(cls->class_name, tp);
        unbound_method->const_function_ft.param_types.insert(unbound_method->const_function_ft.param_types.begin(),
                                                             sem::UType(ot->to_sem()));
        info.set_entity(new EntityConstFunction(unbound_method));
        info.snode = std::make_unique<sem::Id>(unbound_method->path.as_str());
    } else if (cls->static_methods.find(child) != cls->static_methods.end()) {
        info.set_entity(new EntityConstFunction(cls->static_methods[child]));
        info.snode = std::make_unique<sem::Id>(cls->static_methods[child]->path.as_str());
    } else if (cls->static_members.find(child) != cls->static_members.end()) {
        info.set_entity(entity_from_type(*cls->static_members[child].first));
    } else {
        this->error_reporter.error(std::make_unique<ErrorClassNoMember>(ast::ObjectType(cls->class_name, {}),
                                                                        child,
                                                                        n.dot_pos,
                                                                        n.parent,
                                                                        add_one_col(n.dot_pos),
                                                                        n.end));
        return error_stub();
    }
    return info_u;
}
