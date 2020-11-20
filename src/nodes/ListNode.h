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

    ListNode(VectorOfNodes elements);

    bool equal(const Node& other) const;

    ~ListNode();

    json to_json() const override;

    ListNode& lst() override;
    const ListNode& lst() const override;
};


#endif //LISTNODE_H
