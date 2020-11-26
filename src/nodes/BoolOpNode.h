//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_BOOLOPNODE_H
#define UNTITLED1_BOOLOPNODE_H

#include "Node.h"
#include "optypes.h"

class BoolOpNode : public Node {
public:
    Node* left;
    Node* right;
    BoolOp op;

    BoolOpNode& boolop() override;

    const BoolOpNode& boolop() const override;

    BoolOpNode(BoolOp op, Node* left, Node* right);
    bool equal(const Node& x) const override;

    ~BoolOpNode();

    json to_json() const override;

};

#endif //UNTITLED1_BOOLOPNODE_H
