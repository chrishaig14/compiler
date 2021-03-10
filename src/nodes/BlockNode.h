//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"

class BlockNode : public Node {
public:

    BlockNode(VectorOfNodes nodes);

    VectorOfNodes nodes;

    bool equal(const Node& p) const override;

    json to_json();

    BlockNode& block() override;
    const BlockNode& block() const override;
    ~BlockNode();
    VectorOfStrings local_vars;
};


#endif //BLOCKNODE_H
