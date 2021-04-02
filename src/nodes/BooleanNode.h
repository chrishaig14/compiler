//
// Created by chris on 17/8/20.
//

#ifndef BOOLEANNODE_H
#define BOOLEANNODE_H


#include "Node.h"


class BooleanNode : public Node {
public:
    bool value;

    BooleanNode(bool value) : BooleanNode(value, {-1, -1}, {-1, -1}) {
    }

    BooleanNode(bool value, TextPosition start, TextPosition end);

    bool operator!=(const BooleanNode& other) const;

    bool equal(const Node& x) const override;
    const BooleanNode& boolean() const override;
    BooleanNode& boolean() override;
};


#endif //BOOLEANNODE_H
