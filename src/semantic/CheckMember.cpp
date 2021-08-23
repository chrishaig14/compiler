//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckMember.h"
#include "../nodes/ObjectType.h"
#include "errors/ErrorNoMember.h"
#include "errors/ErrorNoMemberSuggestions.h"
#include "errors/ErrorClassNoMember.h"
#include "errors/ErrorPackageNoMember.h"

USemanticInfo Checker::visit_member(MemberNode& n) {
    USemanticInfo parent_info = this->dispatch(*n.parent);
    Entity& parent_entity = parent_info->entity.get();
    switch (parent_entity.type) {
        case E_TYPE::CLASS:
            return this->class_member(((EntityClass&) parent_entity).clazz, n.s_child, n);
        case E_TYPE::CONST_FUNCTION:
            this->error_reporter.error(ErrorNoMember(*((EntityConstFunction&) parent_entity).const_function->ft, n));
            // this->error_reporter.object_no_member(*parent_entity.const_function->ft, n);
            break;
        case E_TYPE::VALUE:
            if (((EntityValue&) parent_entity).value->type->kind == Kind::FUNCTION) {
                this->error_reporter.error(ErrorNoMember(*((EntityConstFunction&) parent_entity).const_function->ft,
                                                         n));
                // this->error_reporter.object_no_member(*parent_entity.value->type, n);
                return error_stub();
            }
            return this->object_member(parent_info->snode, *((EntityValue&) parent_entity).value, n.s_child, n);
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

USemanticInfo Checker::module_member(Module& mod, const std::string& child, MemberNode& n) {
    if (mod.flirpins.count(child) == 0) {
        // this->error_reporter.error(ErrorNoMember())
        // this->error_reporter.module_no_member(&mod,
        //                                       child,
        //                                       n.dot_pos,
        //                                       *n.parent,
        //                                       n.child_token.start,
        //                                       n.child_token.end_pos);
        return error_stub();
    }
    Flirpin flirpin = mod.flirpins[child];
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.entity = *map_flirpin_to_entity(flirpin);
    if (flirpin.type == F_TYPE::CONST_FUNCTION) {
        auto* idn = new IdSNode(flirpin.const_function->path.as_str());
        info.snode = idn;
    }
    return info_u;
}

TextPosition add_one_col(TextPosition t) {
    return {t.line, t.column + 1};
}

USemanticInfo Checker::object_member(SNode* object_snode, Value& p_value, const std::string& child, MemberNode& n) {
    Path object_type_path = p_value.type->object().actual_base_path;
    // if (object_type_path.as_str() == "") {
    //     // is a single type param, error
    //     this->error_reporter.object_no_member(*p_value.type, n);
    //     return error_stub();
    // }
    if (object_type_path.as_str() == "core.core.Union") {
        this->error_reporter.error(ErrorNoMember(*p_value.type, n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return error_stub();
    }
    if (p_value.metatype == Meta::ENUM) {
        this->error_reporter.error(ErrorNoMember(*p_value.type, n));
        // this->error_reporter.object_no_member(*p_value.type, n);
        return error_stub();
    }
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    if (p_value.type->kind == Kind::OBJECT && p_value.type->object().id == "Tuple") {
        info.is_tuple_member = true;
    }
    Class* clazz = p_value.clazz;
    assert(clazz != nullptr);
    if (clazz->members.count(child) != 0) {
        info.entity = *clazz->member_entities.at(child);
        if (info.entity.get().type == E_TYPE::NOTHING) {
            info.entity = *entity_from_type(*clazz->members.at(child));
            clazz->member_entities[child] = &info.entity.get();
            auto* ev = &(EntityValue&) info.entity.get();
            Value& vup = *(ev->value);
            this->fill_value(vup);
        }
        auto* omn = new ObjectMemberSNode(object_snode, clazz->path, child);
        info.snode = omn;
    } else if (clazz->methods.count(child) != 0) {
        auto* idn = new IdSNode(clazz->methods[child]->path.as_str());
        if (this->is_call) {
            // method call
            info.this_arg = object_snode;
            info.snode = idn;
            info.entity = *new EntityConstFunction(clazz->methods[child]);
        } else {
            // return partial
            size_t npartial = clazz->methods[child]->ft->param_types.size();
            auto* non = new NewObjectSNode();
            non->class_name = "Partial" + std::to_string(npartial);
            auto* method_snode = new IdSNode(clazz->methods[child]->path.as_str());
            non->args = {method_snode, object_snode};
            for (size_t i = 0; i < npartial; i++) {
                non->args.push_back(nullptr);
            }
            info.snode = non;
            auto fv = std::make_unique<Value>(clazz->methods[child]->ft->clone());
            info.entity = *new EntityValue(std::move(fv));
        }

    } else {
        this->error_reporter.error(ErrorNoMemberSuggestions(*p_value.type, n, *clazz));
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

USemanticInfo Checker::package_member(Package& package, const std::string& child, MemberNode& n) {
    if (package.units.count(child) == 0) {
        this->error_reporter.error(ErrorPackageNoMember(&package,
                                                        child,
                                                        n.dot_pos,
                                                        *n.parent,
                                                        n.child_token.start,
                                                        n.child_token.end_pos));
        return error_stub();
    }
    Unit unit = package.units[child];
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.entity = *map_flirpin_to_entity(map_unit_to_flirpin(unit));
    return info_u;
}

USemanticInfo Checker::class_member(Class* cls, const std::string& child, MemberNode& n) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    if (cls->methods.find(child) != cls->methods.end()) {
        ConstFunction* bound_method = cls->methods[child];
        auto* unbound_method = new ConstFunction(bound_method->path, bound_method->ft->clone());
        VectorOfTypes tp;
        for (auto tt: cls->type_params) {
            ObjectType* t = new ObjectType(tt);
            t->is_generic_param = true;
            tp.push_back(t);
        }
        ObjectType* ot = new ObjectType(cls->class_name, tp);
        unbound_method->ft->param_types.insert(unbound_method->ft->param_types.begin(), ot);
        info.entity = *new EntityConstFunction(unbound_method);
        info.snode = new IdSNode(unbound_method->path.as_str());
    } else if (cls->static_methods.find(child) != cls->static_methods.end()) {
        info.entity = *new EntityConstFunction(cls->static_methods[child]);
        info.snode = new IdSNode(cls->static_methods[child]->path.as_str());
    } else if (cls->static_members.find(child) != cls->static_members.end()) {
        info.entity = *entity_from_type(*cls->static_members[child].first);
    } else {
        this->error_reporter.error(ErrorClassNoMember(ObjectType(cls->class_name, {}),
                                                      child,
                                                      n.dot_pos,
                                                      *n.parent,
                                                      add_one_col(n.dot_pos),
                                                      n.end));
        return error_stub();
    }
    return info_u;
}
