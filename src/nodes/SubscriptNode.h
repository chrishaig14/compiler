//
// Created by chris on 1/8/20.
//

#ifndef SUBSCRIPTNODE_H
#define SUBSCRIPTNODE_H


#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"

class SubscriptNode : public Node {

public:
    SubscriptNode(NodeContainer parent, std::vector<NodeContainer> child);

    NodeContainer parent;
    std::vector<NodeContainer> child;

    void accept(Visitor& visitor) override;

private:

    bool equal(NodeContainer other) const override;

    bool operator==(SubscriptNode& other) const;

    ~SubscriptNode() {
//        delete this->parent;
//        delete this->child;
    }

    json to_json() const override {
        json j;
        j["node"] = "subscript";
//        j["parent"] = this->parent->to_json();
//        j["child"] = this->child->to_json();
        return j;
    }
};


#endif //SUBSCRIPTNODE_H
