//
// Created by chris on 18/8/20.
//

#ifndef UNTITLED1_BREAKNODE_H
#define UNTITLED1_BREAKNODE_H


#include "Node.h"
#include "Visitor.h"

class BreakNode : public Node {
public:
    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

};


#endif //UNTITLED1_BREAKNODE_H
