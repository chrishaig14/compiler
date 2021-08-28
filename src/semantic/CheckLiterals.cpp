//
// Created by chris on 2/5/21.
//

#include "CheckLiterals.h"
#include "../simple_nodes/NoneSNode.h"
#include "../simple_nodes/DictSNode.h"
#include "errors/ErrorExpectedExpression.h"
#include "errors/ErrorListLiteral.h"
#include "errors/ErrorPartialWrongNumArgs.h"
#include "errors/ErrorTypeMismatch.h"

USemanticInfo Checker::visit_boolean(ast::Boolean& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.Boolean"));
    info.snode = new BoolSNode(node.value);
    return info_u;
}


USemanticInfo Checker::visit_number(ast::Number& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.Integer"));
            auto* snode = new IntegerSNode(std::string());
            snode->str = node.str;
            info.snode = snode;
            break;
        }
        case NumberType::FLOAT: {
            info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.Float"));
            // auto* otype = new ObjectType("Float", {});
            // otype->actual_base_path = Path("core.core.Float");
            // auto ov = std::make_unique<Value>(otype);
            // this->fill_value(*ov);
            // info.entity = new EntityValue(std::move(ov));
            auto* snode = new FloatSNode();
            snode->str = node.str;
            info.snode = snode;
            break;
        }
        case NumberType::DOUBLE: {
            // ObjectValue* ov = new ObjectValue();
            // info.entity = Entity{.type=E_TYPE::OBJECT_VALUE, .object_value=ov};
            // ov->ot = new ObjectType("Double", {});
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
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    // info.set_type(ObjectType("NoneType"));
    auto v = std::make_unique<Value>(new ObjectType("NoneType"));
    info.entity = *new EntityValue(std::move(v));
    info.snode = new NoneSNode();
    return info_u;
}

USemanticInfo Checker::visit_emptylist(ast::EmptyList& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    this->module.fill_actual(node.type);
    auto* otype = new ObjectType("List", {node.type});
    auto ov = std::make_unique<Value>(otype);
    otype->actual_base_path = Path("core.core.List");
    this->fill_value(*ov);
    info.entity = *new EntityValue(std::move(ov));
    std::vector<USNode> v;
    auto* lsn = new ListSNode(v);
    info.snode = lsn;
    // non->class_name = "core.List";
    return info_u;
}

USemanticInfo Checker::visit_string(ast::String& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    info.is_constant = true;
    auto* sn = new StringSNode(node.str);
    info.snode = sn;
    // auto* otype = new ObjectType("String", {});
    // otype->actual_base_path = Path("core.core.String");
    // auto ov = std::make_unique<Value>(otype);
    // this->fill_value(*ov);
    // info.entity = new EntityValue(std::move(ov));
    info.entity = this->entity_value_from_actual_base_path_no_generic(Path("core.core.String"));
    return info_u;
}

USemanticInfo Checker::visit_tuple(ast::Tuple& node) {
    VectorOfTypes types;
    std::vector<SNode*> values;
    for (auto* n: node.values) {
        USemanticInfo vtype = this->dispatch(*n);
        values.push_back(vtype->snode);
        types.emplace_back(((EntityValue&) vtype->entity).value->type->clone());
        // if (!this->is_immutable(vtype->type())) {
        //     this->error_reporter.tuple_member_not_immutable(vtype->type(), node.start);
        //     return error_stub();
        // }
    }
    SemanticInfo sinfo;
    auto* otype = new ObjectType("Tuple", types);
    auto ov = std::make_unique<Value>(otype);
    ov->metatype = Meta::CLASS;

    ov->clazz = new Class();
    for (size_t i = 0; i < ov->type->object().type_params.size(); i++) {
        auto tv = std::make_unique<Value>(ov->type->object().type_params[i]->clone());
        this->fill_value(*tv);
        const std::string& mem_name = std::to_string(i + 1);
        ov->clazz->members[mem_name] = tv->type->clone();
        ov->clazz->member_entities[mem_name] = new EntityValue(std::move(tv));
    }
    sinfo.entity = *new EntityValue(std::move(ov));
    unsigned long num_values = node.values.size();
    otype->actual_base_path = Path("core.Tuple" + std::to_string(num_values));
    auto* nosn = new NewObjectSNode();
    sinfo.snode = nosn;
    nosn->class_name = otype->actual_base_path.as_str();
    nosn->args = values;
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit_partial(ast::PartialApplication& node) {
    USemanticInfo func = this->dispatch(*node.function);
    VectorOfTypes partial_args;
    FunctionType* fun_type = nullptr;
    Entity& f_entity = func->entity;
    if (f_entity.type == E_TYPE::CONST_FUNCTION ||
        (f_entity.type == E_TYPE::VALUE && ((EntityValue&) f_entity).value->type->kind == Kind::FUNCTION)) {
        fun_type = ((EntityConstFunction&) f_entity).const_function->ft->clone();
    } else {
        this->error_reporter.fail("Error: expected a function for partial application");
        return error_stub();
    }
    if (node.args.size() != fun_type->param_types.size()) {
        this->error_reporter.error(ErrorPartialWrongNumArgs(node.start));
        return error_stub();
    }
    std::vector<SNode*> snodes;
    int npartial = 0;
    for (size_t i = 0; i < node.args.size(); i++) {
        TypeNode*& param_type = fun_type->param_types[i];
        if (node.args[i] != nullptr) {
            USemanticInfo arg_sinfo = this->expect_rvalue_of_type(*param_type, *node.args[i]);
            if (arg_sinfo->is_error()) {
                return error_stub();
            }
            SNode* arg_snode = arg_sinfo->snode;
            snodes.push_back(arg_snode);
        } else {
            partial_args.push_back(param_type->clone());
            snodes.push_back(nullptr);
            npartial++;
        }
    }
    node.complete_type = &fun_type->clone()->function();
    SemanticInfo s;
    s.entity = *new EntityValue(std::make_unique<Value>(new FunctionType(partial_args, fun_type->return_type->clone())));
    auto* non = new NewObjectSNode();
    non->class_name = "Partial" + std::to_string(npartial);
    non->args = snodes;
    non->args.insert(non->args.begin(), func->snode);
    s.snode = non;
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_dict(ast::DictNode& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    USemanticInfo first_key_info = this->dispatch(*node.items[0].first);
    USemanticInfo first_value_info = this->dispatch(*node.items[0].second);
    EntityValue& first_key_entity = (EntityValue&) first_key_info->entity.get();
    EntityValue& first_value_entity = (EntityValue&) first_value_info->entity.get();
    ObjectType& first_key_type = first_key_entity.value->type->object();
    ObjectType& first_value_type = first_value_entity.value->type->object();

    std::vector<std::pair<SNode*, SNode*>> items;
    items = {{first_key_info->snode, first_value_info->snode}};
    bool has_error = false;
    for (size_t i = 1; i < node.items.size(); i++) {
        USemanticInfo key_sinfo = this->expect_rvalue_of_type(first_key_type, *node.items[i].first);
        if (key_sinfo->is_error()) {
            has_error = true;
        }
        USemanticInfo value_sinfo = this->expect_rvalue_of_type(first_value_type, *node.items[i].second);
        if (value_sinfo->is_error()) {
            has_error = true;
        }
        items.emplace_back(key_sinfo->snode, value_sinfo->snode);
    }
    if (has_error) {
        return error_stub();
    }
    auto ov = std::make_unique<Value>(new ObjectType("Dict", {first_key_type.clone(), first_value_type.clone()}));
    this->module.fill_actual(ov->type);
    this->fill_value(*ov);
    assert(ov->clazz != nullptr);
    info.entity = *new EntityValue(std::move(ov));
    auto* nsn = new DictSNode(items);
    info.snode = nsn;
    return info_u;
}

USemanticInfo Checker::visit_emptydict(ast::EmptyDict& node) {
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    ObjectType* ot = new ObjectType("Dict", {node.key_type->clone(), node.value_type->clone()});
    ot->actual_base_path = Path("core.core.Dict");
    auto ov = std::make_unique<Value>(ot);
    this->module.fill_actual(ov->type);
    this->fill_value(*ov);
    assert(ov->clazz != nullptr);
    info.entity = *new EntityValue(std::move(ov));
    auto* nsn = new DictSNode({});
    info.snode = nsn;
    return info_u;

    return info_u;
}

USemanticInfo Checker::visit_defconst(ast::DefaultConstructor& node) {
    // this is a regular function
    USemanticInfo info_u = std::make_unique<SemanticInfo>(); SemanticInfo& info = *info_u;
    VectorOfTypes t;
    Entity& entity = this->dispatch(*node.class_node)->entity;
    if (entity.type != E_TYPE::CLASS) {
        this->error_reporter.fail("Error not a class");
        info.entity = *new EntityError();
        return info_u;
    }
    Class& cls = *((EntityClass&) entity).clazz;
    for (auto* pt: cls.member_types) {
        t.push_back(pt->clone());
    }
    VectorOfTypes tp;
    for (const auto& tt: cls.type_params) {
        auto* ot = new ObjectType(tt);
        tp.push_back(ot);
        ot->is_generic_param = true;
    }
    auto* rt = new ObjectType(cls.class_name, tp);
    rt->actual_base_path = cls.path;
    info.entity = *new EntityConstFunction(new ConstFunction(Path(), new FunctionType(t, rt)));
    auto* idn = new IdSNode(cls.path.as_str() + "." + "__init__");
    info.snode = idn;
    return info_u;
}

USemanticInfo Checker::visit_list(ast::List& node) {
    USemanticInfo element_type_p = this->dispatch(*node.elements[0]);
    if (element_type_p->entity.get().type != E_TYPE::VALUE) {
        this->error_reporter.error(ErrorExpectedExpression(element_type_p->entity, *node.elements[0]));
        return error_stub();
    }
    EntityValue& entity_value = (EntityValue&) element_type_p->entity.get();
    TypeNode* element_type = entity_value.value->type->clone();
    bool is_constant = true;
    std::vector<USNode> list_elements;
    list_elements.push_back(USNode(element_type_p->snode));

    for (size_t i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(*node.elements[i]);
        // const TypeNode& current_type = current_type_p->type();
        // if (!current_type_p->is_constant) {
        //     is_constant = false;
        // }
        EntityValue& p_entity = (EntityValue&) current_type_p->entity.get();
        ObjectType* ctype = &p_entity.value->type->object();
        if (*ctype != *element_type) {
            this->error_reporter.error(ErrorTypeMismatch(*element_type, *node.elements[i], p_entity));
        }
        list_elements.push_back(USNode(current_type_p->snode));
    }
    node.type = element_type->clone();
    SemanticInfo return_info;
    return_info.is_constant = is_constant;

    return_info.snode = new ListSNode(list_elements);
    auto* otype = new ObjectType("List", {element_type->clone()});
    auto p_value = std::make_unique<Value>(otype);
    otype->actual_base_path = Path("core.core.List");
    this->fill_value(*p_value);
    return_info.entity = *new EntityValue(std::move(p_value));
    return std::make_unique<SemanticInfo>(return_info);
}