//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_SUBSCRIPTNODE_H
#define UNTITLED1_SUBSCRIPTNODE_H


#include "Node.h"
#include "Visitor.h"

class SubscriptNode : public Node {
    Node* parent;
    Node* child;

public:
    SubscriptNode(Node* parent, Node* child);

private:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(SubscriptNode& other) const;

    ~SubscriptNode() {
        delete this->parent;
        delete this->child;
    }
};


#endif //UNTITLED1_SUBSCRIPTNODE_H
