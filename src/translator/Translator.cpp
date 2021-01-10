//
// Created by chris on 2/8/20.
//

#include <iostream>
#include "Translator.h"
#include "../vm/LabelledCode.h"
#include "../macros.h"

void Translator::visit(AssignmentNode& node) {
    CodeLabel out;
    this->dispatch(node.rvalue);
//    node.rvalue->accept(*this)
    this->dispatch(node.rvalue);
    CodeLabel r_code = this->code;
    out.insert(out.end(), r_code.begin(), r_code.end());

    this->is_lvalue = true;
    if (node.lvalue->ntype == NodeType::ID && node.lvalue->id().identifier == "_") {
        out.push_back(LC("", I_POP));
    } else {
//        node.lvalue->accept(*this)
        this->dispatch(node.lvalue);
        CodeLabel l_code = this->code;
        out.insert(out.end(), l_code.begin(), l_code.end());
    }
    this->code = out;
}

void Translator::visit(BoolOpNode& node) {
//    node.left->accept(*this)
    this->dispatch(node.left);
    CodeLabel left_code = this->code;
//    node.right->accept(*this)
    this->dispatch(node.right);
    CodeLabel right_code = this->code;
    CodeLabel out = left_code;
    out.insert(out.end(), right_code.begin(), right_code.end());
    out.push_back(LC("", new BoolOpInst(node.op)));
    this->code = out;
}


void Translator::visit(BinopNode& node) {
//    node.left->accept(*this)
    this->dispatch(node.left);
    CodeLabel left_code = this->code;
//    node.right->accept(*this)
    this->dispatch(node.right);
    CodeLabel right_code = this->code;
    CodeLabel out = left_code;
    out.insert(out.end(), right_code.begin(), right_code.end());
    out.push_back(LC("", new BinopInst(node.op)));
    this->code = out;
}

void Translator::visit(BlockNode& node) {
    CodeLabel out;
    for (auto n: node.nodes) {
        this->code = {};
//        n->accept(*this)
        this->dispatch(n);
        CodeLabel node_code = this->code;
        out.insert(out.end(), node_code.begin(), node_code.end());
    }
    this->code = out;
}

void Translator::visit(CallNode& node) {
    CodeLabel out;
    for (auto a: node.arguments) {
//        a->accept(*this)
        this->dispatch(a);
        CodeLabel arg_code = this->code;
        out.insert(out.end(), arg_code.begin(), arg_code.end());
    }
    out.push_back(LC("", new GetInst(node.function->id().identifier, node.function->id().location)));
    out.push_back(LC("", I_CALL));
    this->code = out;
}

void Translator::visit(DeclarationNode& node) {
    CodeLabel out;
//    if (node.expression->ntype != NodeType::UNINITIALIZED) {

//    if (node.expression != nullptr) {
//        node.expression->accept(*this)
    this->dispatch(node.expression);
    out = this->code;
//    }
//    out.push_back(LC("", I_DECL(node.identifier)));
    out.push_back(LC("", new SetInst(node.identifier, VariableLocation(0, -1))));
    this->code = out;
}

void Translator::visit(FunctionNode& node) {
    CodeLabel out;
    std::string new_name = node.identifier;
    out.push_back(LC("", I_START_FUNCTION(new_name)));
    CodeLabel body_code;
//    node.body->accept(*this)
    this->visit(*node.body);
    body_code = this->code;
    CodeLabel parameters_code;
    for (int i = node.parameter_names.size() - 1; i >= 0; i--) {
        parameters_code.push_back(NL(I_DECL(node.parameter_names[i])));
        parameters_code.push_back(NL(I_SET(node.parameter_names[i])));
    }
    out.insert(out.end(), parameters_code.begin(), parameters_code.end());
    out.insert(out.end(), body_code.begin(), body_code.end());
    out.push_back(LC("", I_END_FUNCTION(node.identifier)));
    this->code = out;
}

void Translator::visit(IdNode& node) {
    CodeLabel out;
    if (this->is_lvalue) {
        this->is_lvalue = false;
        out.push_back(LC("", new SetInst(node.identifier, node.location)));
    } else {
        out.push_back(LC("", new GetInst(node.identifier, node.location)));
    }
    this->code = out;
}

void Translator::visit(IfNode& node) {
    CodeLabel out;
//    node.condition->accept(*this)
    this->dispatch(node.condition);
    CodeLabel condition_code = this->code;
    out.insert(out.end(), condition_code.begin(), condition_code.end());
//    node.then->accept(*this)
//    this->dispatch(node.then);
    this->visit(*node.then);
    CodeLabel then_code = this->code;
    bool has_else = node.selse != nullptr && !node.selse->nodes.empty();
//    bool has_else = node.selse != nullptr;

    out.push_back(LC("labelinif", I_JUMPF(then_code.size() + 3 + (has_else && node.elifs.size() == 0 ? 1 : 0))));
    out.push_back(LC("", new EnterScope("if")));
    out.insert(out.end(), then_code.begin(), then_code.end());
    out.push_back(LC("", new LeaveScope("if")));

    for (int i = 0; i < node.elifs.size(); i++) {
        CodeLabel eout;
//        node.elifs[i].first->accept(*this)
        this->dispatch(node.elifs[i].first);
        CodeLabel econdition_code = this->code;
        out.insert(out.end(), econdition_code.begin(), econdition_code.end());
//        node.elifs[i].second->accept(*this)
        this->visit(*node.elifs[i].second);
        CodeLabel ethen_code = this->code;
        out.push_back(LC("", I_JUMPF(ethen_code.size() + 3 + ((has_else && i == node.elifs.size() - 1) ? 1 : 0))));
        out.push_back(LC("", new EnterScope("elif")));
        out.insert(out.end(), ethen_code.begin(), ethen_code.end());
        out.push_back(LC("", new LeaveScope("elif")));
    }
    if (has_else) {
//        node.selse->accept(*this)
        this->visit(*node.selse);
        CodeLabel else_code = this->code;
        out.push_back(LC("", I_JUMP(else_code.size() + 3)));
        out.push_back(LC("", new EnterScope("else")));
        out.insert(out.end(), else_code.begin(), else_code.end());
        out.push_back(LC("", new LeaveScope("else")));
    }
    this->code = out;
}

void Translator::visit(ListNode& node) {
    CodeLabel out;
    for (auto e: node.elements) {
//        e->accept(*this)
        this->dispatch(e);
        CodeLabel e_code = this->code;
        out.insert(out.end(), e_code.begin(), e_code.end());
    }
    out.push_back(LC("", I_MAKE_LIST(node.elements.size())));
    this->code = out;
}

void Translator::visit(MemberNode& node) {
    CodeLabel out;
    if (this->is_lvalue) {
        this->is_lvalue = false;

//        node.parent->accept(*this)
        this->dispatch(node.parent);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_SETM(node.s_child)));
    } else {
//        node.parent->accept(*this)
        this->dispatch(node.parent);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());
        if (node.type == MemberType::STR) {
            out.push_back(LC("", I_GETM(node.s_child)));
        } else {
            // NUM
            out.push_back(LC("", new GetTupleMemberInst(node.n_child)));
        }
    }
    this->code = out;
}

void Translator::visit(NumberNode& node) {
    CodeLabel out;
    out.push_back(LC("", I_PUSHI(node.num)));
    this->code = out;
}

void Translator::visit(FloatNode& node) {
    CodeLabel out;
    out.push_back(LC("", new PushFloatInst(node.value)));
    this->code = out;
}

void Translator::visit(ReturnNode& node) {
    CodeLabel out;
    if (node.expression->ntype != NodeType::UNINITIALIZED) {
//    if (node.expression != nullptr) {
//        node.expression->accept(*this)
        this->dispatch(node.expression);
        out = this->code;
    }
    out.push_back(LC("", I_RET));
    this->code = out;
}

void Translator::visit(StringNode& node) {
    CodeLabel out;
    out.push_back(LC("", I_PUSHS(node.str)));
    this->code = out;
}

void Translator::visit(SubscriptNode& node) {
    CodeLabel out;
    if (this->is_lvalue) {
        this->is_lvalue = false;

//        node.child.(*this);
        CodeLabel child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

//        node.parent->accept(*this)
        this->dispatch(node.parent);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_SETS));
    } else {
//        node.child[0]->accept(*this)
        this->dispatch(node.child[0]);
        CodeLabel child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

//        node.parent->accept(*this)
        this->dispatch(node.parent);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_GETS));
    }
    this->code = out;
}


Translator::Translator() : is_lvalue(false) {
    this->loop_counter = 0;
    this->in_for_loop = false;
}

void Translator::visit(ClassLiteralExpressionNode& node) {
    CodeLabel all;
    std::vector<std::string> fields;
    for (int i = 0; i < node.init.size(); i++) {
        Node* exp = node.init[i];
        std::string name = node.names[i];
        this->code = {};
//        exp->accept(*this)
        this->dispatch(exp);
        fields.push_back(name);
        CodeLabel out = this->code;
        all.insert(all.end(), out.begin(), out.end());
    }
    all.push_back(LC("", new MakeObjectInst(node.type->identifier, fields)));
    this->code = all;
}

void Translator::visit(ClassLiteralFieldNode& node) {
    CodeLabel all;
    std::vector<std::string> fields;
    for (int i = 0; node.init_names.size(); i++) {
        this->code = {};
//        f.second->accept(*this)
        this->dispatch(node.init_values[i]);
        fields.push_back(node.init_names[i]);
        CodeLabel out = this->code;
        all.insert(all.end(), out.begin(), out.end());
    }
    all.push_back(LC("", new MakeObjectInst(node.type->identifier, fields)));
    this->code = all;
}

void Translator::visit(ForNode& node) {
    return;
    CodeLabel out;
    int loop_number = this->loop_counter;
    this->current_loop = loop_number;
    this->loop_counter++;

    std::string list_name = ".list" + std::to_string(this->current_loop);
    std::string len_name = ".len" + std::to_string(this->current_loop);
    std::string index_name = ".index" + std::to_string(this->current_loop);

    // list = expression
    auto list_init = new DeclarationNode(list_name, nullptr, node.exp);
//    list_init->accept(*this)
    this->dispatch(list_init);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // len = list.len()
    auto list_len = new DeclarationNode(
            len_name, nullptr,
            new CallNode(new IdNode("List.len"), {new IdNode(list_name)}));
//    list_len->accept(*this)
    this->dispatch(list_len);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // index = 0
    auto index = new DeclarationNode(index_name, nullptr, new NumberNode(0));
//    index->accept(*this)
    this->dispatch(index);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // index < list_len ?
    auto condition = new BoolOpNode(BoolOp::LT, new IdNode(index_name), new IdNode(len_name));
//    condition->accept(*this)
    this->dispatch(condition);
    auto condition_code = this->code;

    // start new iteration
    std::string start_loop_label = "start_loop." + std::to_string(loop_number);
    out.push_back(LC(start_loop_label, condition_code[0].second));
    for (int i = 1; i < condition_code.size(); i++) {
        out.push_back(condition_code[i]);
    }
    out.push_back(LC("", I_ENTER("for")));
    out.push_back(LC("", I_JUMPF("break_loop." + std::to_string(this->current_loop))));

    // var = list[index]
    auto it = new SubscriptNode(new IdNode(list_name), {new IdNode(index_name)});
    node.body->nodes.insert(node.body->nodes.begin(), new DeclarationNode(node.var, nullptr, it));

    // index = index + 1
    auto update_index = new AssignmentNode(
            new IdNode(index_name),
            new BinopNode(
                    OpType::ADD, new IdNode(index_name),
                    new NumberNode(1)));
//    update_index->accept(*this)
    this->dispatch(update_index);
    CodeLabel update_index_code = this->code;

//    out.insert(out.end(), condition_code.begin(), condition_code.end());
    bool old_in_for_loop = this->in_for_loop;
    this->in_for_loop = true;
//    node.body->accept(*this)
    this->visit(*node.body);
    this->in_for_loop = old_in_for_loop;
    out.insert(out.end(), this->code.begin(), this->code.end());
    out.push_back(LC("", I_LEAVE("for")));
    this->current_loop--;
//
//    CodeLabel body_code = this->code;
//    CodeLabel p = {LC("", I_ENTER("while"))};
//    body_code.insert(body_code.begin(), p.begin(), p.end());
//    p = {LC("", I_LEAVE("while"))};
//    body_code.insert(body_code.end(), p.begin(), p.end());
//    out.push_back(NL(I_JUMPF(body_code.size() + 3)));
//    out.insert(out.end(), body_code.begin(), body_code.end());
    out.insert(out.end(), update_index_code.begin(), update_index_code.end());
    out.push_back(LC("", I_JUMP(start_loop_label)));
    out.push_back(LC("break_loop." + std::to_string(loop_number), I_LEAVE("for")));
    this->code = out;

}

void Translator::visit(WhileNode& node) {
    CodeLabel out;
//    node.condition->accept(*this)
    this->dispatch(node.condition);
    std::vector<std::pair<std::string, Instruction*>> condition_code = this->code;
    int loop_number = this->loop_counter;
    this->current_loop = loop_number;
    this->loop_counter++;
    std::string start_loop_label = "start_loop." + std::to_string(loop_number);
    out.push_back(LC(start_loop_label, condition_code[0].second));
    for (int i = 1; i < condition_code.size(); i++) {
        out.push_back(condition_code[i]);
    }
//    node.body->accept(*this)
    this->visit(*node.body);
    this->current_loop--;
    CodeLabel body_code = this->code;
    CodeLabel p = {LC("", I_ENTER("while"))};
    body_code.insert(body_code.begin(), p.begin(), p.end());
    p = {LC("", I_LEAVE("while"))};
    body_code.insert(body_code.end(), p.begin(), p.end());
    out.push_back(NL(I_JUMPF(body_code.size() + 3)));
    out.insert(out.end(), body_code.begin(), body_code.end());
    out.push_back(LC("", I_JUMP(start_loop_label)));
    out.push_back(LC("break_loop." + std::to_string(loop_number), I_LEAVE("while")));
    this->code = out;
}

void Translator::visit(BooleanNode& node) {
    CodeLabel out;
    out.push_back(LC("", new PushBooleanInst(node.value)));
    this->code = out;
}

void Translator::visit(BreakNode& node) {
    CodeLabel out = {NL(I_JUMP("break_loop." + std::to_string(this->current_loop)))};
    this->code = out;
}

void Translator::visit(TernaryNode& node) {
    CodeLabel out = {};
//    node.expression->accept(*this)
    this->dispatch(node.expression);
    CodeLabel expression_code = this->code;
    out.insert(out.end(), expression_code.begin(), expression_code.end());
//    node.true_case->accept(*this)
    this->dispatch(node.true_case);
    CodeLabel true_case = this->code;
    out.push_back(NL(I_JUMPN(true_case.size() + 6)));
    CodeLabel it_code = {NL(I_ENTER("it_scope")), NL(I_DECL("it")), NL(I_SET("it"))};
    out.insert(out.end(), it_code.begin(), it_code.end());
    out.insert(out.end(), true_case.begin(), true_case.end());
    out.push_back(NL(I_LEAVE("it_scope")));
//    node.false_case->accept(*this)
    this->dispatch(node.false_case);
    CodeLabel false_case = this->code;
    out.push_back(NL(I_JUMP(false_case.size() + 1)));
    out.insert(out.end(), false_case.begin(), false_case.end());
    this->code = out;
}

void Translator::visit(NoneNode& node) {
    CodeLabel out = {NL(I_PUSHN)};
    this->code = out;
}

void Translator::visit(EmptyListNode& node) {
    CodeLabel out = {NL(I_MAKE_LIST(0))};
    this->code = out;
}

void Translator::visit(ClassNode& node) {
    CodeLabel out;
    std::vector<std::string> f;
    for (auto field: node.members) {
        f.push_back(field.first);
    }
    out.push_back(LC("", I_MAKE_CLASS(node.class_name, f)));

    for (auto method: node.methods) {
        FunctionNode& method_node = *method.second;
        method_node.identifier = node.class_name + "." + method_node.identifier;
        method_node.parameter_names.insert(method_node.parameter_names.begin(), "this");
        method_node.parameter_types.insert(
                method_node.parameter_types.begin(),
                new ObjectTypeNode("dummy", {}));
//        method.second->accept(*this)
        this->visit(*method.second);
        auto method_code = this->code;
        out.insert(out.end(), method_code.begin(), method_code.end());
    }
    this->code = out;
}


void Translator::visit(TupleNode& node) {
    CodeLabel out;
    for (auto e: node.values) {
//        e->accept(*this)
        this->dispatch(e);
        CodeLabel e_code = this->code;
        out.insert(out.end(), e_code.begin(), e_code.end());
    }
    out.push_back(LC("", new MakeTupleInst(node.values.size())));
    this->code = out;
}

void Translator::visit(ContinueNode& node) {
    if (this->in_for_loop) {
        std::cout << "TRANSLATING A CONTINUE NOED" << std::endl;
        CodeLabel out;
        auto update_index = new AssignmentNode(
                new IdNode(".index" + std::to_string(this->current_loop)),
                new BinopNode(
                        OpType::ADD, new IdNode(
                                ".index" + std::to_string(this->current_loop)),
                        new NumberNode(1)));
//        update_index->accept(*this)
        this->dispatch(update_index);
        out.insert(out.end(), this->code.begin(), this->code.end());
        out.push_back(LC("", I_LEAVE("for")));
        out.push_back(LC("", I_JUMP("start_loop." + std::to_string(this->current_loop))));
        this->code = out;
    } else {
        // while loop
        CodeLabel out;
        out.push_back(LC("", I_JUMP("start_loop." + std::to_string(this->current_loop))));
        this->code = out;

    }
}

void Translator::dispatch(Node* nptr) {
    Node& n = *nptr;
    switch (n.ntype) {
        case NodeType::BOOLOP:
            this->visit(n.boolop());
            break;
        case NodeType::ASSIGN:
            this->visit(n.assign());
            break;
        case NodeType::BINOP:
            this->visit(n.binop());
            break;
        case NodeType::BLOCK:
            this->visit(n.block());
            break;
        case NodeType::BOOLEAN:
            this->visit(n.boolean());
            break;
        case NodeType::BRK:
            this->visit(n.brk());
            break;
        case NodeType::CALL:
            this->visit(n.call());
            break;
        case NodeType::CLSEXP:
            this->visit(n.clsexp());
            break;
        case NodeType::CLSFLD:
            this->visit(n.clsfld());
            break;
        case NodeType::CLS:
            this->visit(n.cls());
            break;
        case NodeType::CNTINUE:
            this->visit(n.cntinue());
            break;
        case NodeType::DECL:
            this->visit(n.decl());
            break;
        case NodeType::EMPTYLST:
            this->visit(n.emptylst());
            break;
        case NodeType::FORLOOP:
            this->visit(n.forloop());
            break;
        case NodeType::FUNC:
            this->visit(n.func());
            break;
        case NodeType::ID:
            this->visit(n.id());
            break;
        case NodeType::IFF:
            this->visit(n.iff());
            break;
        case NodeType::LST:
            this->visit(n.lst());
            break;
        case NodeType::MEMBER:
            this->visit(n.member());
            break;
        case NodeType::NONE:
            this->visit(n.none());
            break;
        case NodeType::NUMBER:
            this->visit(n.number());
            break;
        case NodeType::FLOT:
            this->visit(n.flot());
            break;
        case NodeType::RETRN:
            this->visit(n.retrn());
            break;
        case NodeType::STRNG:
            this->visit(n.strng());
            break;
        case NodeType::SUB:
            this->visit(n.sub());
            break;
        case NodeType::TERNARY:
            this->visit(n.ternary());
            break;
        case NodeType::OTYPE:
//                this->visit(n.type());
            break;
        case NodeType::FTYPE:
//                this->visit(n.type());
            break;
        case NodeType::WHIL:
            this->visit(n.whil());
            break;
        case NodeType::UNINITIALIZED:
            break;
        case TUPLE:
            this->visit(n.tuple());
            break;
        default:
            throw std::runtime_error("Don't know what to do!");
    }
}

