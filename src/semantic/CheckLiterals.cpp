//
// Created by chris on 2/5/21.
//

#include "CheckLiterals.h"

USemanticInfo Checker::visit_boolean(BooleanNode& node) {
    SemanticInfo info;
    info.entity = Entity(new Value());
    ObjectType* ot = new T_BOOL;
    ot->actual_base_path = Path("core.Boolean");
    info.entity.value->type = ot;
    info.snode = new BoolSNode(node.value);
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::visit_number(NumberNode& node) {
    SemanticInfo info;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            Value* ov = new Value();
            info.entity = Entity(ov);
            ov->type = new ObjectType("Integer", {});
            IntegerSNode* snode = new IntegerSNode();
            snode->str = node.str;
            ov->type->object().actual_base_path = Path("core.Integer");
            info.snode = snode;
            break;
        }
        case NumberType::FLOAT: {
            Value* ov = new Value();
            info.entity = Entity(ov);
            ov->type = new ObjectType("Float", {});
            FloatSNode* snode = new FloatSNode();
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
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_none(NoneNode& node) {
    SemanticInfo info;
    // info.set_type(ObjectType("NoneType"));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptylist(EmptyListNode& node) {
    SemanticInfo info;
    this->module->fill_actual(node.type);
    Value* ov = new Value();
    info.entity = Entity(ov);
    ov->type = new ObjectType("List", {node.type});
    ov->type->object().actual_base_path = Path("core.List");
    // NewObjectSNode* non = new NewObjectSNode();
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
    Value* ov = new Value;
    info.entity = Entity(ov);
    ov->type = new ObjectType("String", {});
    ov->type->object().actual_base_path = Path("core.String");
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
    Value* ov = new Value();
    sinfo.entity = Entity(ov);
    ov->type = new ObjectType("Tuple", types);
    unsigned long num_values = node.values.size();
    ov->type->object().actual_base_path = Path("core.Tuple" + std::to_string(num_values));
    NewObjectSNode* nosn = new NewObjectSNode();
    sinfo.snode = nosn;
    nosn->class_name = ov->type->object().actual_base_path.as_str();
    nosn->args = values;
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit_float(FloatNode& node) {
    SemanticInfo s;
    // s.set_type(T_FLOAT);
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
        throw std::runtime_error("Error: expected a function for partial application");
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
            USemanticInfo arg = this->dispatch(node.args[i]);
            ObjectType* arg_ot = &arg->entity.value->type->object();
            if (*arg_ot != *param_type) {
                this->error_reporter.partial_function_call_type_mismatch(*param_type,
                                                                         *arg_ot,
                                                                         node.args[i]->start,
                                                                         node.args[i]->end);
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
    s.entity = Entity(new Value());
    s.entity.value->type = new FunctionType(partial_args, fun_type->return_type->clone());
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

    for (size_t i = 1; i < node.items.size(); i++) {
        USemanticInfo key_info = this->dispatch(node.items[i].first);
        USemanticInfo value_info = this->dispatch(node.items[i].second);
        ObjectType& key_type = key_info->entity.value->type->object();
        ObjectType& value_type = value_info->entity.value->type->object();
        if (key_type != first_key_type) {
            throw std::runtime_error("Second key type different to first");
        }
        if (value_type != first_value_type) {
            throw std::runtime_error("Second value type different to first");
        }
    }
    Value* ov = new Value();
    ov->type = new ObjectType("Dict", {first_key_type.clone(), first_value_type.clone()});
    info.entity = Entity(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptydict(EmptyDictNode& node) {
    SemanticInfo info;
    Value* ov = new Value();
    ov->type = new ObjectType("Dict", {node.key_type, node.value_type});
    info.entity = Entity(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_defconst(DefaultConstructorNode& node) {
    // this is a regular function
    SemanticInfo info;
    VectorOfTypes t;
    Entity entity = this->dispatch(node.class_node)->entity;
    if (entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("Error not a class");
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
            this->error_reporter.list_literal(*element_type, *ctype, node.elements[i]->start);
        }
        lsn->elements.push_back(current_type_p->snode);
    }
    node.type = element_type->clone();
    SemanticInfo return_info;
    return_info.is_constant = is_constant;

    return_info.snode = lsn;;
    return_info.entity = Entity(new Value());
    return_info.entity.value->type = new ObjectType("List", {element_type->clone()});
    return_info.entity.value->type->object().actual_base_path = Path("core.List");
    return std::make_unique<SemanticInfo>(return_info);
}