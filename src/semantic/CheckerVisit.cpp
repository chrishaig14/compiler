//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "util.h"
#include "../logger/Logger.h"
#include "../simple_nodes/BlockSNode.h"
#include "../simple_nodes/AssignmentSNode.h"
#include "../simple_nodes/ReturnSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/DeclarationSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/StringSNode.h"
#include "../units/FunctionValue.h"

#define T_NONE ObjectType(".None")
static TextPosition POS_NONE = {-1, -1};

Entity entity_from_type(const TypeNode& type) {
    if (type.kind == Kind::FUNCTION) {
        FunctionValue* fv = new FunctionValue();
        fv->ft = (FunctionType*) type.clone();
        return Entity{.type=E_TYPE::FUNCTION_VALUE, .function_value=fv};
    }
    ObjectValue* fv = new ObjectValue();
    fv->ot = (ObjectType*) type.clone();
    return Entity{.type=E_TYPE::OBJECT_VALUE, .object_value=fv};
}

USemanticInfo Checker::visit(ListNode& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    const TypeNode& element_type = element_type_p->type();
    node.elements[0] = this->replace_if_necessary(node.elements[0]);
    bool is_constant = true;
    for (int i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
        const TypeNode& current_type = current_type_p->type();
        if (!current_type_p->is_constant) {
            is_constant = false;
        }
        node.elements[i] = this->replace_if_necessary(node.elements[i]);
        if (current_type != element_type) {
            this->error_list_literal(element_type, current_type, node.elements[i]->start);
        }
    }
    node.type = element_type.clone();
    SemanticInfo return_info;
    return_info.is_constant = is_constant;
    return_info.set_type(ObjectType("List", {element_type.clone()}));
    return std::make_unique<SemanticInfo>(return_info);
}

USemanticInfo Checker::visit(BooleanNode& node) {
    SemanticInfo info;
    info.set_type(T_BOOL);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(WhileNode& node) {
    USemanticInfo condition_p = this->dispatch(node.condition);
    SemanticInfo& condition = *condition_p;
    if (condition.type() != T_BOOL) {
        this->error_condition(condition.type(), node.start, "elif");
    }
    this->enter_scope("while");
    this->scope->is_loop = true;
    this->visit_block(*node.body);
    this->scope->is_loop = false;
    for (auto v: this->scope->table) {
        // if (v.second->type == E_TYPE::OBJECT_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((ObjectValue*) v.second)->ot));
        // }
        // if (v.second->type == E_TYPE::FUNCTION_VALUE) {
        //     node.body->local_vars.push_back(std::make_pair(v.first, ((FunctionValue*) v.second)->ft));
        // }
    }
    this->leave_scope();
    return nullptr;
}

USemanticInfo Checker::visit(NumberNode& node) {
    SemanticInfo info;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            ObjectValue* ov = new ObjectValue();
            info.entity = Entity{.type=E_TYPE::OBJECT_VALUE, .object_value=ov};
            ov->ot = new ObjectType("Integer", {});
            IntegerSNode* snode = new IntegerSNode();
            snode->str = node.str;
            info.snode = snode;
            break;
        }
        case NumberType::FLOAT: {
            info.set_type(T_FLOAT);
            break;
        }
        case NumberType::DOUBLE: {
            info.set_type(T_DOUBLE);
            break;
        }
    }
    info.is_constant = true;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(StringNode& node) {
    SemanticInfo info;
    info.set_type(T_STRING);
    info.is_constant = true;
    StringSNode* sn = new StringSNode();
    sn->s = node.str;
    info.snode = sn;
    ObjectValue* ov = new ObjectValue;
    info.entity = {.type=E_TYPE::OBJECT_VALUE, .object_value=ov};
    ov->ot = new ObjectType("String", {});
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    SemanticInfo& parent = *parent_p;
    if (parent.type().kind != Kind::OBJECT) {
        this->error_subscript_non_object(node.start);
        return error_stub();
    }
    const ObjectType& object_type = parent.type().object();

    if (this->is_lvalue && object_type == T_STRING) {
        this->error_string_immutable(node.start);
        return error_stub();
    }

    VectorOfTypes children;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch(c);
    bool is_integer = ct->type() == T_INT;
    children.emplace_back(ct->type().clone());
    this->is_lvalue = old_lvalue;
    node.parent_t = object_type.clone();
    SemanticInfo info;
    if (object_type.id == "List") {
        if (!is_integer) {
            this->error_subscript_type(object_type, ct->type(), T_INT, node.start);
        }
        info.set_type(*object_type.type_params[0]);
    } else if (object_type.id == "Dict") {
        TypeNode* key_type = object_type.type_params[0];
        if (ct->type() != *key_type) {
            throw std::runtime_error(
                    "Error key of dictionary must be of type " + key_type->to_string() + " but it is " +
                    ct->type().to_string());
        }
        TypeNode* value_type = object_type.type_params[1];
        info.set_type(*value_type);
    } else if (object_type.id == "String") {
        if (!is_integer) {
            this->error_subscript_type(object_type, ct->type(), T_INT, node.start);
        }
        info.set_type(object_type);
    } else {
        this->fail("Error subscript of something that's not a list, dict or string!");
        return error_stub();
    }
    if (info.type().kind == Kind::FUNCTION) {
        info.is_function = true;
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BreakNode& node) {
    node.loop_vars = this->scope->get_all_in_loop();
    return nullptr;
}

USemanticInfo Checker::visit(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto& expression_type = expression_info.type().object();

    if (expression_type.id != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    TypeNode& type = *expression_type.type_params[0];
    semanticInfo.set_type(type);
    this->enter_scope("true_case");
    // this->scope->set("it", type);
    USemanticInfo true_case_p = this->dispatch(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    node.true_case = this->replace_if_necessary(node.true_case);
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch(node.false_case);
    SemanticInfo& false_case = *false_case_p;
    node.false_case = this->replace_if_necessary(node.false_case);
    if (false_case.type() != true_case.type()) {
        throw std::runtime_error("True case and false case type don't match: " + true_case.type().to_string() + " != " +
                                 false_case.type().to_string());
    } else {
        semanticInfo.set_type(true_case.type());
    }
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(NoneNode& node) {
    SemanticInfo info;
    info.set_type(ObjectType("NoneType"));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(EmptyListNode& node) {
    SemanticInfo info;
    info.set_type(T_LIST(node.type->clone()));
    return std::make_unique<SemanticInfo>(info);
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

    for (int i = 1; i < members_ordered.size(); i++) {
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

USemanticInfo Checker::visit(ClassNode& node) {
    this->current_class = node.class_name;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;
    inits = std::map<std::string, bool>();

    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        inits[mt] = false;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
    }

    for (auto sm: node.static_members) {
        USemanticInfo sm_exp_info = this->dispatch(sm.second.second);
        if (*sm.second.first != sm_exp_info->type()) {
            throw std::runtime_error("Err: cannt initialize static member of type " + sm.second.first->to_string() +
                                     " with expression of type " + sm_exp_info->type().to_string());
        }
        if (!sm_exp_info->is_constant) {
            throw std::runtime_error("Error: cannot initialize static member with non constant expression!");
        }
    }

    this->this_type = new ObjectType(node.class_name, tp);
    bool has_init = false;
    for (auto method: node.methods) {
        this->is_method = true;
        this->visit_function(*method.second);
        has_init = has_init || method.first == "init";
    }
    for (auto method: node.static_methods) {
        this->is_method = false;
        this->add_this = false;
        this->visit_function(*method.second);
        has_init = has_init || method.first == "init";
    }
    this->add_this = true;
    std::string eq_method_name = "eq";
    // if (node.methods.find(eq_method_name) == node.methods.end()) {
    //     auto eq_meth = generate_eq_method(node.class_name, tp, node.members_ordered);
    //     node.methods[eq_method_name] = eq_meth;
    //     this->is_method = true;
    //     this->visit_function(*eq_meth);
    // } else {
    //     if (*node.methods[eq_method_name]->parameter_types[0] != *this->this_type ||
    //         *node.methods[eq_method_name]->return_type != T_BOOL) {
    //         std::string eq_method_type_string = "fun (" + this->this_type->to_string() + ") -> Boolean";
    //         throw std::runtime_error("eq method MUST be of type " + eq_method_type_string);
    //     }
    // }
    // std::string str_method_name = "str";
    // if (node.methods.find(str_method_name) == node.methods.end()) {
    //     auto str_meth = generate_str_method(node.class_name);
    //     node.methods[str_method_name] = str_meth;
    //     this->is_method = true;
    //     this->visit_function(*str_meth);
    // } else {
    //     if (node.methods[str_method_name]->parameter_types.size() != 0 ||
    //         *node.methods[str_method_name]->return_type != T_STRING) {
    //         std::string str_method_type_string = "fun () -> String";
    //         throw std::runtime_error("str method MUST be of type " + str_method_type_string);
    //     }
    // }

    // if (!has_init) {
    //     BlockNode* init_body = new BlockNode({}, POS_NONE, POS_NONE);
    //     for (auto mt: node.members_ordered) {
    //         init_body->nodes.push_back(new AssignmentNode(new MemberNode(new IdNode("this", POS_NONE, POS_NONE), mt),
    //                                                       new IdNode(mt, POS_NONE, POS_NONE),
    //                                                       POS_NONE,
    //                                                       POS_NONE));
    //     }
    //     node.methods["init"] = new FunctionNode("init",
    //                                             node.members_ordered,
    //                                             members_ordered_types,
    //                                             new ObjectType(node.class_name, tp),
    //                                             init_body,
    //                                             POS_NONE,
    //                                             POS_NONE);
    //     this->is_method = true;
    //     this->visit_function(*node.methods["init"]);
    // }

    this->is_method = false;
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->current_class = "";
    return nullptr;
}

USemanticInfo Checker::visit(ContinueNode& node) {
    return nullptr;
}

USemanticInfo Checker::visit(TupleNode& node) {
    VectorOfTypes types;
    for (auto n: node.values) {
        USemanticInfo vtype = this->dispatch(n);
        types.emplace_back(vtype->type().clone());
        if (!this->is_immutable(vtype->type())) {
            this->error_tuple_member_not_immutable(vtype->type(), node.start);
            return error_stub();
        }
    }
    ObjectType tuple_type("Tuple", types);
    SemanticInfo sinfo;
    sinfo.set_type(tuple_type);
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit(FloatNode& node) {
    SemanticInfo s;
    s.set_type(T_FLOAT);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit(PartialApplication& node) {
    USemanticInfo func = this->dispatch(node.function);
    VectorOfTypes partial_args;
    if (node.args.size() != func->type().function().param_types.size()) {
        this->error_partial_wrong_num_args(node.start);
        return error_stub();
    }
    for (int i = 0; i < node.args.size(); i++) {
        if (node.args[i] != nullptr) {
            USemanticInfo arg = this->dispatch(node.args[i]);
            if (arg->type() != *func->type().function().param_types[i]) {
                this->error_partial_function_call_type_mismatch(*func->type().function().param_types[i],
                                                                arg->type(),
                                                                node.args[i]->start,
                                                                node.args[i]->end);
                return error_stub();
            }
        } else {
            partial_args.push_back(func->type().function().param_types[i]->clone());
        }
    }
    node.complete_type = &func->type().clone()->function();
    SemanticInfo s;
    s.set_type(FunctionType(partial_args, func->type().function().return_type->clone()));
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit(ForNode& node) {
    USemanticInfo info_p = this->dispatch(node.exp);
    SemanticInfo& info = *info_p;
    if (info.type().kind != Kind::OBJECT) {
        this->error_for(info.type(), node.start);
    }

    const ObjectType& obj = info.type().object();
    if (obj.id != "List") {
        this->error_for(obj, node.start);
    }
    IdNode* lid = new IdNode("List.len", POS_NONE, POS_NONE);
    lid->is_global_function = true;
    CallNode* len_call = new CallNode(lid, {new IdNode("_list0", POS_NONE, POS_NONE)}, POS_NONE, POS_NONE);
    len_call->arg_types.push_back(obj.clone());
    Node* new_condition = new BoolOpNode(BoolOp::LT, new IdNode("_index0", POS_NONE, POS_NONE), len_call);

    BlockNode* new_body = new BlockNode({}, POS_NONE, POS_NONE);
    new_body->nodes.push_back(new DeclarationNode(node.var,
                                                  obj.type_params[0]->clone(),
                                                  new SubscriptNode(new IdNode("_list0", POS_NONE, POS_NONE),
                                                                    {new IdNode("_index0", POS_NONE, POS_NONE)},
                                                                    POS_NONE,
                                                                    POS_NONE)));
    new_body->nodes.insert(new_body->nodes.end(), node.body->nodes.begin(), node.body->nodes.end());
    AssignmentNode* asn = new AssignmentNode(new IdNode("_index0", POS_NONE, POS_NONE),
                                             new BinopNode(OpType::ADD,
                                                           new IdNode("_index0", POS_NONE, POS_NONE),
                                                           new NumberNode(NumberType::INTEGER, "1")),
                                             POS_NONE,
                                             POS_NONE);
    asn->type = new T_INT;
    new_body->nodes.push_back(asn);

    TypeNode& var_type = *obj.type_params[0];
    BlockNode* bn = new BlockNode({new DeclarationNode("_index0", new T_INT, new NumberNode(NumberType::INTEGER, "0")),
                                   new DeclarationNode("_list0", obj.clone(), node.exp),}, POS_NONE, POS_NONE);
    this->visit_block(*bn);
    this->enter_scope("for");
    // this->scope->set(node.var, var_type);

    this->scope->is_loop = true;
    this->visit_block(*node.body);
    this->scope->is_loop = false;
    this->dispatch(new_body->nodes[0]->decl().expression);
    this->dispatch(new_body->nodes[new_body->nodes.size() - 1]->assign().rvalue);
    // for (auto v: this->scope->table) {
    //     new_body->local_vars.push_back(std::make_pair(v.first, v.second->clone()));
    // }
    this->leave_scope();
    this->replacement = bn;
    this->replace_me = true;
    bn->nodes.push_back(new WhileNode(new_condition, new_body, POS_NONE, POS_NONE));
    node.body = nullptr;
    node.exp = nullptr;
    return nullptr;
}

USemanticInfo Checker::visit(MethodNode& n) {
    USemanticInfo parent = this->dispatch(n.parent);
    n.parent_t = parent->type().clone();
    if (n.parent_t->kind != Kind::OBJECT) {
        throw std::runtime_error("Cannot call a method on a function");
    }
    Class* class_info = nullptr;//this->class_table->get("asdf");
    SemanticInfo info;
    if (class_info->methods.find(n.s_child) == class_info->methods.end()) {
        throw std::runtime_error("Error " + n.parent_t->to_string() + " has no method " + n.s_child);
    }
    FunctionType* ft = class_info->methods[n.s_child];
    info.set_type(*ft);
    info.is_function = true;
    info.is_method = true;
    info.class_info = class_info;
    n.actual_function_name = class_info->class_name + "." + n.s_child;
    n.n_partial = ft->param_types.size();
    return std::make_unique<SemanticInfo>(info);
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

USemanticInfo Checker::visit_call(CallNode& n) {
    SemanticInfo retv;
    CallSNode* sn = new CallSNode();
    retv.snode = sn;
    Logger::info("Checking CallNode");
    this->is_call = true;
    USemanticInfo fun_info_p = this->dispatch(n.function);
    this->is_call = false;
    if (fun_info_p->this_arg != nullptr) {
        sn->arguments = {fun_info_p->this_arg};
    }
    bool is_def_const = n.function->ntype == NodeType::DEF_CONST;
    bool args_are_constant = true;
    if (fun_info_p->is_error) {
        return error_stub();
    }
    SemanticInfo& fun_info = *fun_info_p;
    bool is_a_method = false;
    Node* object_node;
    SNode* extra_first_argument = nullptr;
    if (fun_info.is_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)
        // extra_first_argument = fun_info.snode;
        // IdSNode* method_node = new IdSNode();
        // sn->function = method_node;
        // method_node->identifier = fun_info.method_name;
    } else if (fun_info.is_class_method) {
        MemberNode& member_node = n.function->member();
        if (member_node.s_child == "init") {
            n.function = this->replacement;
            this->replace_me = false;
            const FunctionType& ftn = fun_info.type().function();
            FunctionType& copy_ftn = ftn.clone()->function();
            retv.set_type(*copy_ftn.clone());
            object_node = member_node.parent;
        } else {
            // n.function = new IdNode(this->map[fun_info.class_info->class_name + "." + member_node.s_child],
            //                         POS_NONE,
            //                         POS_NONE);
            this->replace_me = false;
            const FunctionType& ftn = fun_info.type().function();
            FunctionType& copy_ftn = ftn.clone()->function();
            copy_ftn.param_types.insert(copy_ftn.param_types.begin(), TYPE(fun_info.class_info->class_name, {}));
            retv.set_type(*copy_ftn.clone());
            object_node = member_node.parent;
        }
    } else {
        sn->function = fun_info.snode;
    }
    n.function = replace_if_necessary(n.function);
    FunctionType* function_type = nullptr;
    if (fun_info.entity.type == E_TYPE::CONST_FUNCTION) {
        function_type = fun_info.entity.const_function->ft->clone();
    }
    if (fun_info.entity.type == E_TYPE::FUNCTION_VALUE) {
        function_type = fun_info.entity.function_value->ft->clone();
    }
    if (function_type != nullptr) {
        // ok
        // const FunctionType& function_type = fun_info.type().function();
        if (n.arguments.size() != function_type->param_types.size()) {
            this->error_function_call_num_args(n.start);
            if (!function_is_generic(*function_type)) {
                retv.set_type(*function_type->return_type);
                return std::make_unique<SemanticInfo>(retv);
            } else {
                return error_stub();
            }
        }
        VectorOfTypes arg_types;
        for (auto& arg: n.arguments) {
            USemanticInfo arg_type_p = this->dispatch(arg);
            sn->arguments.push_back(arg_type_p->snode);
            Entity arg_entity = arg_type_p->entity;
            if (arg_entity.type == E_TYPE::CLASS || arg_entity.type == E_TYPE::PACKAGE ||
                arg_entity.type == E_TYPE::MODULE) {
                throw std::runtime_error("Error can't pass as argument");
            }
            TypeNode& arg_type = *get_entity_type(arg_entity);
            // if (!arg_type_p->is_constant) {
            //     args_are_constant = false;
            // }
            arg = this->replace_if_necessary(arg);
            arg_types.push_back(arg_type.clone());
            n.arg_types.push_back(arg_type.clone());
        }

        if (function_is_generic(*function_type)) {
            retv = match_arguments_to_generic_function(*function_type, arg_types);
            // for(auto x: arg_types){
            //     delete x;
            // }
        } else {
            retv.entity = entity_from_type(*function_type->return_type);
            for (int i = 0; i < n.arguments.size(); i++) {
                const TypeNode& arg_type = *arg_types[i];
                const TypeNode& param_type = *function_type->param_types[i];
                if (arg_type != param_type) {
                    if (arg_type.kind != Kind::UNKNOWN) {
                        this->error_function_call_type_mismatch(param_type,
                                                                arg_type,
                                                                n.arguments[i]->start,
                                                                n.arguments[i]->end);
                    }
                    return std::make_unique<SemanticInfo>(retv);
                }
            }
            for (auto x: arg_types) {
                delete x;
            }
        }
    } else {
        std::string entity_type;
        switch (fun_info.entity.type) {
            case E_TYPE::CLASS:
                entity_type = "CLASS";
                break;
            case E_TYPE::CONST_FUNCTION:
                entity_type = "CONST FUNCTION";
                break;
            case E_TYPE::FUNCTION_VALUE:
                entity_type = "FUNCTION VALUE";
                break;
            case E_TYPE::OBJECT_VALUE:
                entity_type = "OBJECT VALUE";
                break;
            case E_TYPE::PACKAGE:
                entity_type = "PACKAGE";
                break;
            case E_TYPE::MODULE:
                entity_type = "MODULE";
                break;
        }
        throw std::runtime_error("Calling something that's not a function it's a " + entity_type);
        this->error_call_not_a_function(n.start);
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
    retv.is_constant = is_def_const && args_are_constant;
    return std::make_unique<SemanticInfo>(retv);
}

USemanticInfo Checker::visit_root(BlockNode& node) {

    // Initialize module level Scope
    for (auto f: this->module->flirpins) {
        this->scope->set(f.first, map_flirpin_to_entity(f.second));
    }
    // for (auto i: this->module->imports) {
    //     this->scope->set(i.first, i.second);
    // }

    USemanticInfo info = this->visit_block(node);
    this->root_snode = static_cast<BlockSNode*>(info->snode);
    SemanticInfo f;
    return std::make_unique<SemanticInfo>(f);
}


USemanticInfo Checker::visit_block(BlockNode& node) {
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    VectorOfNodes vn;
    for (auto& n: node.nodes) {
        USemanticInfo sinfo_p = this->dispatch(n);

        // sn->nodes.push_back(sinfo_p->snode);

        n = this->replace_if_necessary(n);
        if (n->ntype == NodeType::BLOCK) {
            for (auto node: n->block().nodes) {
                vn.push_back(node);
            }
        } else {
            vn.push_back(n);
            sn->nodes.push_back(sinfo_p->snode);
        }
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value

            if (!sinfo.is_error && *get_entity_type(sinfo.entity) != T_NONE) {
                this->error_unused_return_value(n->start);
            }
        }
    }
    node.nodes = vn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_function(FunctionNode& n) {
    SemanticInfo info;
    FunctionSNode* sn = new FunctionSNode();
    info.snode = sn;
    Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    sn->identifier = mangle_path(this->module->full_path + "." + n.identifier);
    sn->params = n.parameter_names;
    this->current_function = function_name;
    this->enter_scope(function_name);
    this->scope->is_function = true;
    bool is_init_method = this->is_method && function_name == "init";
    if (this->add_this) {
        // this->scope->set("this", *this->this_type);
        sn->params.insert(sn->params.begin(), "this");
    }
    for (int i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        if (type.kind == Kind::OBJECT) {
            std::cout << "START" << std::endl;
            this->assert_type_exists(type, n.start);
            std::cout << "END" << std::endl;
        }
        this->scope->set(n.parameter_names[i], entity_from_type(type));
    }
    std::cout << "FINISH " << std::endl;

    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    this->scope->set("__return__", entity_from_type(returnType));
    USemanticInfo body_info = this->visit_block(*n.body);
    sn->body = static_cast<BlockSNode*>(body_info->snode);
    // for (auto v: this->scope->table) {
    //     n.body->local_vars.push_back(std::make_pair(v.first, v.second->clone()));
    // }
    if (is_init_method) {
        for (int i = 0; i < n.body->nodes.size(); i++) {
            if (n.body->nodes[i]->ntype == NodeType::ASSIGN) {
                AssignmentNode& nod = n.body->nodes[i]->assign();
                if (nod.lvalue->ntype == NodeType::MEMBER) {
                    MemberNode& mem = nod.lvalue->member();
                    if (mem.parent->ntype == NodeType::ID) {
                        if (mem.parent->id()._id == "this") {
                            inits[mem.s_child] = true;
                        }
                    }
                }
            }
        }
        bool er = false;
        for (auto x: this->inits) {
            if (x.second == false) {
                er = true;
                this->error_class_init_member_not_init(this->current_class, x.first, n.start);
                // std::cout << "MEMBER " + x.first + " not initialized in init method!" << std::endl;
            }
        }
        if (er) {
            throw std::runtime_error("FAILED");
        }
        this->leave_scope();
        return nullptr;
    }
    if (returnType != T_NONE) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes.back();
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_function_return_last_stmt(function_name, returnType, last_node->start);
            }
        } else {
            this->error_function_return_last_stmt(function_name, returnType, n.start);
        }
    }
    this->leave_scope();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_id(IdNode& n) {
    Logger::info("Checking id node " + n._id);
    SemanticInfo info;
    IdSNode* sn = new IdSNode();
    info.snode = sn;
    Entity entity = this->scope->get(n._id);
    sn->identifier = n._id;
    if (entity.type == E_TYPE::CONST_FUNCTION) {
        sn->identifier = entity.const_function->full_path;
    }

    // if (entity == nullptr) {
    //     throw std::runtime_error("Entity with name : " + n._id + " not found!");
    // }
    info.entity = entity;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_with_type(DeclarationNode& n) {
    SemanticInfo info;
    DeclarationSNode* sn = new DeclarationSNode();
    info.snode = sn;
    sn->identifier = n.identifier;

    TypeNode& n_type = *n.type;
    if (!this->assert_type_exists(n_type, n.start)) {
        return error_stub();
    }
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    sn->expression = exp_info_p->snode;
    SemanticInfo& exp_info = *exp_info_p;
    if (exp_info.is_error) {
        this->scope->set(n.identifier, entity_from_type(n_type));
        return std::make_unique<SemanticInfo>(info);
    }
    n.expression = this->replace_if_necessary(n.expression);
    if (n.type->kind == Kind::FUNCTION) {
        // it's a function
        if (n_type != exp_info.type()) {
            this->error_assignment(n_type, exp_info.type(), n.start);
        }
    } else {
        SemanticInfo expression_info = exp_info;
        const ObjectType& actual_type = n.type->object();
        const TypeNode& exp_type = expression_info.type();
        if (actual_type.id == "Option") {
            if (*actual_type.type_params[0] != exp_type) {
                auto foo = exp_type.object();
                if (foo.id != "NoneType") {
                    this->error_assignment(n_type, exp_type, n.start);
                }
            }
        } else if (actual_type.id == "Union") {
            bool ok = false;
            for (auto type_param: actual_type.type_params) {
                if (*type_param != exp_type) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                this->error_assignment(n_type, exp_type, n.start);
            }
        } else {
            if (n_type != exp_type) {
                this->error_assignment(n_type, exp_type, n.start);
            }
        }
    }
    info.set_type(n_type);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_without_type(DeclarationNode& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    SemanticInfo info;
    DeclarationSNode* sn = new DeclarationSNode();
    info.snode = sn;
    sn->identifier = n.identifier;
    sn->expression = exp_info_p->snode;
    // if (exp_info_p->type() == T_NONE) {
    //     this->error_function_doesnt_return_a_value(n.expression->start, nullptr);
    //     USemanticInfo error_t = error_stub();
    //     this->scope->set(n.identifier, entity_from_type(error_t->type()));
    //     return error_t;
    // }
    // n.type = exp_info_p->type().clone();
    SemanticInfo& exp_info = *exp_info_p;
    n.expression = this->replace_if_necessary(n.expression);
    // info.set_type(exp_info.type());
    info.entity = exp_info_p->entity;
    return std::make_unique<SemanticInfo>(info);
}


USemanticInfo Checker::visit(DeclarationNode& n) {
    Logger::info("Checking DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier)) {
        this->error_redeclared(n.identifier, n.start);
    }
    USemanticInfo info;
    if (n.type != nullptr) {
        info = this->check_declaration_with_type(n);
    } else {
        info = this->check_declaration_without_type(n);
    }
    this->scope->set(n.identifier, info->entity);
    return info;
}

USemanticInfo Checker::visit(AssignmentNode& n) {

    SemanticInfo info;
    AssignmentSNode* sn = new AssignmentSNode();
    info.snode = sn;
    Logger::info("Checking assignment node");
    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            USemanticInfo rv = this->dispatch(n.rvalue);
            info.snode = rv->snode;
            return std::make_unique<SemanticInfo>(info);
        }
    }
    this->is_lvalue = true;
    USemanticInfo linfo_p = this->dispatch(n.lvalue);
    if (linfo_p->is_error) {
        return nullptr;
    }
    this->is_lvalue = false;
    if (n.lvalue->ntype == NodeType::MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_tuple_assign(n.start);
    }
    if (n.lvalue->ntype == NodeType::MEMBER && (linfo_p->is_class_method || n.lvalue->member().is_class_member &&
                                                                            !n.lvalue->member().is_class_static_member)) {
        throw std::runtime_error("Can only assign to static members (not methods!)");
    }
    USemanticInfo expression_type_p = this->dispatch(n.rvalue);
    if (expression_type_p->type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.rvalue->start, &linfo_p->type());
        return nullptr;
    }
    if (expression_type_p->is_error) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;
    SemanticInfo& expression_type = *expression_type_p;
    n.rvalue = this->replace_if_necessary(n.rvalue);

    const TypeNode& l_type = linfo.type();
    const TypeNode& exp_type = expression_type.type();

    if (l_type != exp_type) {
        if (l_type.kind == Kind::OBJECT) {
            const ObjectType& actual_type = l_type.object();
            if (actual_type.id == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (*actual_type.type_params[0] != exp_type) {
                    auto& foo = exp_type.object();
                    if (foo.id != "NoneType") {
                        this->error_assignment(l_type, exp_type, n.start);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_assignment(l_type, exp_type, n.start);
            }
        } else {
            // if it's not Option[t], then it's an error
            this->error_assignment(l_type, exp_type, n.start);
        }
    }
    // else, type matches don't do anything
    n.type = l_type.clone();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::member_class_method(std::string class_name, std::string child, MemberNode& n) {
    Class* class_info = nullptr;//this->class_table->get("");
    SemanticInfo rv;
    if (class_info->methods.find(child) != class_info->methods.end()) {
        // unbound method
        rv.set_type(*class_info->methods.find(child)->second);
        rv.class_info = class_info;

        const FunctionType& ftn = rv.type().function();
        FunctionType& copy_ftn = *ftn.clone();
        VectorOfTypes tp;
        for (auto tttp: rv.class_info->type_params) {
            tp.push_back(new ObjectType(tttp));
        }
        auto instance_type = new ObjectType(rv.class_info->class_name, tp);
        copy_ftn.param_types.insert(copy_ftn.param_types.begin(), instance_type);

        rv.set_type(copy_ftn);
        rv.is_method = false;
        rv.is_class_method = true;
        this->replace_me = true;
        IdNode* idn = new IdNode(class_name + "." + child, POS_NONE, POS_NONE);
        idn->is_global_function = true;
        this->replacement = idn;
        return std::make_unique<SemanticInfo>(rv);
    } else if (class_info->static_methods.find(child) != class_info->static_methods.end()) {
        // static method
        rv.set_type(*class_info->static_methods[child]);
        rv.is_function = true;
        IdNode* idn = new IdNode("", POS_NONE, POS_NONE);
        this->replace_me = true;
        this->replacement = idn;
        n.is_class_member = true;
        return std::make_unique<SemanticInfo>(rv);
    } else if (class_info->static_members.find(child) != class_info->static_members.end()) {
        rv.set_type(*class_info->static_members[child].first);
        n.is_class_static_member = true;
        n.is_class_member = true;
        return std::make_unique<SemanticInfo>(rv);
    } else {
        this->error_class_no_method(class_name, child, n.start);
        return error_stub();
    }
}

USemanticInfo Checker::member_tuple(const ObjectType& final_type, MemberNode& n) {
    if (n.type != MemberType::NUM) {
        throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                 std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
    }
    if (n.n_child < 1 || n.n_child > final_type.type_params.size()) {
        throw std::runtime_error("Error can only access members " + std::to_string(1) + " to " +
                                 std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
    }
    SemanticInfo s;
    s.set_type(*final_type.type_params[n.n_child - 1]);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::member_normal(Class* class_info, std::string child, SNode* object_snode) {
    SemanticInfo rv;
    if (class_info->members.find(child) != class_info->members.end()) {
        // It's a member
        rv.set_type(*class_info->members[child]);
    } else if (class_info->methods.find(child) != class_info->methods.end()) {
        rv.set_type(*class_info->methods[child]);
        rv.is_function = true;
        rv.snode = object_snode;
        rv.is_method = true;
        // rv.method_name = class_info->methods_paths[child];
        // this->error_method_not_member(object, child, n.start);
        // return error_stub();
    } else {
        throw std::runtime_error("Error member not found in object of class " + class_info->class_name);
        return error_stub();
    }
    return std::make_unique<SemanticInfo>(rv);
}


USemanticInfo Checker::visit_member(MemberNode& n) {
    USemanticInfo parent_info = this->dispatch(n.parent);
    Entity parent_entity = parent_info->entity;
    switch (parent_entity.type) {
        case E_TYPE::CLASS:
            return this->class_member(parent_entity.clazz, n.s_child);
            break;
        case E_TYPE::CONST_FUNCTION:
            throw std::runtime_error("Error: trying to get member of const function!");
        case E_TYPE::FUNCTION_VALUE:
            throw std::runtime_error("Error: trying to get member of function value!");
        case E_TYPE::OBJECT_VALUE:
            return this->object_member(parent_info->snode, parent_entity.object_value, n.s_child);
            break;
        case E_TYPE::PACKAGE:
            return this->package_member(parent_entity.package, n.s_child);
            break;
        case E_TYPE::MODULE:
            return this->module_member(parent_entity.module, n.s_child);
            break;
    }
}

USemanticInfo Checker::visit(CastNode& n) {
    USemanticInfo exp_info = this->dispatch(n.exp);
    SemanticInfo info;
    ObjectType cast_type(n.as_type, {});
    const TypeNode& exp_type = exp_info->type();
    if (exp_type == T_INT || exp_type == T_FLOAT || exp_type == T_DOUBLE || exp_type == T_BOOL) {
        if (cast_type != T_BOOL && cast_type != T_FLOAT && cast_type != T_DOUBLE && cast_type != T_INT) {
            throw std::runtime_error("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
        }
        if (exp_type == cast_type) {
            throw std::runtime_error("Casting to same type " + cast_type.to_string());
        }
    } else {
        throw std::runtime_error("Can't cast " + exp_type.to_string() + " to " + cast_type.to_string());
    }
    n.exp_type = exp_type.clone();
    info.set_type(cast_type);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(IfNode& n) {
    SemanticInfo info;
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    std::unordered_map<std::string, bool> not_null_vars;
    if (condition_info.type() == T_NONE) {
        this->error_function_doesnt_return_a_value(n.condition->start, new T_BOOL);
    } else if (condition_info.type() != T_BOOL) {
        this->error_condition(condition_info.type(), n.start, "if");
    }

    this->enter_scope("if");
    this->visit_block(*n.then);
    // for (auto v: this->scope->table) {
    //     n.then->local_vars.push_back(std::make_pair(v.first, v.second->clone()));
    // }
    this->leave_scope();

    for (int i = 0; i < n.elifs.size(); i++) {
        condition_info_p = this->dispatch(n.elifs[i].first);
        SemanticInfo& condition_info = *condition_info_p;
        if (condition_info.type() != T_BOOL) {
            this->error_condition(condition_info.type(), n.start, "elif");
        }
        this->enter_scope("elif");
        this->visit_block(*n.elifs[i].second);
        this->leave_scope();
    }
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        this->visit_block(*n.selse);
        this->leave_scope();
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    SemanticInfo info;
    bool ok = false;
    if (left_info.entity.type != E_TYPE::OBJECT_VALUE || right_info.entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }
    // const TypeNode& l_type = left_info.type();
    // const TypeNode& r_type = right_info.type();
    const TypeNode& l_type = *get_entity_type(left_info.entity);
    const TypeNode& r_type = *get_entity_type(right_info.entity);
    if (l_type.kind == Kind::OBJECT) {
        auto& left = l_type.object();
        if (r_type.kind == Kind::OBJECT) {
            auto& right = r_type.object();
            if (left.id == "Option" && right.id == "NoneType") {
                info.set_type(T_BOOL);
                ok = true;
            }
        }
    }
    if (!ok && l_type != r_type) {
        this->error_bool_op(l_type, r_type, n.start);
    }

    info.set_type(T_BOOL);
    n.ltype = l_type.clone();
    n.rtype = r_type.clone();

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(BinopNode& n) {

    CallSNode* sn = new CallSNode();
    IdSNode* function_id = new IdSNode();
    sn->function = function_id;
    function_id->identifier = "";//mangle_path(this->module->imported_paths.at("Integer.add"));
    SemanticInfo info;
    info.snode = sn;

    Logger::info("Checking binop node");
    USemanticInfo left_info_p = this->dispatch(n.left);
    Node* left_replace = this->replace_if_necessary(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);
    if (left_info_p->entity.type != E_TYPE::OBJECT_VALUE || right_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }
    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    bool err = false;
    if (ltype == T_NONE) {
        this->error_function_doesnt_return_a_value(n.left->start, nullptr);
        err = true;
    }

    if (rtype == T_NONE) {
        this->error_function_doesnt_return_a_value(n.right->start, nullptr);
        err = true;
    }
    if (err) {
        return error_stub();
    }
    n.left = left_replace;
    // n.left = this->replace_if_necessary(n.left);
    n.right = this->replace_if_necessary(n.right);

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    sn->arguments.push_back(left_info.snode);
    sn->arguments.push_back(right_info.snode);

    if (left_info.is_error || right_info.is_error) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    TypeNode* rettype;
    bool ok = true;
    if (ltype.object().id == "Integer" && rtype.object().id == "Integer") {
        function_id->identifier = "core_D_Integer_D_add";
        rettype = new T_INT;
    } else if (ltype.object().id == "Float" && rtype.object().id == "Float") {
        rettype = new ObjectType("Float");
    } else if (ltype.object().id == "Double" && rtype.object().id == "Double") {
        rettype = new ObjectType("Double");

    } else if (ltype.object().id == "String" && rtype.object().id == "String") {
        if (n.op == OpType::ADD) {
            rettype = new T_STRING;
            IdNode* idn = new IdNode("String_add", POS_NONE, POS_NONE);
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}), POS_NONE, POS_NONE);
        } else {
            ok = false;
        }
    } else if (ltype.object().id == "List" && ltype == rtype) {
        if (n.op == OpType::ADD) {
            rettype = ltype.clone();
            IdNode* idn = new IdNode("List_add", POS_NONE, POS_NONE);
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}), POS_NONE, POS_NONE);
        } else {
            ok = false;
        }
    } else {
        ok = false;
    }

    if (!ok) {
        this->error_binop(ltype, rtype, n.op_pos);
        return error_stub();
    }
    // n.ltype = left.clone();
    info.entity = Entity{.type=E_TYPE::OBJECT_VALUE, .object_value=new ObjectValue()};
    info.entity.object_value->ot = (ObjectType*) rettype;

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_return(ReturnNode& n) {
    SemanticInfo info;
    ReturnSNode* sn = new ReturnSNode();
    info.snode = sn;
    Entity return_entity = this->scope->get("__return__");
    TypeNode* return_typet;
    if (return_entity.type == E_TYPE::FUNCTION_VALUE) {
        return_typet = return_entity.function_value->ft;
    }
    if (return_entity.type == E_TYPE::OBJECT_VALUE) {
        return_typet = return_entity.object_value->ot;
    }
    TypeNode& return_type = *return_typet;
    if (return_type == T_NONE) {
        if (n.expression != nullptr) {
            this->error_bad_return(n.start);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_no_return(return_type, n.start);
    }
    USemanticInfo expression_info_p = this->dispatch(n.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.is_error) {
        return nullptr;
    }
    sn->expression = expression_info.snode;
    n.expression = this->replace_if_necessary(n.expression);
    // if (!this->can_assign(expression_info.type(), return_type)) {
    //     this->error_return_mismatch(return_type, expression_info.type(), n.start);
    //     return error_stub();
    // }
    n.ret_type = return_type.clone();
    n.reachables = this->scope->get_all();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(DictNode& node) {
    SemanticInfo info;
    USemanticInfo first_key_type = this->dispatch(node.items[0].first);
    USemanticInfo first_value_type = this->dispatch(node.items[0].second);

    for (int i = 1; i < node.items.size(); i++) {
        USemanticInfo key_type = this->dispatch(node.items[i].first);
        USemanticInfo value_type = this->dispatch(node.items[i].second);
        if (key_type->type() != first_key_type->type()) {
            throw std::runtime_error("Second key type different to first");
        }
        if (value_type->type() != first_value_type->type()) {
            throw std::runtime_error("Second value type different to first");
        }
    }
    info.set_type(ObjectType("Dict", {first_key_type->type().clone(), first_value_type->type().clone()}));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(EmptyDictNode& node) {
    SemanticInfo info;
    info.set_type(ObjectType("Dict", {node.key_type, node.value_type}));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit(DefaultConstructorNode& node) {
    // this is a regular function
    SemanticInfo info;
    VectorOfTypes t;
    Entity entity = this->scope->get(node.name);
    // if (entity == nullptr) {
    //     throw std::runtime_error("Error: " + node.name + " not defined");
    // }
    if (entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("Error: " + node.name + " is not a class");
    }
    Class* cls = entity.clazz;
    for (auto pt: cls->member_types) {
        t.push_back(pt->clone());
    }
    info.entity = Entity{.type=E_TYPE::CONST_FUNCTION};
    info.entity.const_function = new ConstFunction();
    info.entity.const_function->ft = new FunctionType(t, new ObjectType(node.name, {}));
    info.is_function = true;
    return std::make_unique<SemanticInfo>(info);
}

