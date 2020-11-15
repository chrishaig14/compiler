//
// Created by chris on 17/8/20.
//

#ifndef BOOLEANNODE_H
#define BOOLEANNODE_H


#include "Node.h"
#include "Visitor.h"

class BooleanNode : public Node {
public:
    bool value;

    BooleanNode(bool value);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;
    bool operator!=(const BooleanNode& other) const;

    bool operator==(const BooleanNode& other) const {
        return this->value == other.value;
    }
};


#endif //BOOLEANNODE_H
