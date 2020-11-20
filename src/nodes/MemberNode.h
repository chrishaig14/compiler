//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "Node.h"

#include "Node.h"

class MemberNode : public Node {
public:
    Node* parent;
    std::string child;

    MemberNode(Node* parent, const std::string& child);

    bool equal(const Node& x) const override;

    ~MemberNode();

    json to_json() const override;

    MemberNode& member() override;
    const MemberNode& member() const override;
};


#endif //MEMBERNODE_H
