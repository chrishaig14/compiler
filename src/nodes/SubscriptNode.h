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
    SubscriptNode(Node* parent, VectorOfNodes child);

    Node* parent;
    VectorOfNodes child;

    bool equal(const Node& other) const override;

    ~SubscriptNode();

    json to_json() const
    override;

    SubscriptNode& sub() override;
    const SubscriptNode& sub() const override;
};


#endif //SUBSCRIPTNODE_H
