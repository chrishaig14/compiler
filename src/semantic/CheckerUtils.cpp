//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"

std::string binoptype_to_str(OpType op) {
    std::map<OpType, std::string> funs;
    funs[OpType::ADD] = "add";
    funs[OpType::SUB] = "sub";
    funs[OpType::MUL] = "mul";
    funs[OpType::DIV] = "div";
    funs[OpType::MOD] = "mod";

    return "__" + funs.at(op) + "__";
}


int target_union_type(const ObjectType& target, const TypeNode& source) {
    for (size_t ti = 0; ti < target.type_params.size(); ti++) {
        if (target.type_params[ti]->actual_to_string() == source.actual_to_string()) {
            return ti;
        }
    }
    return -1;
}

Entity entity_from_type(const TypeNode& type) {
    if (type.kind == Kind::FUNCTION) {
        auto* fv = new Value(type.clone());
        return Entity(fv);
    }
    if (type.kind == Kind::OBJECT) {
        if (type.object().id == ".None") {
            return Entity(E_TYPE::NOTHING);
        }
    }
    auto* fv = new Value(type.clone());
    return Entity(fv);
}

FunctionSNode* make_class_default_init(const std::string& class_path, const VectorOfStrings& members) {
    auto* bn = new BlockSNode();
    auto* nn = new NewObjectSNode();
    nn->class_name = class_path;
    for (const auto& m: members) {
        auto* idn = new IdSNode(m);
        nn->args.push_back(idn);
    }
    auto* rn = new ReturnSNode(nn);
    bn->nodes.push_back(rn);
    auto* fn = new FunctionSNode(class_path + ".__init__", members, bn);
    return fn;
}


TypeNode* get_entity_type(Entity e) {
    Entity* ent = &e;
    if (ent->type == E_TYPE::CONST_FUNCTION) {
        return e.const_function->ft->clone();
    } else if (ent->type == E_TYPE::VALUE) {
        return e.value->type->clone();
    }
    throw std::runtime_error("Get type of non function/object!");
}


void mangle_generic_names(TypeNode* t) {
    if (t->kind == Kind::OBJECT) {
        return mangle_generic_names(&t->object());
    }
    return mangle_generic_names(&t->function());
}

void mangle_generic_names(FunctionType* t) {
    for (auto* pt: t->param_types) {
        mangle_generic_names(pt);
    }
    mangle_generic_names(t->return_type);
}

void mangle_generic_names(ObjectType* t) {
    if (t->is_generic_param) {
        t->id = t->id + "0";
    } else {
        for (auto* tp: t->type_params) {
            mangle_generic_names(tp);
        }
    }
}


void make_not_generic(TypeNode* t) {
    if (t->kind == Kind::FUNCTION) {
        return make_not_generic(&t->function());
    }
    return make_not_generic(&t->object());
}

void make_not_generic(FunctionType* ft) {
    for (auto* pt: ft->param_types) {
        make_not_generic(pt);
    }
    make_not_generic(ft->return_type);
}

void make_not_generic(ObjectType* ot) {
    ot->is_generic_param = false;
    for (auto* tp: ot->type_params) {
        make_not_generic(tp);
    }
}

std::string map_boolop_to_method_name(BoolOp op) {
    std::map<BoolOp, std::string> funs;

    funs[BoolOp::EQ] = "eq";
    funs[BoolOp::NE] = "ne";

    funs[BoolOp::AND] = "and";
    funs[BoolOp::OR] = "or";

    funs[BoolOp::LE] = "le";
    funs[BoolOp::GE] = "ge";
    funs[BoolOp::LT] = "lt";
    funs[BoolOp::GT] = "gt";

    return "__" + funs[op] + "__";
}


SNode* make_union_wrapper(int type_index, SNode* expression) {
    auto* new_union = new NewObjectSNode();
    new_union->class_name = "core_D_Union";
    auto* in = new IntegerSNode(std::string());
    in->str = std::to_string(type_index); // FIXME, use int directly
    new_union->args = {expression, in};
    return new_union;
}


SNode* make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info) {
    auto* function_id = new IdSNode(operator_fun->path.as_str());
    auto* sn = new CallSNode(function_id, {left_info.snode, right_info.snode});
    return sn;
}

