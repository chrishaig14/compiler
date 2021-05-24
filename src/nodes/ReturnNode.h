//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"
#include "../types.h"
#include "TypeNode.h"

class ReturnNode : public Node {
public:
    TypeNode* ret_type;
    explicit ReturnNode(Node* expression, TextPosition start, TextPosition end);


    bool equal(const Node& other) const override;

    ~ReturnNode();

    ReturnNode& retrn() override;

    const ReturnNode& retrn() const override;

    Node* expression;
    std::vector<std::pair<std::string, TypeNode*>> reachables;
};

#endif //RETURNNODE_H
