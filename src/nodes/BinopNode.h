//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "Node.h"
#include "Visitor.h"
#include "Node.h"
#include "../utils.h"

enum class OpType {
    ADD, SUB, MUL, DIV, EQ, AND, OR, LEQ, GEQ, LT, GT, NEQ, MOD
};

std::string op_to_string(OpType op);

class BinopNode : public Node {
public:
    Node* left;
    Node* right;
    OpType op;

    BinopNode(OpType op, Node* left, Node* right);

    void accept(Visitor& visitor) override;

    bool operator==(const BinopNode& other) const;
    bool operator!=(const BinopNode& other) const;


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
