//
// Created by chris on 2/8/20.
//

#include "Translator.h"
#include "../vm/LabelledCode.h"


void Translator::visit(AssignmentNode& node) {
    CodeLabel out;

    node.rvalue->accept(*this);
    CodeLabel r_code = this->code;
    out.insert(out.end(), r_code.begin(), r_code.end());

    this->is_lvalue = true;
    node.lvalue->accept(*this);
    CodeLabel l_code = this->code;
    out.insert(out.end(), l_code.begin(), l_code.end());

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
    node.function->accept(*this);
    CodeLabel function_code = this->code;
    out.insert(out.end(), function_code.begin(), function_code.end());
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
    out.push_back(LC("", I_START_FUNCTION(node.identifier)));
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
    out.push_back(LC("", I_JUMPF(then_code.size() + 3)));
    out.push_back(LC("", new EnterScope("if")));
    out.insert(out.end(), then_code.begin(), then_code.end());
    out.push_back(LC("", new LeaveScope("if")));
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
    node.expression->accept(*this);
    CodeLabel out = this->code;
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

        node.child->accept(*this);
        CodeLabel child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

        node.parent->accept(*this);
        CodeLabel parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(LC("", I_SETS));
    } else {
        node.child->accept(*this);
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
}

void Translator::visit(ClassLiteralExpressionNode& node) {
    CodeLabel all;
    for (auto exp: node.init) {
        this->code = {};
        exp->accept(*this);
        CodeLabel out = this->code;
        all.insert(all.end(), out.begin(), out.end());
    }
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
    all.push_back(LC("", new MakeObjectInst(node.identifier, fields)));
    this->code = all;
}

void Translator::visit(ForNode& node) {

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
