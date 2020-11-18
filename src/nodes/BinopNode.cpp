//
// Created by chris on 1/8/20.
//

#include "BinopNode.h"

void BinopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

BinopNode::BinopNode(OpType op, Node* left, Node* right) : left(left), right(right), op(op) {this->ntype =BINOP;}

bool BinopNode::equal(const Node& x) const {
    auto& other = x.binop();
    return this->op == other.op && *this->left == *other.left and *this->right == *other.right;
}


std::string op_to_string(OpType op) {
    switch (op) {
        case OpType::ADD:
            return "ADD";
        case OpType::SUB:
            return "SUB";
        case OpType::MUL:
            return "MUL";
        case OpType::DIV:
            return "DIV";
        case OpType::EQ:
            return "EQ";
        case OpType::AND:
            return "AND";
        case OpType::OR:
            return "OR";
        case OpType::LEQ:
            return "CMP <=";
        case OpType::GEQ:
            return "CMP >=";
        case OpType::LT:
            return "CMP <";
        case OpType::GT:
            return "CMP >";
        case OpType::NEQ:
            return "CMP !=";
        case OpType::MOD:
            return "MOD";
        default:
            return "OP";
    }
}
