//
// Created by chris on 6/9/20.
//

#ifndef EMPTYLISTNODE_H
#define EMPTYLISTNODE_H


#include "Node.h"
#include "TypeNode.h"

class EmptyListNode : public Node {
public:
    EmptyListNode(TypeNode* type, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~EmptyListNode() override;
    EmptyListNode& emptylst() override;
    const EmptyListNode& emptylst() const override;

    TypeNode* type;
};


#endif //EMPTYLISTNODE_H
