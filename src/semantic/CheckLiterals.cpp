//
// Created by chris on 2/5/21.
//

#include "CheckLiterals.h"
#include "../simple_nodes/NoneSNode.h"
#include "../simple_nodes/DictSNode.h"

USemanticInfo Checker::visit_boolean(BooleanNode& node) {
    SemanticInfo info;
    ObjectType* ot = new T_BOOL;
    ot->actual_base_path = Path("core.Boolean");
    info.entity = Entity(new Value(ot));
    info.snode = new BoolSNode(node.value);
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::visit_number(NumberNode& node) {
    SemanticInfo info;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            ObjectType* otype = new ObjectType("Integer", {});
            Value* ov = new Value(otype);
            info.entity = Entity(ov);
            IntegerSNode* snode = new IntegerSNode();
            snode->str = node.str;
            otype->actual_base_path = Path("core.Integer");
            this->fill_value(ov);
            info.snode = snode;
            break;
        }
        case NumberType::FLOAT: {
            ObjectType* otype = new ObjectType("Float", {});
            Value* ov = new Value(otype);
            info.entity = Entity(ov);
            FloatSNode* snode = new FloatSNode();
            snode->str = node.str;
            otype->actual_base_path = Path("core.Float");
            this->fill_value(ov);
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
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_none(NoneNode& node) {
    SemanticInfo info;
    // info.set_type(ObjectType("NoneType"));
    Value* v = new Value(new ObjectType("NoneType"));
    info.entity = Entity(v);
    info.snode = new NoneSNode();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptylist(EmptyListNode& node) {
    SemanticInfo info;
    this->module->fill_actual(node.type);
    ObjectType* otype = new ObjectType("List", {node.type});
    Value* ov = new Value(otype);
    info.entity = Entity(ov);
    otype->actual_base_path = Path("core.List");
    this->fill_value(ov);
    ListSNode* lsn = new ListSNode();
    info.snode = lsn;
    lsn->elements = {};
    // non->class_name = "core.List";
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_string(StringNode& node) {
    SemanticInfo info;
    info.is_constant = true;
    StringSNode* sn = new StringSNode();
    sn->s = node.str;
    info.snode = sn;
    ObjectType* otype = new ObjectType("String", {});
    otype->actual_base_path = Path("core.String");
    Value* ov = new Value(otype);
    info.entity = Entity(ov);
    this->fill_value(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_tuple(TupleNode& node) {
    VectorOfTypes types;
    std::vector<SNode*> values;
    for (auto n: node.values) {
        USemanticInfo vtype = this->dispatch(n);
        values.push_back(vtype->snode);
        types.emplace_back(vtype->entity.value->type->clone());
        // if (!this->is_immutable(vtype->type())) {
        //     this->error_reporter.tuple_member_not_immutable(vtype->type(), node.start);
        //     return error_stub();
        // }
    }
    SemanticInfo sinfo;
    ObjectType* otype = new ObjectType("Tuple", types);
    Value* ov = new Value(otype);
    ov->metatype = Meta::CLASS;
    sinfo.entity = Entity(ov);

    ov->clazz = new Class();
    for (int i = 0; i < ov->type->object().type_params.size(); i++) {
        Value* tv = new Value(ov->type->object().type_params[i]->clone());
        this->fill_value(tv);
        const std::string& mem_name = std::to_string(i + 1);
        ov->clazz->member_entities[mem_name] = Entity(tv);
        ov->clazz->members[mem_name] = tv->type->clone();
    }
    unsigned long num_values = node.values.size();
    otype->actual_base_path = Path("core.Tuple" + std::to_string(num_values));
    NewObjectSNode* nosn = new NewObjectSNode();
    sinfo.snode = nosn;
    nosn->class_name = otype->actual_base_path.as_str();
    nosn->args = values;
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit_float(FloatNode& node) {
    SemanticInfo s;
    Value* value = new Value(new ObjectType("Float", {}));
    value->type->object().actual_base_path = Path("core.Float");
    s.entity = Entity(value);
    this->fill_value(value);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_partial(PartialApplication& node) {
    USemanticInfo func = this->dispatch(node.function);
    VectorOfTypes partial_args;
    FunctionType* fun_type = nullptr;
    if (func->entity.type == E_TYPE::CONST_FUNCTION) {
        fun_type = func->entity.const_function->ft->clone();
    } else if (func->entity.type == E_TYPE::VALUE && func->entity.value->type->kind == Kind::FUNCTION) {
        fun_type = func->entity.const_function->ft->clone();
    } else {
        this->error_reporter.fail("Error: expected a function for partial application");
        return error_stub();
    }
    if (node.args.size() != fun_type->param_types.size()) {
        this->error_reporter.partial_wrong_num_args(node.start);
        return error_stub();
    }
    std::vector<SNode*> snodes;
    int npartial = 0;
    for (size_t i = 0; i < node.args.size(); i++) {
        TypeNode*& param_type = fun_type->param_types[i];
        if (node.args[i] != nullptr) {
            USemanticInfo arg = this->expect_type(*param_type, *node.args[i]);
            if (arg->entity.type == E_TYPE::ERROR) {
                return error_stub();
            }
            snodes.push_back(arg->snode);
        } else {
            partial_args.push_back(param_type->clone());
            snodes.push_back(nullptr);
            npartial++;
        }
    }
    node.complete_type = &fun_type->clone()->function();
    SemanticInfo s;
    s.entity = Entity(new Value(new FunctionType(partial_args, fun_type->return_type->clone())));
    NewObjectSNode* non = new NewObjectSNode();
    non->class_name = "Partial" + std::to_string(npartial);
    non->args = snodes;
    non->args.insert(non->args.begin(), func->snode);
    s.snode = non;
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_dict(DictNode& node) {
    SemanticInfo info;
    USemanticInfo first_key_info = this->dispatch(node.items[0].first);
    USemanticInfo first_value_info = this->dispatch(node.items[0].second);
    ObjectType& first_key_type = first_key_info->entity.value->type->object();
    ObjectType& first_value_type = first_value_info->entity.value->type->object();

    std::vector<std::pair<SNode*, SNode*>> items;
    items = {{first_key_info->snode, first_value_info->snode}};
    bool has_error = false;
    for (size_t i = 1; i < node.items.size(); i++) {
        USemanticInfo key_info = this->expect_type(first_key_type, *node.items[i].first);
        if (key_info->entity.type == E_TYPE::ERROR) {
            has_error = true;
        }
        USemanticInfo value_info = this->expect_type(first_value_type, *node.items[i].second);
        if (value_info->entity.type == E_TYPE::ERROR) {
            has_error = true;
        }
        items.push_back(std::make_pair(key_info->snode, value_info->snode));
    }
    if (has_error) {
        return error_stub();
    }
    Value* ov = new Value(new ObjectType("Dict", {first_key_type.clone(), first_value_type.clone()}));
    this->module->fill_actual(ov->type);
    this->fill_value(ov);
    assert(ov->clazz != nullptr);
    info.entity = Entity(ov);
    DictSNode* nsn = new DictSNode(items);
    info.snode = nsn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptydict(EmptyDictNode& node) {
    SemanticInfo info;
    Value* ov = new Value(new ObjectType("Dict", {node.key_type, node.value_type}));
    info.entity = Entity(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_defconst(DefaultConstructorNode& node) {
    // this is a regular function
    SemanticInfo info;
    VectorOfTypes t;
    Entity entity = this->dispatch(node.class_node)->entity;
    if (entity.type != E_TYPE::CLASS) {
        this->error_reporter.fail("Error not a class");
    }
    Class* cls = entity.clazz;
    for (auto pt: cls->member_types) {
        t.push_back(pt->clone());
    }
    info.entity = Entity(new ConstFunction());
    VectorOfTypes tp;
    for (auto tt: entity.clazz->type_params) {
        ObjectType* ot = new ObjectType(tt);
        tp.push_back(ot);
        ot->is_generic_param = true;
    }
    auto rt = new ObjectType(entity.clazz->class_name, tp);
    rt->actual_base_path = cls->path;
    info.entity.const_function->ft = new FunctionType(t, rt);
    IdSNode* idn = new IdSNode();
    idn->identifier = cls->path.as_str() + "." + "__init__";
    info.snode = idn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_list(ListNode& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    if (element_type_p->entity.type != E_TYPE::VALUE) {
        this->error_reporter.expected_expression(element_type_p->entity, *node.elements[0]);
        return error_stub();
    }

    TypeNode* element_type = element_type_p->entity.value->type->clone();
    bool is_constant = true;
    ListSNode* lsn = new ListSNode();
    lsn->elements.push_back(element_type_p->snode);

    for (size_t i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
        // const TypeNode& current_type = current_type_p->type();
        // if (!current_type_p->is_constant) {
        //     is_constant = false;
        // }
        ObjectType* ctype = &current_type_p->entity.value->type->object();
        if (*ctype != *element_type) {
            this->error_reporter.list_literal(*element_type, *ctype, node.elements[i]->start, *node.elements[i]);
        }
        lsn->elements.push_back(current_type_p->snode);
    }
    node.type = element_type->clone();
    SemanticInfo return_info;
    return_info.is_constant = is_constant;

    return_info.snode = lsn;
    ObjectType* otype = new ObjectType("List", {element_type->clone()});
    return_info.entity = Entity(new Value(otype));
    otype->actual_base_path = Path("core.List");
    this->fill_value(return_info.entity.value);
    return std::make_unique<SemanticInfo>(return_info);
}