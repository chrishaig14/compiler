//
// Created by chris on 1/8/20.
//

#ifndef SUBSCRIPTNODE_H
#define SUBSCRIPTNODE_H


#include "Node.h"

#include "Node.h"

#include "../types.h"

class SubscriptNode : public Node {

public:
    SubscriptNode(Node* parent, VectorOfNodes child, TextPosition start, TextPosition end);
    TypeNode* parent_t;
    Node* parent;
    VectorOfNodes child;

    bool equal(const Node& other) const override;

    ~SubscriptNode();


    SubscriptNode& sub() override;
    const SubscriptNode& sub() const override;
};


#endif //SUBSCRIPTNODE_H
