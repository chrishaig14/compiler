//
// Created by chris on 17/8/20.
//

#ifndef UNTITLED1_BOOLEANNODE_H
#define UNTITLED1_BOOLEANNODE_H


#include "Node.h"
#include "Visitor.h"

class BooleanNode : public Node {
public:
    bool value;

    BooleanNode(bool value);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(const BooleanNode& other) const {
        return this->value == other.value;
    }
};


#endif //UNTITLED1_BOOLEANNODE_H
