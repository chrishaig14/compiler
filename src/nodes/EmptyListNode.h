//
// Created by chris on 6/9/20.
//

#ifndef EMPTYLISTNODE_H
#define EMPTYLISTNODE_H


#include "Node.h"
#include "TypeNode.h"

class EmptyListNode : public Node {
public:
    EmptyListNode(TypeNode type);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    TypeNode& type;
};


#endif //EMPTYLISTNODE_H
