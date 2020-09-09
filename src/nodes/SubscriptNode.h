//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_SUBSCRIPTNODE_H
#define UNTITLED1_SUBSCRIPTNODE_H


#include "Node.h"
#include "Visitor.h"

class SubscriptNode : public Node {

public:
    SubscriptNode(Node* parent, std::vector<Node*> child);

    Node* parent;
    std::vector<Node*> child;
private:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(SubscriptNode& other) const;

    ~SubscriptNode() {
        delete this->parent;
//        delete this->child;
    }

    json to_json() const override {
        json j;
        j["node"] = "subscript";
        j["parent"] = this->parent->to_json();
//        j["child"] = this->child->to_json();
        return j;
    }
};


#endif //UNTITLED1_SUBSCRIPTNODE_H
