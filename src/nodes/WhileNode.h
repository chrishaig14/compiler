//
// Created by chris on 16/8/20.
//

#ifndef WHILENODE_H
#define WHILENODE_H


#include "Node.h"
#include "BlockNode.h"

class WhileNode : public Node {
public:
    BlockNode& body;
    NodeContainer condition;


    WhileNode(NodeContainer condition, BlockNode& body);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(const WhileNode& other) const;
};


#endif //WHILENODE_H
