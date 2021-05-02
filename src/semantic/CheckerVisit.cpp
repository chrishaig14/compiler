//
// Created by chris on 17/1/21.
//

#include "Checker.h"
#include "util.h"
#include "../logger/Logger.h"
#include "../simple_nodes/BlockSNode.h"
#include "../simple_nodes/AssignmentSNode.h"
#include "../simple_nodes/ReturnSNode.h"
#include "../simple_nodes/ContinueSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/DeclarationSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/StringSNode.h"
#include "../units/FunctionValue.h"
#include "../simple_nodes/BoolSNode.h"
#include "../simple_nodes/FloatSNode.h"
#include "../simple_nodes/ClassSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/WhileSNode.h"
#include "../simple_nodes/BreakSNode.h"
#include "../simple_nodes/ListSNode.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/MatchSNode.h"
#include "CheckerUtils.h"

#define T_NONE ObjectType(".None")

SNode* Checker::make_for_snode(ForNode& node, USemanticInfo& binfo, USemanticInfo& exp_info_p) {
    BlockSNode* bbn = new BlockSNode();
    DeclarationSNode* dsn = new DeclarationSNode();
    WhileSNode* wsn = new WhileSNode();


    dsn->identifier = this->loop_list_var_id;
    dsn->expression = exp_info_p->snode;
    bbn->nodes.push_back(dsn);
    DeclarationSNode* lidx_decl = new DeclarationSNode();
    lidx_decl->identifier = this->loop_index_var_id;
    IntegerSNode* init_idx = new IntegerSNode();
    init_idx->str = "0";
    lidx_decl->expression = init_idx;

    bbn->nodes.push_back(lidx_decl);

    DeclarationSNode* lensn = new DeclarationSNode();
    lensn->identifier = this->loop_list_len_var_id;
    CallSNode* call_list_len_sn = new CallSNode();
    IdSNode* list_len_fn = new IdSNode("core.List.len");
    call_list_len_sn->function = list_len_fn;
    IdSNode* list_sn = new IdSNode(this->loop_list_var_id);
    call_list_len_sn->arguments = {list_sn};
    lensn->expression = call_list_len_sn;
    bbn->nodes.push_back(lensn);


    IdSNode* idxsn = new IdSNode(this->loop_index_var_id);
    CallSNode* cn = new CallSNode();
    IdSNode* cmpfunsn = new IdSNode("core.Integer.lt");

    IdSNode* llensn = new IdSNode(this->loop_list_len_var_id);


    cn->function = cmpfunsn;
    cn->arguments = {idxsn, llensn};

    wsn->condition = cn;

    bbn->nodes.push_back(wsn);


    BlockSNode* bn = (BlockSNode*) (binfo->snode);
    DeclarationSNode* loop_elem_sn = new DeclarationSNode();

    CallSNode* list_subscript_n = new CallSNode();
    list_subscript_n->function = new IdSNode("core.List.__sub__");
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_list_var_id));
    list_subscript_n->arguments.push_back(new IdSNode(this->loop_index_var_id));


    loop_elem_sn->expression = list_subscript_n;
    loop_elem_sn->identifier = node.var;
    bn->nodes.insert(bn->nodes.begin(), loop_elem_sn);

    bn->nodes.push_back(this->update_loop_index_snode);
    wsn->body = bn;
    return bbn;
}

USemanticInfo Checker::visit_list(ListNode& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    TypeNode* element_type = element_type_p->entity.object_value->ot->clone();
    node.elements[0] = this->replace_if_necessary(node.elements[0]);
    bool is_constant = true;
    ListSNode* lsn = new ListSNode();
    lsn->elements.push_back(element_type_p->snode);

    for (size_t i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
        // const TypeNode& current_type = current_type_p->type();
        // if (!current_type_p->is_constant) {
        //     is_constant = false;
        // }
        node.elements[i] = this->replace_if_necessary(node.elements[i]);
        ObjectType* ctype = current_type_p->entity.object_value->ot;
        if (*ctype != *element_type) {
            this->error_reporter.list_literal(*element_type, *ctype, node.elements[i]->start);
        }
        lsn->elements.push_back(current_type_p->snode);
    }
    node.type = element_type->clone();
    SemanticInfo return_info;
    return_info.is_constant = is_constant;

    return_info.snode = lsn;;
    return_info.entity = Entity(new ObjectValue());
    return_info.entity.object_value->ot = new ObjectType("List", {element_type->clone()});
    return_info.entity.object_value->ot->actual_base_path = Path("core.List");
    return std::make_unique<SemanticInfo>(return_info);
}

USemanticInfo Checker::visit_boolean(BooleanNode& node) {
    SemanticInfo info;
    info.entity = Entity(new ObjectValue());
    ObjectType* ot = new T_BOOL;
    ot->actual_base_path = Path("core.Boolean");
    info.entity.object_value->ot = ot;
    info.snode = new BoolSNode(node.value);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_while(WhileNode& node) {
    WhileSNode* while_sn = new WhileSNode();
    SemanticInfo info;
    info.snode = while_sn;
    USemanticInfo condition_p = this->dispatch(node.condition);
    SemanticInfo& condition = *condition_p;
    if (*condition.entity.object_value->ot != T_BOOL) {
        this->error_reporter.condition(*condition.entity.object_value->ot, node.start, "elif");
    }
    this->enter_scope("while");
    this->scope->is_loop = true;
    USemanticInfo body_info_p = this->visit_block(*node.body);
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
    while_sn->condition = condition.snode;
    while_sn->body = (BlockSNode*) body_info_p->snode;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_number(NumberNode& node) {
    SemanticInfo info;
    switch (node.num_type) {
        case NumberType::INTEGER: {
            ObjectValue* ov = new ObjectValue();
            info.entity = Entity(ov);
            ov->ot = new ObjectType("Integer", {});
            IntegerSNode* snode = new IntegerSNode();
            snode->str = node.str;
            ov->ot->actual_base_path = Path("core.Integer");
            info.snode = snode;
            break;
        }
        case NumberType::FLOAT: {
            ObjectValue* ov = new ObjectValue();
            info.entity = Entity(ov);
            ov->ot = new ObjectType("Float", {});
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

USemanticInfo Checker::visit_string(StringNode& node) {
    SemanticInfo info;
    info.is_constant = true;
    StringSNode* sn = new StringSNode();
    sn->s = node.str;
    info.snode = sn;
    ObjectValue* ov = new ObjectValue;
    info.entity = Entity(ov);
    ov->ot = new ObjectType("String", {});
    ov->ot->actual_base_path = Path("core.String");
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_subscript(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    Entity entity_parent = parent_p->entity;
    if (entity_parent.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Error subscript of something that is not an object!");
    }
    Entity class_entity = this->scope->get(entity_parent.object_value->ot->id);
    if (class_entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("Error this should be a CLASS, but it's not!");
    }
    Class* cls = class_entity.clazz;

    if (cls->type_params.size() != 0) {
        cls = instantiate_generic(cls, *entity_parent.object_value->ot);
    }

    auto subscript_it = cls->methods.find("__sub__");
    if (subscript_it == cls->methods.end()) {
        throw std::runtime_error("Error class " + cls->class_name + " does not define the __sub__ operator!");
    }
    ConstFunction* subscript_fun = subscript_it->second;
    std::string sub_fun_path = subscript_fun->path.as_str();
    TypeNode* rtype = subscript_fun->ft->return_type->clone();

    // SemanticInfo& parent = *parent_p;
    // if (parent.type().kind != Kind::OBJECT) {
    //     this->error_reporter.subscript_non_object(node.start);
    //     return error_stub();
    // }
    // const ObjectType& object_type = parent.type().object();

    // if (this->is_lvalue && object_type == T_STRING) {
    //     this->error_reporter.string_immutable(node.start);
    //     return error_stub();
    // }

    VectorOfTypes children;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    Node* c = node.child[0];
    USemanticInfo ct = this->dispatch(c);
    Entity child_entity = ct->entity;
    if (child_entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Error using something that's not an object as a subscript!");
    }
    if (*child_entity.object_value->ot != *subscript_fun->ft->param_types[0]) {
        throw std::runtime_error(
                "Error subscript type is " + child_entity.object_value->ot->to_string() + " but should be " +
                subscript_fun->ft->param_types[0]->to_string());
    }
    this->is_lvalue = old_lvalue;
    SemanticInfo info;
    info.entity = Entity(new ObjectValue());
    info.entity.object_value->ot = (ObjectType*) rtype;
    CallSNode* csn = new CallSNode();
    IdSNode* fsn = new IdSNode();
    fsn->identifier = sub_fun_path;
    csn->function = fsn;
    csn->arguments.push_back(parent_p->snode);
    csn->arguments.push_back(ct->snode);
    info.snode = csn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_break(BreakNode& node) {
    // node.loop_vars = this->scope->get_all_in_loop();
    SemanticInfo info;
    info.snode = new BreakSNode();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_ternary(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto& expression_type = *expression_info.entity.object_value->ot;

    if (expression_type.id != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    // TypeNode& type = *expression_type.type_params[0];
    // semanticInfo.set_type(type);
    this->enter_scope("true_case");
    // this->scope->set("it", type);
    USemanticInfo true_case_p = this->dispatch(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    node.true_case = this->replace_if_necessary(node.true_case);
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch(node.false_case);
    SemanticInfo& false_case = *false_case_p;
    node.false_case = this->replace_if_necessary(node.false_case);
    if (*false_case.entity.object_value->ot != *true_case.entity.object_value->ot) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.entity.object_value->ot->to_string() +
                " != " + false_case.entity.object_value->ot->to_string());
    } else {
        // semanticInfo.set_type(true_case.type());
    }
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit_none(NoneNode& node) {
    SemanticInfo info;
    // info.set_type(ObjectType("NoneType"));
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptylist(EmptyListNode& node) {
    SemanticInfo info;
    this->module->fill_actual(node.type);
    ObjectValue* ov = new ObjectValue();
    info.entity = Entity(ov);
    ov->ot = new ObjectType("List", {node.type});
    ov->ot->actual_base_path = Path("core.List");
    // NewObjectSNode* non = new NewObjectSNode();
    ListSNode* lsn = new ListSNode();
    info.snode = lsn;
    lsn->elements = {};
    // non->class_name = "core.List";
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_for(ForNode& node) {
    USemanticInfo exp_info_p = this->dispatch(node.exp);
    if (exp_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("iterating over something that's not an object");
    }
    ObjectType* exp_ot = exp_info_p->entity.object_value->ot;
    if (exp_ot->id != "List") {
        throw std::runtime_error("iterating over something that's not an object");
    }

    TypeNode* elem_type = exp_ot->type_params[0];
    Entity elem_entity = entity_from_type(*elem_type);
    this->enter_scope("for");
    this->scope->set(node.var, elem_entity);


    this->loop_list_var_id = "__loop_list__";
    this->loop_index_var_id = "__loop_index__";
    this->loop_list_len_var_id = "__loop_list_len__";

    AssignmentSNode* increment_index_sn = new AssignmentSNode();
    this->update_loop_index_snode = increment_index_sn;
    increment_index_sn->lvalue = new IdSNode(this->loop_index_var_id);
    CallSNode* inc_exp_node = new CallSNode();
    inc_exp_node->function = new IdSNode("core.Integer.add");
    inc_exp_node->arguments.push_back(new IdSNode(this->loop_index_var_id));
    IntegerSNode* one_node = new IntegerSNode();
    one_node->str = "1";
    inc_exp_node->arguments.push_back(one_node);
    increment_index_sn->rvalue = inc_exp_node;


    USemanticInfo binfo = this->visit_block(*node.body);
    this->leave_scope();

    SemanticInfo rinfo;


    rinfo.snode = make_for_snode(node, binfo, exp_info_p);
    this->update_loop_index_snode = nullptr;
    return std::make_unique<SemanticInfo>(rinfo);
}

USemanticInfo Checker::visit_enum(EnumNode& node) {
    SemanticInfo info;
    EnumSNode* esn = new EnumSNode();
    Enum* enumm = this->scope->get(node.id).enumm;
    esn->id = enumm->path.as_str();
    esn->values = node.values;
    info.snode = esn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_class(ClassNode& node) {
    this->error_reporter.current_class = node.class_name;
    SemanticInfo info;
    BlockSNode* sn = new BlockSNode();
    info.snode = sn;
    this->current_class = node.class_name;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }

    VectorOfTypes members_ordered_types;
    inits = std::map<std::string, bool>();

    ClassSNode* csn = new ClassSNode();

    Class* clazz = this->scope->get(node.class_name).clazz;
    csn->identifier = clazz->path.as_str();
    sn->nodes.push_back(csn);
    sn->nodes.push_back(make_class_default_init(clazz->path.as_str(), node.members_ordered));

    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        inits[mt] = false;
        members_ordered_types.push_back(&t);
        this->assert_type_exists(t, node.start);
        csn->members.push_back(mt);
    }

    for (auto sm: node.static_members) {
        USemanticInfo sm_exp_info = this->dispatch(sm.second.second);
        if (*sm.second.first != *sm_exp_info->entity.object_value->ot) {
            throw std::runtime_error("Err: cannt initialize static member of type " + sm.second.first->to_string() +
                                     " with expression of type " + sm_exp_info->entity.object_value->ot->to_string());
        }
        if (!sm_exp_info->is_constant) {
            throw std::runtime_error("Error: cannot initialize static member with non constant expression!");
        }
    }

    std::vector<SNode*> methods_snodes;
    std::vector<SNode*> static_methods_snodes;

    this->this_type = new ObjectType(node.class_name, tp);
    for (auto method: node.methods) {
        this->is_method = true;
        this->add_this = true;
        this->this_entity = Entity(new ObjectValue());
        this->this_entity.object_value->ot = new ObjectType(node.class_name);
        this->this_entity.object_value->ot->actual_base_path = clazz->path;
        // method.second->path = clazz->path + "." + method.second->identifier;
        USemanticInfo method_info = this->visit_function(*method.second);
        methods_snodes.push_back(method_info->snode);
    }

    for (auto method: node.static_methods) {
        this->is_method = false;
        this->add_this = false;
        USemanticInfo method_info = this->visit_function(*method.second);
        static_methods_snodes.push_back(method_info->snode);
    }

    for (auto m: methods_snodes) {
        sn->nodes.push_back(m);
    }
    for (auto m: static_methods_snodes) {
        sn->nodes.push_back(m);
    }

    this->add_this = true;
    std::string eq_method_name = "eq";

    this->is_method = false;
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->error_reporter.current_class = "";
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_continue(ContinueNode& node) {
    SemanticInfo info;
    BlockSNode* bn = new BlockSNode();
    info.snode = bn;
    if (this->update_loop_index_snode != nullptr) {
        bn->nodes.push_back(this->update_loop_index_snode);
    }
    bn->nodes.push_back(new ContinueSNode());
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_tuple(TupleNode& node) {
    VectorOfTypes types;
    std::vector<SNode*> values;
    for (auto n: node.values) {
        USemanticInfo vtype = this->dispatch(n);
        values.push_back(vtype->snode);
        types.emplace_back(vtype->entity.object_value->ot->clone());
        // if (!this->is_immutable(vtype->type())) {
        //     this->error_reporter.tuple_member_not_immutable(vtype->type(), node.start);
        //     return error_stub();
        // }
    }
    SemanticInfo sinfo;
    ObjectValue* ov = new ObjectValue();
    sinfo.entity = Entity(ov);
    ov->ot = new ObjectType("Tuple", types);
    unsigned long num_values = node.values.size();
    ov->ot->actual_base_path = Path("core.Tuple" + std::to_string(num_values));
    NewObjectSNode* nosn = new NewObjectSNode();
    sinfo.snode = nosn;
    nosn->class_name = ov->ot->actual_base_path.as_str();
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
    }
    if (func->entity.type == E_TYPE::FUNCTION_VALUE) {
        fun_type = func->entity.const_function->ft->clone();
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
            ObjectType* arg_ot = arg->entity.object_value->ot;
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
    s.entity = Entity(new FunctionValue());
    s.entity.function_value->ft = new FunctionType(partial_args, fun_type->return_type->clone());
    NewObjectSNode* non = new NewObjectSNode();
    non->class_name = "Partial" + std::to_string(npartial);
    non->args = snodes;
    non->args.insert(non->args.begin(), func->snode);
    s.snode = non;
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_call(CallNode& n) {
    SemanticInfo retv;
    CallSNode* sn = new CallSNode();
    retv.snode = sn;
    Logger::info("Checking CallNode");
    bool old_is_call = this->is_call;
    this->is_call = true;
    USemanticInfo fun_info_p = this->dispatch(n.function);
    this->is_call = old_is_call;
    if (fun_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (fun_info_p->this_arg != nullptr) {
        sn->arguments = {fun_info_p->this_arg};
    }
    bool is_def_const = n.function->ntype == NodeType::DEF_CONST;
    bool args_are_constant = true;
    if (fun_info_p->is_error) {
        return error_stub();
    }
    SemanticInfo& fun_info = *fun_info_p;
    // bool is_a_method = false;
    // Node* object_node;
    if (fun_info.is_class_method) {
        MemberNode& member_node = n.function->member();
        if (member_node.s_child == "init") {
            n.function = this->replacement;
            this->replace_me = false;
            // const FunctionType& ftn = fun_info.type().function();
            // FunctionType& copy_ftn = ftn.clone()->function();
            // retv.set_type(*copy_ftn.clone());
            // object_node = member_node.parent;
        } else {
            // n.function = new IdNode(this->map[fun_info.class_info->class_name + "." + member_node.s_child],
            //                         POS_NONE,
            //                         POS_NONE);
            this->replace_me = false;
            // const FunctionType& ftn = fun_info.type().function();
            // FunctionType& copy_ftn = ftn.clone()->function();
            // copy_ftn.param_types.insert(copy_ftn.param_types.begin(), TYPE(fun_info.class_info->class_name, {}));
            // retv.set_type(*copy_ftn.clone());
            // object_node = member_node.parent;
        }
    } else {
        sn->function = fun_info.snode;
    }
    n.function = replace_if_necessary(n.function);
    FunctionType* function_type = nullptr;
    if (fun_info.entity.type == E_TYPE::CONST_FUNCTION) {
        function_type = fun_info.entity.const_function->ft->clone();
    } else if (fun_info.entity.type == E_TYPE::FUNCTION_VALUE) {
        function_type = fun_info.entity.function_value->ft->clone();
    } else {
        this->error_reporter.call_not_a_function(n.start);
    }
    // ok
    // const FunctionType& function_type = fun_info.type().function();
    if (n.arguments.size() != function_type->param_types.size()) {
        this->error_reporter.function_call_num_args(*function_type, n.start);
        if (!function_is_generic(*function_type)) {
            // retv.set_type(*function_type->return_type);
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
            std::cout << ("Error can't pass as argument") << std::endl;
            return error_stub();
        }
        if (arg_entity.type == E_TYPE::ERROR) {
            return error_stub();
        }

        TypeNode& arg_type = *get_entity_type(arg_entity);
        // if (!arg_type_p->is_constant) {
        //     args_are_constant = false;
        // }
        arg = this->replace_if_necessary(arg);
        arg_types.push_back(arg_type.clone());
        n.arg_types.push_back(arg_type.clone());
    }

    if (function_type->is_generic()) {
        // mangle the generic types in function_type to prevent collisions
        mangle_generic_names(function_type);
        retv.entity = match_arguments_to_generic_function(*function_type, arg_types).entity;
        // for(auto x: arg_types){
        //     delete x;
        // }
    } else {
        retv.entity = entity_from_type(*function_type->return_type);
        retv.entity.object_value->ot = (ObjectType*) function_type->return_type->clone();
        for (size_t i = 0; i < n.arguments.size(); i++) {
            const TypeNode& arg_type = *arg_types[i];
            const TypeNode& param_type = *function_type->param_types[i];
            if (param_type == arg_type) {
                continue;
            }
            const std::string& arg_path_as_str = arg_type.object().actual_base_path.as_str();
            const std::string& param_path_as_str = param_type.object().actual_base_path.as_str();
            if (arg_path_as_str != param_path_as_str) {
                // if (arg_type != param_type) {
                if (param_type.kind == Kind::OBJECT && param_type.object().id == "Union") {
                    int type_index = target_union_type(param_type.object(), arg_type);
                    if (type_index == -1) {
                        this->error_reporter.function_call_type_mismatch(param_type,
                                                                         arg_type,
                                                                         n.arguments[i]->start,
                                                                         n.arguments[i]->end);
                    } else {
                        int fixed_index = i + (fun_info_p->this_arg != nullptr);
                        sn->arguments[fixed_index] = make_union_wrapper(type_index, sn->arguments[fixed_index]);
                    }
                    return std::make_unique<SemanticInfo>(retv);
                } else if (arg_type.kind != Kind::UNKNOWN) {
                    this->error_reporter.function_call_type_mismatch(param_type,
                                                                     arg_type,
                                                                     n.arguments[i]->start,
                                                                     n.arguments[i]->end);
                }
                // }
            }
        }
    }
    for (auto x: arg_types) {
        delete x;
    }

    retv.is_constant = is_def_const && args_are_constant;
    return std::make_unique<SemanticInfo>(retv);
}

USemanticInfo Checker::visit_root(BlockNode& node) {
    this->error_reporter.__file__ = this->__file__;
    this->error_reporter.code_lines = code_lines;

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
            for (auto bnode: n->block().nodes) {
                vn.push_back(bnode);
            }
        } else {
            vn.push_back(n);
            if (sinfo_p->snode != nullptr) {
                if (sinfo_p->snode->type == SNodeType::BLOCK) {
                    for (auto nn : ((BlockSNode*) sinfo_p->snode)->nodes) {
                        sn->nodes.push_back(nn);
                    }
                } else {
                    sn->nodes.push_back(sinfo_p->snode);
                }
            }
        }
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value

            if (!sinfo.is_error && *get_entity_type(sinfo.entity) != T_NONE) {
                this->error_reporter.unused_return_value(n->start);
            }
        }
    }
    node.nodes = vn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_function(FunctionNode& n) {
    this->error_reporter.current_function = n.identifier;
    SemanticInfo info;
    FunctionSNode* sn = new FunctionSNode();
    info.snode = sn;
    Logger::info("Checking FunctionNode " + n.identifier);
    std::string& function_name = n.identifier;
    sn->identifier = n.path.as_str();
    sn->params = n.parameter_names;
    this->current_function = function_name;
    this->enter_scope(function_name);
    this->scope->is_function = true;
    if (this->add_this) {
        this->scope->set("this", this->this_entity);
        sn->params.insert(sn->params.begin(), "this");
    }
    for (size_t i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        TypeNode* cl = type.clone();
        make_not_generic(cl);
        this->scope->set(n.parameter_names[i], entity_from_type(*cl));
        // if (!param_type.is_generic()) {
        //     if (param_type.kind == Kind::OBJECT) {
        //         ObjectType& o_type = param_type.object();
        //         Entity pt = this->scope->get(o_type.id);
        //         o_type.actual_base_path = pt.clazz->path;
        //         if (type.kind == Kind::OBJECT) {
        //             std::cout << "START" << std::endl;
        //             this->assert_type_exists(type, n.start);
        //             std::cout << "END" << std::endl;
        //         }
        //         this->scope->set(n.parameter_names[i], entity_from_type(*cl));
        //     } else {
        //         this->scope->set(n.parameter_names[i], entity_from_type(type));
        //     }
        // } else {
        //     this->scope->set(n.parameter_names[i], entity_from_type(param_type));
        // }
    }
    std::cout << "FINISH " << std::endl;

    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
    this->scope->set("__return__", entity_from_type(returnType));
    USemanticInfo body_info = this->visit_block(*n.body);
    sn->body = static_cast<BlockSNode*>(body_info->snode);

    if (returnType != T_NONE) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes.back();
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                this->error_reporter.function_return_last_stmt(function_name, returnType, last_node->start);
            }
        } else {
            this->error_reporter.function_return_last_stmt(function_name, returnType, n.start);
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
    if (entity.type == E_TYPE::NOT_FOUND) {
        this->error_reporter.variable_not_declared(n._id, n.start);
        this->scope->set(n._id, Entity(E_TYPE::ERROR));
        return error_stub();
    }
    sn->identifier = n._id;
    if (entity.type == E_TYPE::CONST_FUNCTION) {
        sn->identifier = entity.const_function->path.as_str();
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

    TypeNode* orig_type = n.type;

    if (n.type->kind == Kind::OBJECT && this->module->aliased_types.count(n.type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(n.type->object().id);
        n.type = aliased_type;
    } else {
        this->module->fill_actual(n.type);
    }

    USemanticInfo exp_info_p = this->dispatch(n.expression);
    sn->expression = exp_info_p->snode;
    SemanticInfo& exp_info = *exp_info_p;
    if (exp_info.is_error) {
        ObjectValue* ov = new ObjectValue();
        info.entity = Entity(ov);
        ov->ot = (ObjectType*) n.type->clone();
        return std::make_unique<SemanticInfo>(info);
    }
    n.expression = this->replace_if_necessary(n.expression);
    if (n.type->kind == Kind::FUNCTION) {
        // it's a function
        // if (*n.type != exp_info.type()) {
        //     this->error_reporter.assignment(*n.type, exp_info.type(), n.start);
        // }
    } else {
        SemanticInfo expression_info = exp_info;
        const ObjectType& actual_type = n.type->object();
        const TypeNode& exp_type = *expression_info.entity.object_value->ot;
        if (*n.type != exp_type) {
            if (actual_type.id == "Option") {
                if (*actual_type.type_params[0] != exp_type) {
                    auto foo = exp_type.object();
                    if (foo.id != "NoneType") {
                        this->error_reporter.assignment(*n.type, exp_type, n.start);
                    }
                }
            } else if (actual_type.id == "Union") {
                int type_index = target_union_type(actual_type, exp_type);
                if (type_index == -1) {
                    this->error_reporter.assignment(*orig_type, exp_type, n.start);
                }
                SNode* union_wrapper = make_union_wrapper(type_index, sn->expression);
                sn->expression = union_wrapper;

            } else {
                this->error_reporter.assignment(*n.type, exp_type, n.start);
            }
        }
    }
    // info.set_type(n_type);
    ObjectValue* ov = new ObjectValue();
    info.entity = Entity(ov);
    ov->ot = (ObjectType*) n.type->clone();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::check_declaration_without_type(DeclarationNode& n) {
    USemanticInfo exp_info_p = this->dispatch(n.expression);
    if (exp_info_p->entity.type == E_TYPE::ERROR) {
        std::cout << "Ignoring all subsequenct error involving variable " + n.identifier + " as type cannot be inferred"
                  << std::endl;

    }
    SemanticInfo info;
    DeclarationSNode* sn = new DeclarationSNode();
    info.snode = sn;
    sn->identifier = n.identifier;
    sn->expression = exp_info_p->snode;
    // if (exp_info_p->type() == T_NONE) {
    //     this->error_reporter.function_doesnt_return_a_value(n.expression->start, nullptr);
    //     USemanticInfo error_t = error_stub();
    //     this->scope->set(n.identifier, entity_from_type(error_t->type()));
    //     return error_t;
    // }
    // n.type = exp_info_p->type().clone();
    n.expression = this->replace_if_necessary(n.expression);
    // info.set_type(exp_info.type());
    info.entity = exp_info_p->entity;
    if (info.entity.type == E_TYPE::CONST_FUNCTION) {
        info.entity = Entity(new FunctionValue());
        info.entity.function_value->ft = exp_info_p->entity.const_function->ft;

        if (info.entity.function_value->ft->is_generic()) {
            throw std::runtime_error("Error: you need to specialize the generic function of type " +
                                     info.entity.function_value->ft->to_string() +
                                     " to be able to use it without calling it");
        }
    }
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_declaration(DeclarationNode& n) {
    Logger::info("Checking DeclarationNode for var: " + n.identifier);
    if (this->scope->declared(n.identifier)) {
        this->error_reporter.redeclared(n.identifier, n.start);
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

USemanticInfo Checker::visit_assignment(AssignmentNode& n) {

    SemanticInfo info;

    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            USemanticInfo rv = this->dispatch(n.rvalue);
            info.snode = rv->snode;
            return std::make_unique<SemanticInfo>(info);
        }
    }

    this->is_lvalue = true;
    USemanticInfo linfo_p = this->dispatch(n.lvalue);
    this->is_lvalue = false;

    if (linfo_p->is_error) {
        return nullptr;
    }

    if (linfo_p->entity.type != E_TYPE::FUNCTION_VALUE && linfo_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Cannot assign to this thing!");
    }

    if (n.lvalue->ntype == NodeType::MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_reporter.tuple_assign(n.start);
    }

    USemanticInfo expression_info_p = this->dispatch(n.rvalue);

    if (linfo_p->entity.type == E_TYPE::OBJECT_VALUE && expression_info_p->entity.type == E_TYPE::CONST_FUNCTION) {
        this->error_reporter.assignment(*linfo_p->entity.function_value->ft,
                                        *expression_info_p->entity.object_value->ot,
                                        n.rvalue->start);
    }
    TypeNode* exp_type = expression_info_p->entity.object_value->ot;

    if (exp_type->kind == Kind::OBJECT && this->module->aliased_types.count(exp_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(exp_type->object().id);
        exp_type = aliased_type;
    } else {
        this->module->fill_actual(exp_type);
    }

    if (expression_info_p->is_error) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;
    n.rvalue = this->replace_if_necessary(n.rvalue);

    const TypeNode& l_type = *linfo.entity.object_value->ot;

    if (linfo.entity.type == E_TYPE::OBJECT_VALUE && expression_info_p->entity.type == E_TYPE::OBJECT_VALUE) {

        std::string lpath_as_str = linfo.entity.object_value->ot->actual_base_path.as_str();
        std::string exppath_as_str = expression_info_p->entity.object_value->ot->actual_base_path.as_str();

        if (lpath_as_str != exppath_as_str) {
            if (l_type.kind == Kind::OBJECT) {
                const ObjectType& actual_type = l_type.object();
                if (actual_type.id == "Option") {
                    // if type doesn't match exactly, we may be assigning to an Option[t]
                    if (*actual_type.type_params[0] != *exp_type) {
                        auto& foo = exp_type->object();
                        if (foo.id != "NoneType") {
                            this->error_reporter.assignment(l_type, *exp_type, n.start);
                        }
                    }
                } else {
                    if (actual_type.kind == Kind::OBJECT && actual_type.object().id == "Union") {
                        int type_index = target_union_type(actual_type.object(), *exp_type);
                        if (type_index == -1) {
                            this->error_reporter.assignment(actual_type, *exp_type, n.rvalue->start);
                        } else {
                            expression_info_p->snode = make_union_wrapper(type_index, expression_info_p->snode);
                        }
                    } else {
                        // if it's not Option[t], then it's an error
                        this->error_reporter.assignment(l_type, *exp_type, n.start);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_reporter.assignment(l_type, *exp_type, n.start);
            }
        }
        // else, type matches don't do anything
        n.type = l_type.clone();
    } else {
        this->error_reporter.assignment(*linfo_p->entity.function_value->ft,
                                        *expression_info_p->entity.object_value->ot,
                                        n.rvalue->start);
    }
    AssignmentSNode* sn = new AssignmentSNode();
    sn->lvalue = linfo_p->snode;
    sn->rvalue = expression_info_p->snode;

    info.snode = sn;
    return std::make_unique<SemanticInfo>(info);
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
    // s.set_type(*final_type.type_params[n.n_child - 1]);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit_member(MemberNode& n) {
    USemanticInfo parent_info = this->dispatch(n.parent);
    Entity parent_entity = parent_info->entity;
    if (n.type == MemberType::NUM) {
        ObjectType* ot = parent_entity.object_value->ot;
        if (ot->id != "Tuple") {
            throw std::runtime_error("Integer member of not a Tuple!");
        }
        size_t tuple_size = ot->type_params.size();
        if (n.n_child > tuple_size || n.n_child == 0) {
            throw std::runtime_error("Tuple member out of range, has " + std::to_string(tuple_size) + " but required " +
                                     std::to_string(n.n_child));
        }
        SemanticInfo info;
        ObjectMemberSNode* omsn = new ObjectMemberSNode();
        info.snode = omsn;
        omsn->object = parent_info->snode;
        omsn->member_name = "mem_" + std::to_string(n.n_child);
        omsn->class_path = ot->actual_base_path;
        ObjectValue* ov = new ObjectValue();
        info.entity = Entity(ov);
        ov->ot = (ObjectType*) ot->type_params[n.n_child - 1]->clone();
        return std::make_unique<SemanticInfo>(info);
    }
    switch (parent_entity.type) {
        case E_TYPE::CLASS:
            return this->class_member(parent_entity.clazz, n.s_child, n);
        case E_TYPE::CONST_FUNCTION:
            this->error_reporter.function_no_member(n.dot_pos);
            break;
        case E_TYPE::FUNCTION_VALUE:
            this->error_reporter.function_no_member(n.dot_pos);
            break;
        case E_TYPE::OBJECT_VALUE:
            return this->object_member(parent_info->snode, parent_entity.object_value, n.s_child, n);
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

USemanticInfo Checker::visit_cast(CastNode& n) {
    USemanticInfo exp_info = this->dispatch(n.exp);
    SemanticInfo info;
    ObjectType cast_type(n.as_type, {});
    const TypeNode& exp_type = *exp_info->entity.object_value->ot;
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
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_if(IfNode& n) {
    SemanticInfo info;
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    if (condition_info.entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("If condition should be a Boolean");
    }
    if (*condition_info.entity.object_value->ot != T_BOOL) {
        throw std::runtime_error("If condition should be a Boolean");
    }

    // std::unordered_map<std::string, bool> not_null_vars;
    // if (condition_info.type() == T_NONE) {
    //     this->error_reporter.function_doesnt_return_a_value(n.condition->start, new T_BOOL);
    // } else if (condition_info.type() != T_BOOL) {
    //     this->error_reporter.condition(condition_info.type(), n.start, "if");
    // }

    this->enter_scope("if");
    USemanticInfo body_info = this->visit_block(*n.then);
    // for (auto v: this->scope->table) {
    //     n.then->local_vars.push_back(std::make_pair(v.first, v.second->clone()));
    // }
    this->leave_scope();

    std::vector<std::pair<SNode*, BlockSNode*>> elifs;

    for (size_t i = 0; i < n.elifs.size(); i++) {
        USemanticInfo elif_condition_info_p = this->dispatch(n.elifs[i].first);
        SemanticInfo& elif_condition_info = *elif_condition_info_p;
        if (elif_condition_info.entity.type != E_TYPE::OBJECT_VALUE) {
            throw std::runtime_error("If condition should be a Boolean");
        }
        if (*elif_condition_info.entity.object_value->ot != T_BOOL) {
            throw std::runtime_error("If condition should be a Boolean");
        }
        this->enter_scope("elif");
        USemanticInfo elif_block_info = this->visit_block(*n.elifs[i].second);
        this->leave_scope();
        elifs.push_back(std::make_pair(elif_condition_info.snode, (BlockSNode*) elif_block_info->snode));
    }
    USemanticInfo else_info;
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        else_info = this->visit_block(*n.selse);
        this->leave_scope();
    }
    SNode* else_snode = else_info == nullptr ? nullptr : else_info->snode;
    info.snode = make_if_snode(condition_info.snode, body_info->snode, elifs, else_snode);

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_boolop(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);
    SemanticInfo info;
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::OBJECT_VALUE || right_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }

    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        throw std::runtime_error("Binary operation between values of different types: " + ltype.to_string() + " and " +
                                 rtype.to_string());
    }

    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        return error_stub();
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
        return error_stub();
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;


    if (left_info.is_error || right_info.is_error) {
        return error_stub();
    }

    if (left_info_p->is_constant && right_info_p->is_constant) {
        info.is_constant = true;
    }
    std::string fun = map_boolop_to_method_name(n.op);

    Entity entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS && entity.type != E_TYPE::ENUM) {
        throw std::runtime_error("This should be a CLASS/ENUM, but it's not!");
    }
    if (entity.type == E_TYPE::ENUM) {
        if (fun != "eq" && fun != "ne") {
            throw std::runtime_error("Error: enum type doesnt support this operator");
        }
        ObjectType* ot = new ObjectType("Boolean", {});
        ot->actual_base_path = Path("core.Boolean");
        TypeNode* rettype = ot;
        info.entity = Entity(new ObjectValue());
        info.entity.object_value->ot = (ObjectType*) rettype;

        ConstFunction* opfun = entity.enumm->functions[fun];

        info.snode = make_boolop_snode(opfun, left_info, right_info);

        // IdSNode* function_id = new IdSNode(opfun->path.as_str());
        // CallSNode* sn = new CallSNode();
        // sn->function = function_id;
        // sn->arguments = {left_info.snode, right_info.snode};
        //
        // info.snode = sn;
    } else {
        Class* cls = entity.clazz;
        auto operator_fun_it = cls->static_methods.find(fun);
        if (operator_fun_it == cls->static_methods.end()) {
            throw std::runtime_error("Class " + cls->class_name + " has no operator " + fun + " defined ");
        }

        ConstFunction* operator_fun = operator_fun_it->second;
        TypeNode* rettype = operator_fun->ft->return_type->clone();
        info.entity = Entity(new ObjectValue());
        info.entity.object_value->ot = (ObjectType*) rettype;

        info.snode = make_boolop_snode(operator_fun, left_info, right_info);
    }

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_binop(BinopNode& n) {

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
    if (left_info_p->entity.type == E_TYPE::ERROR || right_info_p->entity.type == E_TYPE::ERROR) {
        return error_stub();
    }
    if (left_info_p->entity.type != E_TYPE::OBJECT_VALUE || right_info_p->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Can't have binop between 2 non objects!");
    }
    const TypeNode& ltype = *get_entity_type(left_info_p->entity);
    const TypeNode& rtype = *get_entity_type(right_info_p->entity);
    if (ltype != rtype) {
        this->error_reporter.binop(ltype, rtype, n.start);
        return error_stub();
        // throw std::runtime_error("Binary operation between values of different types: " + ltype.to_string() + " and " +
        //                          rtype.to_string());
    }
    bool err = false;
    if (ltype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.left->start, nullptr);
        err = true;
    }

    if (rtype == T_NONE) {
        this->error_reporter.function_doesnt_return_a_value(n.right->start, nullptr);
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
    std::string fun = binoptype_to_str(n.op);

    Entity entity = this->scope->get(ltype.object().id);
    if (entity.type != E_TYPE::CLASS) {
        throw std::runtime_error("This should be a CLASS, but it's not!");
    }
    Class* cls = entity.clazz;
    auto operator_fun_it = cls->static_methods.find(fun);
    if (operator_fun_it == cls->static_methods.end()) {
        throw std::runtime_error("Class " + cls->class_name + " has no operator " + fun + " defined ");
    }
    ConstFunction* operator_fun = operator_fun_it->second;
    function_id->identifier = operator_fun->path.as_str();
    rettype = operator_fun->ft->return_type->clone();
    // n.ltype = left.clone();
    info.entity = Entity(new ObjectValue());
    info.entity.object_value->ot = (ObjectType*) rettype;

    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_return(ReturnNode& n) {
    SemanticInfo info;
    ReturnSNode* sn = new ReturnSNode();
    info.snode = sn;
    Entity return_entity = this->scope->get("__return__");
    TypeNode* return_typet = nullptr;
    if (return_entity.type == E_TYPE::FUNCTION_VALUE) {
        return_typet = return_entity.function_value->ft;
    } else if (return_entity.type == E_TYPE::OBJECT_VALUE) {
        return_typet = return_entity.object_value->ot;
    }
    TypeNode* return_type = return_typet;
    if (return_type->kind == Kind::OBJECT && this->module->aliased_types.count(return_type->object().id) == 1) {
        TypeNode* aliased_type = this->module->aliased_types.at(return_type->object().id);
        return_type = aliased_type;
    } else {
        this->module->fill_actual(return_type);
    }
    if (*return_type == T_NONE) {
        if (n.expression != nullptr) {
            this->error_reporter.bad_return(n.start);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_reporter.no_return(*return_type, n.start);
    }
    USemanticInfo expression_info_p = this->dispatch(n.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.is_error) {
        return nullptr;
    }
    sn->expression = expression_info.snode;
    n.expression = this->replace_if_necessary(n.expression);
    if (return_type->object().id == "Union") {
        int type_index = target_union_type(return_type->object(), *expression_info.entity.object_value->ot);
        if (type_index == -1) {
            // throw std::runtime_error("OH NO!");
            this->error_reporter.assignment(*return_typet, *expression_info.entity.object_value->ot, n.start);
        }
        SNode* union_wrapper = make_union_wrapper(type_index, sn->expression);
        sn->expression = union_wrapper;
    } else {
        if (!this->can_assign(*expression_info.entity.object_value->ot, *return_type)) {
            this->error_reporter.return_mismatch(*return_typet, *expression_info.entity.object_value->ot, n.start);
            return error_stub();
        }
    }
    n.ret_type = return_type->clone();
    n.reachables = this->scope->get_all();
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_dict(DictNode& node) {
    SemanticInfo info;
    USemanticInfo first_key_info = this->dispatch(node.items[0].first);
    USemanticInfo first_value_info = this->dispatch(node.items[0].second);
    ObjectType& first_key_type = *first_key_info->entity.object_value->ot;
    ObjectType& first_value_type = *first_value_info->entity.object_value->ot;

    for (size_t i = 1; i < node.items.size(); i++) {
        USemanticInfo key_info = this->dispatch(node.items[i].first);
        USemanticInfo value_info = this->dispatch(node.items[i].second);
        ObjectType& key_type = *key_info->entity.object_value->ot;
        ObjectType& value_type = *value_info->entity.object_value->ot;
        if (key_type != first_key_type) {
            throw std::runtime_error("Second key type different to first");
        }
        if (value_type != first_value_type) {
            throw std::runtime_error("Second value type different to first");
        }
    }
    ObjectValue* ov = new ObjectValue();
    ov->ot = new ObjectType("Dict", {first_key_type.clone(), first_value_type.clone()});
    info.entity = Entity(ov);
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_emptydict(EmptyDictNode& node) {
    SemanticInfo info;
    ObjectValue* ov = new ObjectValue();
    ov->ot = new ObjectType("Dict", {node.key_type, node.value_type});
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
    auto rt = new ObjectType(entity.clazz->path.as_str(), tp);
    rt->actual_base_path = cls->path;
    info.entity.const_function->ft = new FunctionType(t, rt);
    IdSNode* idn = new IdSNode();
    idn->identifier = cls->path.as_str() + "." + "__init__";
    info.snode = idn;
    return std::make_unique<SemanticInfo>(info);
}

USemanticInfo Checker::visit_match(MatchExpressionNode* node) {
    SemanticInfo info;
    USemanticInfo exp_info = this->dispatch(node->exp);
    if (exp_info->entity.type != E_TYPE::OBJECT_VALUE) {
        throw std::runtime_error("Error, match expression should have type Union[...]");
    }

    ObjectType* ot = exp_info->entity.object_value->ot;
    if (ot->aliased_type != nullptr) {
        ot = (ObjectType*) ot->aliased_type;
    }
    if (ot->id != "Union") {
        throw std::runtime_error("Error, match expression should have type Union[...]");
    }
    std::vector<std::pair<int, BlockSNode*>> cas;
    std::string varname = "match_var";
    for (size_t i = 0; i < node->ids.size(); i++) {
        std::string case_id = node->ids[i];
        std::pair<TypeNode*, BlockNode*> c = node->cases[i];
        TypeNode* case_type = c.first;
        BlockNode* case_node = c.second;

        this->module->fill_actual(case_type);
        bool ok = false;
        for (auto t: ot->type_params) {
            if (t->actual_to_string() == case_type->actual_to_string()) {
                ok = true;
                break;
            }
        }
        if (!ok) {
            throw std::runtime_error("Error, type " + case_type->to_string() + " not part of " + ot->to_string());
        }
        this->enter_scope("case");
        this->scope->set(case_id, entity_from_type(*case_type));
        USemanticInfo case_info = this->dispatch(case_node);
        BlockSNode* bn = (BlockSNode*) case_info->snode;
        DeclarationSNode* dn = new DeclarationSNode();
        dn->identifier = case_id;
        ObjectMemberSNode* omn = new ObjectMemberSNode();
        dn->expression = omn;
        omn->member_name = "o";
        omn->object = new IdSNode(varname);
        omn->class_path = Path("core.Union");
        bn->nodes.insert(bn->nodes.begin(), dn);
        cas.push_back(std::make_pair(i, (BlockSNode*) case_info->snode));
        this->leave_scope();
    }
    DeclarationSNode* init = new DeclarationSNode();
    init->expression = exp_info->snode;
    init->identifier = varname;
    MatchSNode* mn = new MatchSNode(init, varname, cas);
    info.snode = mn;

    return std::make_unique<SemanticInfo>(info);
}
