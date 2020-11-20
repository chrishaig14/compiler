//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "Node.h"

#include "Node.h"
#include "../utils.h"
#include "optypes.h"



class BinopNode : public Node {
public:
    Node* left;
    Node* right;
    OpType op;

    BinopNode& binop() override { return *this; }
    const BinopNode& binop() const override { return *this; }

    BinopNode(OpType op, Node* left, Node* right);
    bool equal(const Node& x) const override ;


    ~BinopNode() {
        delete this->left;
        delete this->right;
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
