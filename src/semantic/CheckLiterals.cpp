//
// Created by chris on 2/5/21.
//

#include "CheckLiterals.h"
#include <simple_nodes/expressions/include/expressions.h>
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <units/entities/EntityNone.h>

UExpressionInfo ModuleChecker::visit_boolean(const ast::Boolean& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("libcore.libcore.Boolean")).clone());
    info.exp_snode = std::make_unique<sem::Bool>(node.value);
    return info_u;
}


UExpressionInfo ModuleChecker::visit_number(const ast::Number& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("libcore.libcore.Integer")).clone());
            info.exp_snode = std::make_unique<sem::Integer>(node.str);
            break;
        }
        case NumberType::FLOAT: {
            info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("libcore.libcore.Float")).clone());
            info.exp_snode = std::make_unique<sem::Float>(node.str);
            break;
        }
        case NumberType::DOUBLE: {
            break;
        }
    }
    info.is_constant = true;
    return info_u;
}

UExpressionInfo ModuleChecker::visit_none(const ast::None& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.set_entity(std::make_unique<EntityNone>());
    info.exp_snode = std::make_unique<sem::None>();
    return info_u;
}

UExpressionInfo ModuleChecker::visit_emptylist(const ast::EmptyList& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    sem::Type* nt = node.type->to_sem();
    this->module.fill_actual(*nt);
    auto* otype = new sem::TypeObject("List", {nt}, Path("libcore.libcore.List"));
    info.set_entity(this->make_value(otype));
    std::vector<sem::UExp> v;
    info.exp_snode = std::make_unique<sem::List>(std::move(v));
    return info_u;
}

UExpressionInfo ModuleChecker::visit_string(const ast::String& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    info.is_constant = true;
    info.exp_snode = std::make_unique<sem::String>(node.str);
    info.set_entity(this->entity_value_from_actual_base_path_no_generic(Path("libcore.libcore.String")).clone());
    return info_u;
}

UExpressionInfo ModuleChecker::visit_tuple(const ast::Tuple& node) {
    sem::VectorOfTypes types;
    std::vector<sem::UExp> values;
    for (auto& n: node.values) {
        UExpressionInfo vtype = this->dispatch_rvalue(*n);
        values.push_back(std::move(vtype->exp_snode));
        types.emplace_back(vtype->entity.get().get_value().type.clone());
    }
    UExpressionInfo sinfo_p = std::make_unique<ExpressionInfo>();
    auto& sinfo = *sinfo_p;
    unsigned long num_values = node.values.size();
    auto* otype = new sem::TypeObject("Tuple", types, Path("core.Tuple" + std::to_string(num_values)));


    ConcreteClass* clazz = new ConcreteClass("Tuple", Path("libcore.libcore.Tuple"));
    for (size_t i = 0; i < otype->object().type_params.size(); i++) {
        auto tv = this->make_value(otype->object().type_params[i]->clone());
        const std::string& mem_name = std::to_string(i + 1);
        clazz->attributes[mem_name] = tv->type.to_ast();
        clazz->attribute_entities[mem_name] = std::move(tv);
    }
    auto ov = std::make_unique<EntityValue>(otype, clazz);
    sinfo.set_entity(std::move(ov));
    auto nosn = std::make_unique<sem::NewObject>();
    nosn->class_name = otype->data.actual_base_path.as_str();
    nosn->args = std::move(values);
    sinfo.exp_snode = std::move(nosn);
    return sinfo_p;
}

UExpressionInfo ModuleChecker::visit_partial(const ast::PartialApplication& node) {
    UExpressionInfo func = this->dispatch_rvalue(*node.function);
    sem::VectorOfTypes partial_args;
    ast::FunctionType* fun_type = nullptr;
    Entity& f_entity = func->entity;
    if (f_entity.is_constfun() || (f_entity.is_value() && (f_entity.get_value()).type.kind == sem::Kind::FUNCTION)) {
        fun_type = (ast::FunctionType*) f_entity.get_constfun().const_function.const_function_ft.to_ast();
    } else {
        this->error_reporter.fail("Error: expected a function for partial application");
        return exp_error_stub();
    }
    if (node.args.size() != fun_type->param_types.size()) {
        this->error_reporter.error(std::make_unique<error::PartialWrongNumArgs>(node.start));
        return exp_error_stub();
    }
    std::vector<sem::UExp> snodes;
    int npartial = 0;
    for (size_t i = 0; i < node.args.size(); i++) {
        ast::UTypeNode& param_type = fun_type->param_types[i];
        if (node.args[i] != nullptr) {
            UExpressionInfo arg_sinfo = this->expect_rvalue_of_type(*param_type->to_sem(), *node.args[i]);
            if (arg_sinfo->is_error()) {
                return exp_error_stub();
            }
            auto arg_snode = std::move(arg_sinfo->exp_snode);
            snodes.push_back(std::move(arg_snode));
        } else {
            partial_args.push_back(param_type->to_sem());
            snodes.push_back(nullptr);
            npartial++;
        }
    }
    UExpressionInfo s_p = std::make_unique<ExpressionInfo>();
    auto& s = *s_p;
    s.entity = *EntityValue::function_value(new sem::TypeFunction(partial_args,
                                                                  sem::UType(fun_type->return_type->to_sem())));
    auto non = std::make_unique<sem::NewObject>();
    non->class_name = "Partial" + std::to_string(npartial);
    non->args = std::move(snodes);
    non->args.insert(non->args.begin(), std::move(func->exp_snode));
    s.exp_snode = std::move(non);
    return s_p;
}

UExpressionInfo ModuleChecker::visit_dict(const ast::DictNode& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    UExpressionInfo first_key_info = this->dispatch_rvalue(node.items[0].first);
    UExpressionInfo first_value_info = this->dispatch_rvalue(node.items[0].second);
    EntityValue& first_key_entity = first_key_info->entity.get().get_value();
    EntityValue& first_value_entity = first_value_info->entity.get().get_value();
    sem::TypeObject& first_key_type = first_key_entity.type.object();
    sem::TypeObject& first_value_type = first_value_entity.type.object();

    std::vector<std::pair<sem::UExp, sem::UExp>> items;
    items.emplace_back(std::move(first_key_info->exp_snode), std::move(first_value_info->exp_snode));
    bool has_error = false;
    for (size_t i = 1; i < node.items.size(); i++) {
        UExpressionInfo key_sinfo = this->expect_rvalue_of_type(first_key_type, node.items[i].first);
        if (key_sinfo->is_error()) {
            has_error = true;
        }
        UExpressionInfo value_sinfo = this->expect_rvalue_of_type(first_value_type, node.items[i].second);
        if (value_sinfo->is_error()) {
            has_error = true;
        }
        items.emplace_back(key_sinfo->exp_snode.release(), value_sinfo->exp_snode.release());
    }
    if (has_error) {
        return exp_error_stub();
    }
    sem::TypeObject* type = new sem::TypeObject("Dict", {first_key_type.clone(), first_value_type.clone()});
    this->module.fill_actual(*type);
    info.set_entity(this->make_value(type));
    info.exp_snode = std::make_unique<sem::Dict>(std::move(items));
    return info_u;
}

UExpressionInfo ModuleChecker::visit_emptydict(const ast::EmptyDict& node) {
    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    sem::TypeObject* ot = new sem::TypeObject("Dict",
                                              {node.key_type->to_sem(), node.value_type->to_sem()},
                                              Path("libcore.libcore.Dict"));
    this->module.fill_actual(*ot);
    info.set_entity(this->make_value(ot));
    info.exp_snode = std::make_unique<sem::Dict>(std::vector<std::pair<sem::UExp, sem::UExp>>{});
    return info_u;
}

UExpressionInfo ModuleChecker::visit_defconst(const ast::DefaultConstructor& node) {
    // this is a regular function
    UExpressionInfo class_info = this->dispatch_rvalue(*node.class_node);
    Entity& entity = class_info->entity;
    if (entity.e_type != E_TYPE::CLASS and entity.e_type != E_TYPE::TEMPLATE_CLASS) {
        this->error_reporter.fail("Error not a class");
        return exp_error_stub();
    }

    UExpressionInfo info_u = std::make_unique<ExpressionInfo>();
    ExpressionInfo& info = *info_u;
    if (entity.is_class()) {

        ConcreteClass& cls = entity.get_class().clazz;
        sem::VectorOfTypes t;
        for (auto* pt: cls.attribute_types) {
            t.push_back(pt->to_sem());
        }
        sem::VectorOfTypes tp;
        for (const auto& tt: cls.type_params) {
            auto* ot = new sem::TypeObject(tt);
            tp.push_back(ot);
            ot->is_generic_param = true;
        }
        auto* rt = new sem::TypeObject(cls.class_name, tp, cls.path);
        info.set_entity(std::make_unique<EntityConstFunction>(*new ConstFunction(Path(),
                                                                                 std::make_unique<sem::TypeFunction>(t,
                                                                                                                     sem::UType(
                                                                                                                             rt)))));
        info.exp_snode = std::make_unique<sem::ObjectConstructor>(cls.path);
    } else {
        TemplateClassInfo& tci = entity.get_template_class().clazz;
        sem::VectorOfTypes t;
        for (auto* pt: tci.member_types) {
            t.push_back(pt->to_sem());
        }
        sem::VectorOfTypes tp;
        for (const auto& tt: tci.type_params) {
            auto* ot = new sem::TypeObject(tt);
            tp.push_back(ot);
            ot->is_generic_param = true;
        }
        auto* rt = new sem::TypeObject(tci.class_name, tp, tci.path);
        info.set_entity(std::make_unique<EntityConstFunction>(*new ConstFunction(Path(),
                                                                                 std::make_unique<sem::TypeFunction>(t,
                                                                                                                     sem::UType(
                                                                                                                             rt)))));
        info.exp_snode = std::make_unique<sem::ObjectConstructor>(tci.path);
    }
    return info_u;
}

UExpressionInfo ModuleChecker::visit_list(const ast::List& node) {
    UExpressionInfo element_type_p = this->dispatch_rvalue(node.elements[0]);
    if (element_type_p->entity.get().e_type != E_TYPE::VALUE) {
        throw std::runtime_error("Expected expression");
        return exp_error_stub();
    }
    EntityValue& entity_value = element_type_p->entity.get().get_value();
    sem::Type& element_type = entity_value.type;
    bool is_constant = true;
    std::vector<sem::UExp> list_elements;
    list_elements.push_back(std::move(element_type_p->exp_snode));

    for (size_t i = 1; i < node.elements.size(); i++) {
        UExpressionInfo current_type_p = this->dispatch_rvalue(node.elements[i]);
        EntityValue& p_entity = current_type_p->entity.get().get_value();
        sem::TypeObject* ctype = &p_entity.type.object();
        if (*ctype != element_type) {
            this->error_reporter.error(std::make_unique<error::TypeMismatch>(element_type, node.elements[i], p_entity));
        }
        list_elements.push_back(std::move(current_type_p->exp_snode));
    }
    UExpressionInfo return_info_p = std::make_unique<ExpressionInfo>();
    auto& return_info = *return_info_p;
    return_info.is_constant = is_constant;

    return_info.exp_snode = std::make_unique<sem::List>(std::move(list_elements));
    auto* otype = new sem::TypeObject("List", {element_type.clone()}, Path("libcore.libcore.List"));
    return_info.set_entity(this->make_value(otype));
    return return_info_p;
}