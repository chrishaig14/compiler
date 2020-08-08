//
// Created by chris on 2/8/20.
//

#include "Translator.h"


void Translator::visit(AssignmentNode& node) {
    Code out;

    node.rvalue->accept(*this);
    Code r_code = this->code;
    out.insert(out.end(), r_code.begin(), r_code.end());

    this->is_lvalue = true;
    node.lvalue->accept(*this);
    Code l_code = this->code;
    out.insert(out.end(), l_code.begin(), l_code.end());

    this->code = out;
}

void Translator::visit(BinopNode& node) {
    node.left->accept(*this);
    Code left_code = this->code;
    node.right->accept(*this);
    Code right_code = this->code;
    Code out = left_code;
    out.insert(out.end(), right_code.begin(), right_code.end());
    out.push_back(I_BIN(node.op));
    this->code = out;
}

void Translator::visit(BlockNode& node) {
    Code out;
    for (auto n: node.nodes) {
        this->code = {};
        n->accept(*this);
        Code node_code = this->code;
        out.insert(out.end(), node_code.begin(), node_code.end());
    }
    this->code = out;
}

void Translator::visit(CallNode& node) {
    Code out;
    for (auto a: node.arguments) {
        a->accept(*this);
        Code arg_code = this->code;
        out.insert(out.end(), arg_code.begin(), arg_code.end());
    }
    node.function->accept(*this);
    Code function_code = this->code;
    out.insert(out.end(), function_code.begin(), function_code.end());
    out.push_back(I_CALL);
    this->code = out;
}

void Translator::visit(ClassNode& node) {
    Code out;
    std::vector<std::string> f;
    for (auto field: node.fields) {
        f.push_back(field->identifier);
    }
    out.push_back(I_MAKE_CLASS(node.identifier, f));
    this->code = out;
}

void Translator::visit(DeclarationNode& node) {
    Code out;
    if (node.expression != nullptr) {
        node.expression->accept(*this);
        out = this->code;
    }
    out.push_back(I_DECL(node.identifier));
    out.push_back(I_SET(node.identifier));
    this->code = out;
}

void Translator::visit(FunctionNode& node) {
    Code out;
    out.push_back(I_DECL(node.identifier));
    Code body_code;
    node.body->accept(*this);
    body_code = this->code;
    std::vector<std::string> closure;
    for (auto fv: node.free_variables) {
        closure.push_back(fv.first);
    }
    out.push_back(I_PUSHF(node.parameter_names, body_code, closure));
    out.push_back(I_SET(node.identifier));
    this->code = out;
}

void Translator::visit(IdNode& node) {
    Code out;
    if (this->is_lvalue) {
        this->is_lvalue = false;
        out.push_back(I_SET(node.identifier));
    } else {
        out.push_back(I_GET(node.identifier));
    }
    this->code = out;
}

void Translator::visit(IfNode& node) {
    Code out;
    node.condition->accept(*this);
    Code condition_code = this->code;
    out.insert(out.end(), condition_code.begin(), condition_code.end());
    node.then->accept(*this);
    Code then_code = this->code;
    out.push_back(I_JUMPF(then_code.size() + 1));
    out.insert(out.end(), then_code.begin(), then_code.end());
    this->code = out;
}

void Translator::visit(ListNode& node) {

}

void Translator::visit(MemberNode& node) {
    Code out;
    if (this->is_lvalue) {
        this->is_lvalue = false;

        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(I_SETM(node.child));
    } else {
        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(I_GETM(node.child));
    }
    this->code = out;
}

void Translator::visit(NumberNode& node) {
    Code out;
    out.push_back(I_PUSHI(node.number));
    this->code = out;
}

void Translator::visit(ReturnNode& node) {
    node.expression->accept(*this);
    Code out = this->code;
    out.push_back(I_RET);
    this->code = out;
}

void Translator::visit(StringNode& node) {
    Code out;
    out.push_back(I_PUSHS(node.str));
    this->code = out;
}

void Translator::visit(SubscriptNode& node) {
    Code out;
    if (this->is_lvalue) {
        this->is_lvalue = false;

        node.child->accept(*this);
        Code child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(I_SETS);
    } else {
        node.child->accept(*this);
        Code child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(I_GETS);
    }
    this->code = out;
}

void Translator::visit(TypeNode& node) {

}

Translator::Translator() : is_lvalue(false) {}

void Translator::visit(ClassLiteralExpressionNode& node) {
    Code all;
    for(auto exp: node.init){
        this->code = {};
        exp->accept(*this);
        Code out = this->code;
        all.insert(all.end(), out.begin(), all.end());
    }
//    all.push_back(MakeObjectExpInst(node.identifier, node.init.size()));
    this->code = all;
}

void Translator::visit(ClassLiteralFieldNode& node) {
    Code all;
    for(auto exp: node.init){
        this->code = {};
//        exp->accept(*this);
        Code out = this->code;
        all.insert(all.end(), out.begin(), all.end());
    }
//    all.push_back(MakeObjectExpInst(node.identifier, node.init.size()));
    this->code = all;
}
