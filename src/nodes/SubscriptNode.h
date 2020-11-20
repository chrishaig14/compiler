//
// Created by chris on 1/8/20.
//

#ifndef SUBSCRIPTNODE_H
#define SUBSCRIPTNODE_H


#include "Node.h"
#include "Visitor.h"
#include "Node.h"

#include "../types.h"

class SubscriptNode : public Node {

public:
    SubscriptNode(Node* parent, VectorOfNodes child);

    Node* parent;
    VectorOfNodes child;

    void accept(Visitor& visitor) override;
    bool equal(const Node& other) const override;

    ~SubscriptNode() {
        delete this->parent;
        for (auto c: this->child) {
            delete c;
        }
    }

    json to_json() const
    override {
        json j;
        j["node"] = "subscript";
//        j["parent"] = this->parent->to_json();
//        j["child"] = this->child->to_json();
        return j;
    }

    SubscriptNode& sub() override;
    const SubscriptNode& sub() const override;
};


#endif //SUBSCRIPTNODE_H
