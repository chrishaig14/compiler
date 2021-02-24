//
// Created by chris on 18/8/20.
//

#ifndef BREAKNODE_H
#define BREAKNODE_H


#include "Node.h"


class BreakNode : public Node {
public:

    BreakNode();
    bool equal(const Node& other) const override;
    BreakNode& brk() override;
    const BreakNode& brk() const override;

};


#endif //BREAKNODE_H
