//
// Created by chris on 2/8/20.
//

#include "Translator.h"
#include "../instructions/BinopInst.h"
#include "../instructions/CallInst.h"
#include "../instructions/DeclareInst.h"
#include "../instructions/PushStringInst.h"
#include "../instructions/ReturnInst.h"
#include "../instructions/SetMemberInst.h"
#include "../instructions/GetMemberInst.h"
#include "../instructions/SetInst.h"
#include "../instructions/GetInst.h"
#include "../instructions/PushIntegerInst.h"
#include "../instructions/SetSubscriptInst.h"
#include "../instructions/GetSubscriptInst.h"

void Translator::visit(AssignmentNode& node) {
    Code out;
    node.rvalue->accept(*this);
    Code r_code = this->code;
    this->is_lvalue = true;
    node.lvalue->accept(*this);
    Code l_code = this->code;
    out = l_code;
    out.insert(out.end(), r_code.begin(), r_code.end());
    this->code = out;
}

void Translator::visit(BinopNode& node) {
    node.left->accept(*this);
    Code left_code = this->code;
    node.right->accept(*this);
    Code right_code = this->code;
    Code out = left_code;
    out.insert(out.end(), right_code.begin(), right_code.end());
    out.push_back(BinopInst(node.op));
    this->code = out;
}

void Translator::visit(BlockNode& node) {
    Code out;
    for (auto n: node.nodes) {
        n->accept(*this);
        Code node_code = this->code;
        out.insert(out.end(), node_code.begin(), node_code.end());
    }
    this->code = out;
}

void Translator::visit(CallNode& node) {
    Code out;
    node.function->accept(*this);
    out = this->code;
    for (auto a: node.arguments) {
        a->accept(*this);
        Code arg_code = this->code;
        out.insert(out.end(), arg_code.begin(), arg_code.end());
    }
    out.push_back(CallInst());
    this->code = out;
}

void Translator::visit(ClassNode& node) {

}

void Translator::visit(DeclarationNode& node) {
    Code out;
    if (node.expression != nullptr) {
        node.expression->accept(*this);
        out = this->code;
    }
    out.push_back(DeclareInst(node.identifier));
    this->code = out;
}

void Translator::visit(FunctionNode& node) {

}

void Translator::visit(IdNode& node) {
    Code out;
    if (this->is_lvalue) {
        this->is_lvalue = false;
        out.push_back(SetInst(node.identifier));
    } else {
        out.push_back(GetInst(node.identifier));
    }
    this->code = out;
}

void Translator::visit(IfNode& node) {

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

        out.push_back(SetMemberInst(node.child));
    } else {
        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(GetMemberInst(node.child));
    }
    this->code = out;
}

void Translator::visit(NumberNode& node) {
    Code out;
    out.push_back(PushIntegerInst(node.number));
    this->code = out;
}

void Translator::visit(ReturnNode& node) {
    node.expression->accept(*this);
    Code out = this->code;
    out.push_back(ReturnInst());
    this->code = out;
}

void Translator::visit(StringNode& node) {
    Code out;
    out.push_back(PushStringInst(node.str));
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

        out.push_back(SetSubscriptInst());
    } else {
        node.child->accept(*this);
        Code child_code = this->code;
        out.insert(out.end(), child_code.begin(), child_code.end());

        node.parent->accept(*this);
        Code parent_code = this->code;
        out.insert(out.end(), parent_code.begin(), parent_code.end());

        out.push_back(GetSubscriptInst());
    }
    this->code = out;
}

void Translator::visit(TypeNode& node) {

}
