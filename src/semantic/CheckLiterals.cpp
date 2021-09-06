//
// Created by chris on 2/5/21.
//

#include "CheckLiterals.h"
#include "../simple_nodes/with_unique/None.h"
#include "../simple_nodes/with_unique/Dict.h"
#include "errors/ErrorExpectedExpression.h"
#include "errors/ErrorListLiteral.h"
#include "errors/ErrorPartialWrongNumArgs.h"
#include "errors/ErrorTypeMismatch.h"

USemanticInfo Checker::visit_boolean(ast::Boolean& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("core.core.Boolean")).clone());
    info.snode = std::make_unique<sem::Bool>(node.value);
    return info_u;
}


USemanticInfo Checker::visit_number(ast::Number& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("core.core.Integer")).clone());
            info.snode = std::make_unique<sem::Integer>(node.str);
            break;
        }
        case NumberType::FLOAT: {
            info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("core.core.Float")).clone());
            // auto* otype = new ast::ObjectType("Float", {});
            // otype->actual_base_path = Path("core.core.Float");
            // auto ov = std::make_unique<Value>(otype);
            // this->fill_value(*ov);
            // info.entity = new EntityValue(std::move(ov));
            info.snode = std::make_unique<sem::Float>(node.str);
            break;
        }
        case NumberType::DOUBLE: {
            // ObjectValue* ov = new ObjectValue();
            // info.entity = Entity{.type=E_TYPE::OBJECT_VALUE, .object_value=ov};
            // ov->ot = new ast::ObjectType("Double", {});
            // IntegerSNode* snode = new IntegerSNode();
            // snode->str = node.str;
            // info.snode = snode;
            break;
        }
    }
    info.is_constant = true;
    return info_u;
}

USemanticInfo Checker::visit_none(ast::None& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    // info.set_type(ObjectType("NoneType"));
    auto v = std::make_unique<Value>(new ast::ObjectType("NoneType"));
    info.set_entity(v.release());
    info.snode = std::make_unique<sem::None>();
    return info_u;
}

USemanticInfo Checker::visit_emptylist(ast::EmptyList& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    this->module.fill_actual(*node.type);
    auto* otype = new ast::ObjectType("List", {node.type->clone()});
    auto ov = std::make_unique<Value>(otype);
    otype->data.actual_base_path = Path("core.core.List");
    this->fill_value(*ov);
    info.set_entity(ov.release());
    std::vector<USNode> v;
    info.snode = std::make_unique<sem::List>(std::move(v));
    // non->class_name = "core.List";
    return info_u;
}

USemanticInfo Checker::visit_string(ast::String& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    info.is_constant = true;
    info.snode = std::make_unique<sem::String>(node.str);
    // auto* otype = new ast::ObjectType("String", {});
    // otype->actual_base_path = Path("core.core.String");
    // auto ov = std::make_unique<Value>(otype);
    // this->fill_value(*ov);
    // info.entity = new EntityValue(std::move(ov));
    info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("core.core.String")).clone());
    return info_u;
}

USemanticInfo Checker::visit_tuple(ast::Tuple& node) {
    ast::VectorOfTypes types;
    std::vector<USNode> values;
    for (auto& n: node.values) {
        USemanticInfo vtype = this->dispatch(*n);
        values.push_back(std::move(vtype->snode));
        types.emplace_back(((Value&) vtype->entity).type.clone());
        // if (!this->is_immutable(vtype->type())) {
        //     this->error_reporter.tuple_member_not_immutable(vtype->type(), node.start);
        //     return error_stub();
        // }
    }
    USemanticInfo sinfo_p = std::make_unique<SemanticInfo>();
    auto& sinfo = *sinfo_p;
    auto* otype = new ast::ObjectType("Tuple", types);
    auto ov = std::make_unique<Value>(otype);
    ov->metatype = Meta::CLASS;

    ov->clazz = new Class("Tuple", Path("core.core.Tuple"));
    for (size_t i = 0; i < ov->type.object().type_params.size(); i++) {
        auto tv = std::make_unique<Value>(ov->type.object().type_params[i]->clone());
        this->fill_value(*tv);
        const std::string& mem_name = std::to_string(i + 1);
        ov->clazz->members[mem_name] = tv->type.clone();
        ov->clazz->member_entities[mem_name] = tv.release();
    }
    sinfo.set_entity(ov.release());
    unsigned long num_values = node.values.size();
    otype->data.actual_base_path = Path("core.Tuple" + std::to_string(num_values));
    auto nosn = std::make_unique<sem::NewObject>();
    nosn->class_name = otype->data.actual_base_path.as_str();
    nosn->args = std::move(values);
    sinfo.snode = std::move(nosn);
    return sinfo_p;
}

USemanticInfo Checker::visit_partial(ast::PartialApplication& node) {
    USemanticInfo func = this->dispatch(*node.function);
    ast::VectorOfTypes partial_args;
    ast::FunctionType* fun_type = nullptr;
    Entity& f_entity = func->entity;
    if (f_entity.type == E_TYPE::CONST_FUNCTION ||
        (f_entity.type == E_TYPE::VALUE && ((Value&) f_entity).type.kind == Kind::FUNCTION)) {
        fun_type = ((EntityConstFunction&) f_entity).const_function->const_function_ft.clone();
    } else {
        this->error_reporter.fail("Error: expected a function for partial application");
        return error_stub();
    }
    if (node.args.size() != fun_type->param_types.size()) {
        this->error_reporter.error(std::make_unique<ErrorPartialWrongNumArgs>(node.start));
        return error_stub();
    }
    std::vector<USNode> snodes;
    int npartial = 0;
    for (size_t i = 0; i < node.args.size(); i++) {
        ast::UTypeNode& param_type = fun_type->param_types[i];
        if (node.args[i] != nullptr) {
            USemanticInfo arg_sinfo = this->expect_rvalue_of_type(*param_type, *node.args[i]);
            if (arg_sinfo->is_error()) {
                return error_stub();
            }
            auto arg_snode = std::move(arg_sinfo->snode);
            snodes.push_back(std::move(arg_snode));
        } else {
            partial_args.push_back(param_type->clone());
            snodes.push_back(nullptr);
            npartial++;
        }
    }
    node.complete_type = &fun_type->clone()->function();
    USemanticInfo s_p = std::make_unique<SemanticInfo>();
    auto& s = *s_p;
    s.entity = *std::make_unique<Value>(new ast::FunctionType(partial_args,
                                                              ast::UTypeNode(fun_type->return_type->clone())));
    auto non = std::make_unique<sem::NewObject>();
    non->class_name = "Partial" + std::to_string(npartial);
    non->args = std::move(snodes);
    non->args.insert(non->args.begin(), std::move(func->snode));
    s.snode = std::move(non);
    return s_p;
}

USemanticInfo Checker::visit_dict(ast::DictNode& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    USemanticInfo first_key_info = this->dispatch(node.items[0].first);
    USemanticInfo first_value_info = this->dispatch(node.items[0].second);
    Value& first_key_entity = (Value&) first_key_info->entity.get();
    Value& first_value_entity = (Value&) first_value_info->entity.get();
    ast::ObjectType& first_key_type = first_key_entity.type.object();
    ast::ObjectType& first_value_type = first_value_entity.type.object();

    std::vector<std::pair<USNode, USNode>> items;
    items.emplace_back(std::move(first_key_info->snode), std::move(first_value_info->snode));
    bool has_error = false;
    for (size_t i = 1; i < node.items.size(); i++) {
        USemanticInfo key_sinfo = this->expect_rvalue_of_type(first_key_type, node.items[i].first);
        if (key_sinfo->is_error()) {
            has_error = true;
        }
        USemanticInfo value_sinfo = this->expect_rvalue_of_type(first_value_type, node.items[i].second);
        if (value_sinfo->is_error()) {
            has_error = true;
        }
        items.emplace_back(key_sinfo->snode.release(), value_sinfo->snode.release());
    }
    if (has_error) {
        return error_stub();
    }
    auto ov = std::make_unique<Value>(new ast::ObjectType("Dict", {first_key_type.clone(), first_value_type.clone()}));
    this->module.fill_actual(ov->type);
    this->fill_value(*ov);
    assert(ov->clazz != nullptr);
    info.set_entity(ov.release());
    info.snode = std::make_unique<sem::Dict>(std::move(items));
    return info_u;
}

USemanticInfo Checker::visit_emptydict(ast::EmptyDict& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    ast::ObjectType* ot = new ast::ObjectType("Dict", {node.key_type->clone(), node.value_type->clone()});
    ot->data.actual_base_path = Path("core.core.Dict");
    auto ov = std::make_unique<Value>(ot);
    this->module.fill_actual(ov->type);
    this->fill_value(*ov);
    assert(ov->clazz != nullptr);
    info.set_entity(ov.release());
    info.snode = std::make_unique<sem::Dict>(std::vector<std::pair<USNode, USNode>>{});
    return info_u;
}

USemanticInfo Checker::visit_defconst(ast::DefaultConstructor& node) {
    // this is a regular function
    USemanticInfo info_u = std::make_unique<SemanticInfo>();
    SemanticInfo& info = *info_u;
    ast::VectorOfTypes t;
    Entity& entity = this->dispatch(*node.class_node)->entity;
    if (entity.type != E_TYPE::CLASS) {
        this->error_reporter.fail("Error not a class");
        info.set_entity(new EntityError());
        return info_u;
    }
    Class& cls = *((EntityClass&) entity).clazz;
    for (auto* pt: cls.member_types) {
        t.push_back(pt->clone());
    }
    ast::VectorOfTypes tp;
    for (const auto& tt: cls.type_params) {
        auto* ot = new ast::ObjectType(tt);
        tp.push_back(ot);
        ot->is_generic_param = true;
    }
    auto* rt = new ast::ObjectType(cls.class_name, tp);
    rt->data.actual_base_path = cls.path;
    info.set_entity(new EntityConstFunction(new ConstFunction(Path(), new ast::FunctionType(t, ast::UTypeNode(rt)))));
    info.snode = std::make_unique<sem::Id>(cls.path.as_str() + "." + "__init__");
    return info_u;
}

USemanticInfo Checker::visit_list(ast::List& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    if (element_type_p->entity.get().type != E_TYPE::VALUE) {
        this->error_reporter.error(std::make_unique<ErrorExpectedExpression>(element_type_p->entity, node.elements[0]));
        return error_stub();
    }
    Value& entity_value = (Value&) element_type_p->entity.get();
    ast::Type* element_type = entity_value.type.clone();
    bool is_constant = true;
    std::vector<USNode> list_elements;
    list_elements.push_back(std::move(element_type_p->snode));

    for (size_t i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
        // const ast::TypeNode& current_type = current_type_p->type();
        // if (!current_type_p->is_constant) {
        //     is_constant = false;
        // }
        Value& p_entity = (Value&) current_type_p->entity.get();
        ast::ObjectType* ctype = &p_entity.type.object();
        if (*ctype != *element_type) {
            this->error_reporter.error(std::make_unique<ErrorTypeMismatch>(*element_type, node.elements[i], p_entity));
        }
        list_elements.push_back(std::move(current_type_p->snode));
    }
    node.type = element_type->clone();
    USemanticInfo return_info_p = std::make_unique<SemanticInfo>();
    auto& return_info = *return_info_p;
    return_info.is_constant = is_constant;

    return_info.snode = std::make_unique<sem::List>(std::move(list_elements));
    auto* otype = new ast::ObjectType("List", {element_type->clone()});
    auto p_value = std::make_unique<Value>(otype);
    otype->data.actual_base_path = Path("core.core.List");
    this->fill_value(*p_value);
    return_info.set_entity(p_value.release());
    return return_info_p;
}