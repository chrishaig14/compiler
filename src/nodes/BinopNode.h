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

    BinopNode& binop() override;
    const BinopNode& binop() const override;

    BinopNode(OpType op, Node* left, Node* right) : BinopNode(op, left, right, start) {}

    BinopNode(OpType op, Node* left, Node* right, TextPosition start);
    bool equal(const Node& x) const override;

    ~BinopNode();

    json to_json() const override;

};


#endif //BINOPNODE_H
