//
// Created by chris on 2/8/20.
//

#include <iostream>
#include "Translator.h"
#include "../vm/LabelledCode.h"


void Translator::visit(AssignmentNode& node) {
    CodeLabel out;

    node.rvalue->accept(*this);
    CodeLabel r_code = this->code;
    out.insert(out.end(), r_code.begin(), r_code.end());

    this->is_lvalue = true;
    IdNode* ln = TO_ID(node.lvalue);
    if (ln != nullptr && ln->identifier == "_") {
        out.push_back(LC("", I_POP));
    } else {
        node.lvalue->accept(*this);
        CodeLabel l_code = this->code;
        out.insert(out.end(), l_code.begin(), l_code.end());
    }
    this->code = out;
}

void Translator::visit(BinopNode& node) {
    node.left->accept(*this);
    CodeLabel left_code = this->code;
    node.right->accept(*this);
    CodeLabel right_code = this->code;
    CodeLabel out = left_code;
    out.insert(out.end(), right_code.begin(), right_code.end());
    out.push_back(LC("", I_BIN(node.op)));
    this->code = out;
}

void Translator::visit(BlockNode& node) {
    CodeLabel out;
    for (auto n: node.nodes) {
        this->code = {};
        n->accept(*this);
        CodeLabel node_code = this->code;
        out.insert(out.end(), node_code.begin(), node_code.end());
    }
    this->code = out;
}

void Translator::visit(CallNode& node) {
    CodeLabel out;
    for (auto a: node.arguments) {
        a->accept(*this);
        CodeLabel arg_code = this->code;
        out.insert(out.end(), arg_code.begin(), arg_code.end());
    }
    IdNode* function_id = dynamic_cast<IdNode*>(node.function);
    out.push_back(LC("", I_GET(function_id->identifier)));
    out.push_back(LC("", I_CALL));
    this->code = out;
}

void Translator::visit(StructNode& node) {
    CodeLabel out;
    std::vector<std::string> f;
    for (auto field: node.fields) {
        f.push_back(field.first);
    }
    out.push_back(LC("", I_MAKE_CLASS(node.identifier, f)));
    this->code = out;
}

void Translator::visit(DeclarationNode& node) {
    CodeLabel out;
    if (node.expression != nullptr) {
        node.expression->accept(*this);
        out = this->code;
    }
    out.push_back(LC("", I_DECL(node.identifier)));
    out.push_back(LC("", I_SET(node.identifier)));
    this->code = out;
}

void Translator::visit(FunctionNode& node) {
    CodeLabel out;
    std::string new_name = node.identifier;
    out.push_back(LC("", I_START_FUNCTION(new_name)));
    CodeLabel body_code;
    node.body->accept(*this);
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
        out.push_back(LC("", I_SET(node.identifier)));
    } else {
        out.push_back(LC("", I_GET(node.identifier)));
    }
    this->code = out;
}

void Translator::visit(IfNode& node) {
    CodeLabel out;
    node.condition->accept(*this);
    CodeLabel condition_code = this->code;
    out.insert(out.end(), condition_code.begin(), condition_code.end());
    node.then->accept(*this);
    CodeLabel then_code = this->code;
    bool has_else = node.selse != nullptr;
    out.push_back(LC("labelinif", I_JUMPF(then_code.size() + 3 + (has_else && node.elifs.size() == 0 ? 1 : 0))));
    out.push_back(LC("", new EnterScope("if")));
    out.insert(out.end(), then_code.begin(), then_code.end());
    out.push_back(LC("", new LeaveScope("if")));

    for (int i = 0; i < node.elifs.size(); i++) {
        CodeLabel eout;
        node.elifs[i].first->accept(*this);
        CodeLabel econdition_code = this->code;
        out.insert(out.end(), econdition_code.begin(), econdition_code.end());
        node.elifs[i].second->accept(*this);
        CodeLabel ethen_code = this->code;
        out.push_back(LC("", I_JUMPF(ethen_code.size() + 3 + ((has_else && i == node.elifs.size() - 1) ? 1 : 0))));
        out.push_back(LC("", new EnterScope("elif")));
        out.insert(out.end(), ethen_code.begin(), ethen_code.end());
        out.push_back(LC("", new LeaveScope("elif")));
    }
    if (has_else) {
        node.selse->accept(*this);
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
        e->accept(*this);
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

        node.parent->accept(*this);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_SETM(node.child)));
    } else {
        node.parent->accept(*this);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_GETM(node.child)));
    }
    this->code = out;
}

void Translator::visit(NumberNode& node) {
    CodeLabel out;
    out.push_back(LC("", I_PUSHI(node.number)));
    this->code = out;
}

void Translator::visit(ReturnNode& node) {
    CodeLabel out;
    if (node.expression != nullptr) {
        node.expression->accept(*this);
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

        node.parent->accept(*this);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_SETS));
    } else {
//        node.child->accept(*this);
        node.child[0]->accept(*this);
        CodeLabel child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

        node.parent->accept(*this);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_GETS));
    }
    this->code = out;
}

void Translator::visit(TypeNode& node) {

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
        exp->accept(*this);
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
    for (auto f: node.init) {
        this->code = {};
        f.second->accept(*this);
        fields.push_back(f.first);
        CodeLabel out = this->code;
        all.insert(all.end(), out.begin(), out.end());
    }
    all.push_back(LC("", new MakeObjectInst(node.type->identifier, fields)));
    this->code = all;
}

void Translator::visit(ForNode& node) {
    CodeLabel out;
    int loop_number = this->loop_counter;
    this->current_loop = loop_number;
    this->loop_counter++;

    std::string list_name = ".list" + std::to_string(this->current_loop);
    std::string len_name = ".len" + std::to_string(this->current_loop);
    std::string index_name = ".index" + std::to_string(this->current_loop);

    // list = expression
    auto list_init = DECL(list_name, nullptr, node.exp);
    list_init->accept(*this);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // len = list.len()
    auto list_len = DECL(len_name, nullptr, CALL(ID("List.len"), {ID(list_name)}));
    list_len->accept(*this);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // index = 0
    auto index = DECL(index_name, nullptr, NUM(0));
    index->accept(*this);
    out.insert(out.end(), this->code.begin(), this->code.end());

    // index < list_len ?
    auto condition = BIN(OpType::LT, ID(index_name), ID(len_name));
    condition->accept(*this);
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
    auto it = SUB(ID(list_name), { ID(index_name) });
    node.body->nodes.insert(node.body->nodes.begin(), DECL(node.var, nullptr, it));

    // index = index + 1
    auto update_index = ASN(ID(index_name), BIN(OpType::ADD, ID(index_name), NUM(1)));
    update_index->accept(*this);
    CodeLabel update_index_code = this->code;

//    out.insert(out.end(), condition_code.begin(), condition_code.end());
    bool old_in_for_loop = this->in_for_loop;
    this->in_for_loop = true;
    node.body->accept(*this);
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
    node.condition->accept(*this);
    std::vector<std::pair<std::string, Instruction*>> condition_code = this->code;
    int loop_number = this->loop_counter;
    this->current_loop = loop_number;
    this->loop_counter++;
    std::string start_loop_label = "start_loop." + std::to_string(loop_number);
    out.push_back(LC(start_loop_label, condition_code[0].second));
    for (int i = 1; i < condition_code.size(); i++) {
        out.push_back(condition_code[i]);
    }
    node.body->accept(*this);
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
    node.expression->accept(*this);
    CodeLabel expression_code = this->code;
    out.insert(out.end(), expression_code.begin(), expression_code.end());
    node.true_case->accept(*this);
    CodeLabel true_case = this->code;
    out.push_back(NL(I_JUMPN(true_case.size() + 6)));
    CodeLabel it_code = {NL(I_ENTER("it_scope")), NL(I_DECL("it")), NL(I_SET("it"))};
    out.insert(out.end(), it_code.begin(), it_code.end());
    out.insert(out.end(), true_case.begin(), true_case.end());
    out.push_back(NL(I_LEAVE("it_scope")));
    node.false_case->accept(*this);
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
        method.second->identifier = node.class_name + "." + method.second->identifier;
        method.second->parameter_names.insert(method.second->parameter_names.begin(), "this");
        method.second->parameter_types.insert(method.second->parameter_types.begin(), nullptr);
        method.second->accept(*this);
        auto method_code = this->code;
        out.insert(out.end(), method_code.begin(), method_code.end());
    }
    this->code = out;
}

void Translator::visit(InstanceNode& node) {

}

void Translator::visit(ContinueNode& node) {
    if (this->in_for_loop) {
        std::cout << "TRANSLATING A CONTINUE NOED" << std::endl;
        CodeLabel out;
        auto update_index = ASN(ID(".index" + std::to_string(this->current_loop)),
                                BIN(OpType::ADD, ID(".index" + std::to_string(this->current_loop)), NUM(1)));
        update_index->accept(*this);
        out.insert(out.end(), this->code.begin(), this->code.end());
        out.push_back(LC("", I_LEAVE("for")));
        out.push_back(LC("", I_JUMP("start_loop." + std::to_string(this->current_loop))));
        this->code = out;
    }else {
        // while loop
        CodeLabel out;
        out.push_back(LC("", I_JUMP("start_loop." + std::to_string(this->current_loop))));
        this->code = out;

    }
}
