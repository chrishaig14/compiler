//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_BOOLOPNODE_H
#define UNTITLED1_BOOLOPNODE_H

#include "Node.h"
#include "optypes.h"
#include "TypeNode.h"

class BoolOpNode : public Node {
public:
    Node* left;
    Node* right;
    BoolOp op;

    BoolOpNode& boolop() override;

    const BoolOpNode& boolop() const override;

    BoolOpNode(BoolOp op, Node* left, Node* right) : BoolOpNode(op, left, right, {-1, -1}, {-1, -1}) {
    }

    BoolOpNode(BoolOp op, Node* left, Node* right, TextPosition start, TextPosition end);
    bool equal(const Node& x) const override;

    ~BoolOpNode();

    json to_json() const override;

    TypeNode* ltype;
    TypeNode* rtype;
};

#endif //UNTITLED1_BOOLOPNODE_H
