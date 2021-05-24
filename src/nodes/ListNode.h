//
// Created by chris on 1/8/20.
//

#ifndef LISTNODE_H
#define LISTNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"

class ListNode : public Node {
public:
    VectorOfNodes elements;

    ListNode(VectorOfNodes elements, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~ListNode();

    ListNode& lst() override;
    const ListNode& lst() const override;
    TypeNode* type;
};


#endif //LISTNODE_H
