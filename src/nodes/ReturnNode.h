//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"
#include "../types.h"

class ReturnNode : public Node {
public:
    explicit ReturnNode(Node* expression, TextPosition start);

    explicit ReturnNode(Node* expression) : ReturnNode(expression, {-1, -1}) {}

    bool equal(const Node& other) const;

    ~ReturnNode();

    json to_json() const override;

    ReturnNode& retrn() override;

    const ReturnNode& retrn() const override;

    Node* expression;
    VectorOfStrings reachables;
};

#endif //RETURNNODE_H
