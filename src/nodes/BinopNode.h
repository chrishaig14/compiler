//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
enum class OpType {
    ADD, SUB, MUL, DIV, EQ, AND, OR, LEQ, GEQ, LT, GT, NEQ, MOD
};

std::string op_to_string(OpType op);

class BinopNode : public Node {
public:
    NodeContainer left;
    NodeContainer right;
    OpType op;

    BinopNode(OpType op, NodeContainer left, NodeContainer right);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const {
//        auto other_ptr = dynamic_cast<BinopNodeContainer>(other);
//        if (other_ptr == nullptr) return false;
//        return *this == *other_ptr;
return false;
    }

    bool operator==(BinopNode& other) const {
//        return this->op == other.op && this->left->equal(other.left) and this->right->equal(other.right);
return false;
    }

    ~BinopNode() {
//        delete this->left;
//        delete this->right;
    }

    json to_json() const override {
        json j;
        j["node"] = "binop";
//        j["left"] = this->left->to_json();
//        j["right"] = this->right->to_json();
        j["op"] = op_to_string(this->op);
        return j;
    }
};


#endif //BINOPNODE_H
