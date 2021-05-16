//
// Created by chris on 2/5/21.
//

#include <cassert>
#include "CheckMember.h"

USemanticInfo Checker::visit_member(MemberNode& n) {
    USemanticInfo parent_info = this->dispatch(n.parent);
    Entity parent_entity = parent_info->entity;
    if (n.type == MemberType::NUM) {
        ObjectType* ot = &parent_entity.value->type->object();
        if (ot->id != "Tuple") {
            this->error_reporter.fail("Integer member of not a Tuple!");
        }
        size_t tuple_size = ot->type_params.size();
        if (n.n_child > tuple_size || n.n_child == 0) {
            this->error_reporter.object_no_member(*ot, n);
            // this->error_reporter.fail(
            //         "Tuple member out of range, has " + std::to_string(tuple_size) + " but required " +
            //         std::to_string(n.n_child));
            return error_stub();
        }
        SemanticInfo info;
        ObjectMemberSNode* omsn = new ObjectMemberSNode();
        info.snode = omsn;
        omsn->object = parent_info->snode;
        omsn->member_name = "mem_" + std::to_string(n.n_child);
        omsn->class_path = ot->actual_base_path;
        Value* ov = new Value(ot->type_params[n.n_child - 1]->clone());
        info.entity = Entity(ov);
        return std::make_unique<SemanticInfo>(info);
    }
    switch (parent_entity.type) {
        case E_TYPE::CLASS:
            return this->class_member(parent_entity.clazz, n.s_child, n);
        case E_TYPE::CONST_FUNCTION:
            this->error_reporter.object_no_member(*parent_entity.const_function->ft, n);
            // this->error_reporter.function_no_member(n.dot_pos);
            break;
        case E_TYPE::VALUE:
            if (parent_entity.value->type->kind == Kind::FUNCTION) {
                this->error_reporter.object_no_member(*parent_entity.value->type, n);
                return error_stub();
            }
            return this->object_member(parent_info->snode, parent_entity.value, n.s_child, n);
        case E_TYPE::PACKAGE:
            return this->package_member(parent_entity.package, n.s_child, n);
        case E_TYPE::MODULE:
            return this->module_member(parent_entity.module, n.s_child, n);
        case E_TYPE::ERROR:
            break;
        case E_TYPE::NOT_FOUND:
            break;
        case E_TYPE::ENUM:
            return this->enum_member(parent_entity.enumm, n.s_child, n);
    }
    return error_stub();
}

USemanticInfo Checker::module_member(Module* mod, std::string child, MemberNode& n) {
    if (mod->flirpins.count(child) == 0) {
        this->error_reporter.module_no_member(mod, child, n.dot_pos, *n.parent, add_one_col(n.dot_pos), n.end);
        return error_stub();
    }
    Flirpin flirpin = mod->flirpins[child];
    SemanticInfo info;
    info.entity = map_flirpin_to_entity(flirpin);
    if (flirpin.type == F_TYPE::CONST_FUNCTION) {
        IdSNode* idn = new IdSNode();
        idn->identifier = flirpin.const_function->path.as_str();
        info.snode = idn;
    }
    return std::make_unique<SemanticInfo>(info);
}

TextPosition add_one_col(TextPosition t) {
    return {t.line, t.column + 1};
}

USemanticInfo Checker::object_member(SNode* object_snode, Value* pValue, std::string child, MemberNode& n) {
    Path object_type_path = pValue->type->object().actual_base_path;
    if (object_type_path.as_str() == "") {
        // is a single type param, error
        this->error_reporter.fail(
                "Error: no member " + child + " in totally generic type " + pValue->type->to_string());
    }
    if (object_type_path.as_str() == "core.Union") {
        this->error_reporter.object_no_member(*pValue->type, n);
        return error_stub();
    }
    if (pValue->metatype == Meta::ENUM) {
        this->error_reporter.object_no_member(*pValue->type, n);
    }
    Class* clazz = pValue->clazz;
    assert(clazz != nullptr);
    SemanticInfo info;
    if (clazz->members.count(child)) {
        info.entity = clazz->member_entities[child];
        if (info.entity.type == E_TYPE::NOTHING) {
            info.entity = entity_from_type(*clazz->members[child]);
            clazz->member_entities[child] = info.entity;
            this->fill_value(info.entity.value);
        }
        ObjectMemberSNode* omn = new ObjectMemberSNode();
        omn->class_path = clazz->path;
        omn->object = object_snode;
        omn->member_name = child;
        info.snode = omn;
    } else if (clazz->methods.count(child)) {
        IdSNode* idn = new IdSNode();
        idn->identifier = Path(clazz->path, child).as_str();
        if (this->is_call) {
            // method call
            info.this_arg = object_snode;
            info.snode = idn;
            info.entity = Entity(clazz->methods[child]);
        } else {
            // return partial
            int npartial = clazz->methods[child]->ft->param_types.size();
            NewObjectSNode* non = new NewObjectSNode();
            non->class_name = "Partial" + std::to_string(npartial);
            IdSNode* method_snode = new IdSNode(clazz->methods[child]->path.as_str());
            non->args = {method_snode, object_snode};
            for (int i = 0; i < npartial; i++) {
                non->args.push_back(nullptr);
            }
            info.snode = non;
            Value* fv = new Value(clazz->methods[child]->ft->clone());
            info.entity = Entity(fv);
        }

    } else {
        this->error_reporter.object_no_member_with_suggestions(*pValue->type,
                                                               child,
                                                               n.dot_pos,
                                                               *n.parent,
                                                               add_one_col(n.dot_pos),
                                                               n.end,
                                                               clazz);

        return error_stub();
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::package_member(Package* package, std::string child, MemberNode& n) {
    if (package->units.count(child) == 0) {
        this->error_reporter.package_no_member(package, child, n.dot_pos, *n.parent, add_one_col(n.dot_pos), n.end);
        return error_stub();
    }
    Unit unit = package->units[child];
    SemanticInfo info;
    info.entity = map_flirpin_to_entity(map_unit_to_flirpin(unit));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::class_member(Class* cls, std::string child, MemberNode& n) {
    SemanticInfo info;
    if (cls->methods.find(child) != cls->methods.end()) {
        ConstFunction* bound_method = cls->methods[child];
        ConstFunction* unbound_method = new ConstFunction();
        unbound_method->path = bound_method->path;
        unbound_method->ft = bound_method->ft->clone();
        unbound_method->ft->param_types.insert(unbound_method->ft->param_types.begin(),
                                               new ObjectType(cls->class_name));
        info.entity = Entity(unbound_method);
        info.snode = new IdSNode(unbound_method->path.as_str());
    } else if (cls->static_methods.find(child) != cls->static_methods.end()) {
        info.entity = Entity(cls->static_methods[child]);
        info.snode = new IdSNode(cls->static_methods[child]->path.as_str());
    } else if (cls->static_members.find(child) != cls->static_members.end()) {
        info.entity = entity_from_type(*cls->static_members[child].first);
    } else {
        this->error_reporter.class_no_member(ObjectType(cls->class_name, {}),
                                             child,
                                             n.dot_pos,
                                             *n.parent,
                                             add_one_col(n.dot_pos),
                                             n.end);
        return error_stub();
    }
    return std::make_unique<SemanticInfo>(info);
}
