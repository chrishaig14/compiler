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
    SubscriptNode(UNode& parent, VectorOfNodesU& child, TextPosition start, TextPosition end);
    TypeNode* parent_t;
    UNode parent;
    VectorOfNodesU child;

    bool equal(const Node& other) const override;

    ~SubscriptNode();
    nlohmann::json to_json() const override;


};


#endif //SUBSCRIPTNODE_H
