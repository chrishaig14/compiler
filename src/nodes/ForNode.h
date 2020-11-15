//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "BlockNode.h"

class ForNode : public Node {
public:
    std::string var;
    NodeContainer exp;
    BlockNode& body;
    bool operator==(const ForNode& other) const {
        return false;
    }
    bool operator!=(const ForNode& other) const {
        return !(*this==other);
    }
    ForNode(const std::string& var, NodeContainer exp, BlockNode body);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

};


#endif //FORNODE_H
