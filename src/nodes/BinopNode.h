//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_BINOPNODE_H
#define UNTITLED1_BINOPNODE_H


#include "Node.h"
#include "Visitor.h"

enum class OpType {
    ADD, SUB, MUL, DIV, EQ, AND, OR
};

std::string op_to_string(OpType op);

class BinopNode : public Node {
public:
    Node* left;
    Node* right;
    OpType op;

    BinopNode(OpType op, Node* left, Node* right);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const {
        auto other_ptr = dynamic_cast<BinopNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(BinopNode& other) const {
        return this->op == other.op && this->left->equal(other.left) and this->right->equal(other.right);
    }

    ~BinopNode() {
        delete this->left;
        delete this->right;
    }

    json to_json() const override {
        json j;
        j["node"] = "binop";
        j["left"] = this->left->to_json();
        j["right"] = this->right->to_json();
        j["op"] = op_to_string(this->op);
        return j;
    }
};


#endif //UNTITLED1_BINOPNODE_H
