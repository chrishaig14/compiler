//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"

static TextPosition POS_NONE = {-1, -1};


std::string binoptype_to_str(OpType op) {
    std::string fun;
    if (op == OpType::ADD) {
        fun = "add";
    } else if (op == OpType::SUB) {
        fun = "sub";
    } else if (op == OpType::MUL) {
        fun = "mul";
    } else if (op == OpType::DIV) {
        fun = "div";
    }
    return fun;
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
        FunctionValue* fv = new FunctionValue();
        fv->ft = (FunctionType*) type.clone();
        return Entity(fv);
    }
    ObjectValue* fv = new ObjectValue();
    fv->ot = (ObjectType*) type.clone();
    return Entity(fv);
}

FunctionNode* generate_eq_method(std::string class_name, VectorOfTypes tp, VectorOfStrings members_ordered) {
    auto eq_body = new BlockNode({}, POS_NONE, POS_NONE);
    std::string eq_method_name = "eq";
    auto eq_meth = new FunctionNode(eq_method_name,
                                    {"other"},
                                    {new ObjectType(class_name, tp)},
                                    new T_BOOL,
                                    eq_body,
                                    POS_NONE,
                                    POS_NONE);
    auto cmp_node = new BoolOpNode(BoolOp::EQ,
                                   new MemberNode(new IdNode("this", POS_NONE, POS_NONE), members_ordered[0]),
                                   new MemberNode(new IdNode("other", POS_NONE, POS_NONE), members_ordered[0]));

    for (size_t i = 1; i < members_ordered.size(); i++) {
        cmp_node = new BoolOpNode(BoolOp::AND,
                                  cmp_node,
                                  new BoolOpNode(BoolOp::EQ,
                                                 new MemberNode(new IdNode("this", POS_NONE, POS_NONE),
                                                                members_ordered[i]),
                                                 new MemberNode(new IdNode("other", POS_NONE, POS_NONE),
                                                                members_ordered[i])));
    }
    eq_body->nodes.push_back(new ReturnNode(cmp_node));
    return eq_meth;
}

FunctionNode* generate_str_method(std::string class_name) {
    auto eq_body = new BlockNode({}, POS_NONE, POS_NONE);
    std::string eq_method_name = "str";
    auto eq_meth = new FunctionNode(eq_method_name, {}, {}, new T_STRING, eq_body, POS_NONE, POS_NONE);
    eq_body->nodes.push_back(new ReturnNode(new StringNode("<" + class_name + " object>")));
    return eq_meth;
}

FunctionSNode* make_class_default_init(std::string class_path, VectorOfStrings members) {
    FunctionSNode* fn = new FunctionSNode();
    fn->identifier = class_path + ".__init__";
    fn->params = members;
    BlockSNode* bn = new BlockSNode();
    ReturnSNode* rn = new ReturnSNode();
    NewObjectSNode* nn = new NewObjectSNode();
    nn->class_name = class_path;
    for (auto m: members) {
        IdSNode* idn = new IdSNode();
        idn->identifier = m;
        nn->args.push_back(idn);
    }
    rn->expression = nn;
    bn->nodes.push_back(rn);
    fn->body = bn;
    return fn;
}


TypeNode* get_entity_type(Entity e) {
    Entity* ent = &e;
    if (ent->type == E_TYPE::CONST_FUNCTION) {
        return e.const_function->ft->clone();
    }
    if (ent->type == E_TYPE::FUNCTION_VALUE) {
        return e.function_value->ft->clone();
    }
    if (ent->type == E_TYPE::OBJECT_VALUE) {
        return e.object_value->ot->clone();
    }
    throw std::runtime_error("Get type of non function/object!");
}

void mangle_generic_names(TypeNode* t);
void mangle_generic_names(FunctionType* t);
void mangle_generic_names(ObjectType* t);

void mangle_generic_names(TypeNode* t) {
    if (t->kind == Kind::OBJECT) {
        return mangle_generic_names(&t->object());
    }
    return mangle_generic_names(&t->function());
}

void mangle_generic_names(FunctionType* t) {
    for (auto pt: t->param_types) {
        mangle_generic_names(pt);
    }
    mangle_generic_names(t->return_type);
}

void mangle_generic_names(ObjectType* t) {
    if (t->is_generic_param) {
        t->id = t->id + "0";
    } else {
        for (auto tp: t->type_params) {
            mangle_generic_names(tp);
        }
    }
}

void make_not_generic(FunctionType* ft);
void make_not_generic(ObjectType* ft);
void make_not_generic(TypeNode* ft);

void make_not_generic(TypeNode* t) {
    if (t->kind == Kind::FUNCTION) {
        return make_not_generic(&t->function());
    }
    return make_not_generic(&t->object());
}

void make_not_generic(FunctionType* ft) {
    for (auto pt: ft->param_types) {
        make_not_generic(pt);
    }
    make_not_generic(ft->return_type);
}

void make_not_generic(ObjectType* ot) {
    ot->is_generic_param = false;
    for (auto tp: ot->type_params) {
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

    return funs[op];
}

std::string map_binop_to_method_name(OpType op) {
    std::map<OpType, std::string> funs;

    funs[OpType::ADD] = "add";
    funs[OpType::SUB] = "sub";

    funs[OpType::MUL] = "mul";
    funs[OpType::DIV] = "div";
    funs[OpType::MOD] = "mod";

    return funs[op];
}


int target_union_type(const ObjectType& target, const TypeNode& source);

SNode* make_union_wrapper(int type_index, SNode* expression) {
    NewObjectSNode* new_union = new NewObjectSNode();
    new_union->class_name = "core_D_Union";
    IntegerSNode* in = new IntegerSNode();
    in->str = std::to_string(type_index); // FIXME, use int directly
    new_union->args = {expression, in};
    return new_union;
}

std::string binoptype_to_str(OpType op);

SNode* make_if_snode(SNode* condition, SNode* body, std::vector<std::pair<SNode*, BlockSNode*>> elifs, SNode* _else) {
    IfSNode* ifs = new IfSNode();
    ifs->condition = condition;
    ifs->then = (BlockSNode*) body;
    ifs->elifs = elifs;
    ifs->_else = (BlockSNode*) _else;
    return ifs;
}

std::string map_binop_to_method_name(OpType op);

SNode* make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info) {
    IdSNode* function_id = new IdSNode(operator_fun->path.as_str());
    CallSNode* sn = new CallSNode();
    sn->function = function_id;
    sn->arguments = {left_info.snode, right_info.snode};
    return sn;
}

std::string map_boolop_to_method_name(BoolOp op);