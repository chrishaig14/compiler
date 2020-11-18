//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "BlockNode.h"
#include "../utils.h"
class ForNode : public Node {
public:
    std::string var;
    Node* exp;
    BlockNode* body;

    bool operator==(const ForNode& other) const;

    bool operator!=(const ForNode& other) const;

    ForNode(const std::string& var, Node* exp, BlockNode* body);

    void accept(Visitor& visitor) override;

};


#endif //FORNODE_H
