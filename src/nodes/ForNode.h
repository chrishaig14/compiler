//
// Created by chris on 14/8/20.
//

#ifndef UNTITLED1_FORNODE_H
#define UNTITLED1_FORNODE_H


#include "Node.h"
#include "BlockNode.h"

class ForNode : public Node {
public:
    std::string var;
    NodeContainer exp;
    BlockNode* body;

    ForNode(const std::string& var, NodeContainer exp, BlockNode* body);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

};


#endif //UNTITLED1_FORNODE_H
