//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"

#include "Node.h"

class ReturnNode : public Node {
public:
    explicit ReturnNode(Node* expression);

    bool equal(const Node& other) const;

    ~ReturnNode();

    json to_json() const override;

    ReturnNode& retrn() override;

    const ReturnNode& retrn() const override;

    Node* expression;
};

#endif //RETURNNODE_H
