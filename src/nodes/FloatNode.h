//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_FLOATNODE_H
#define UNTITLED1_FLOATNODE_H


#include "Node.h"

class FloatNode : public Node {
    float value;
public:
    FloatNode(float value) {
        this->value = value;
        this->ntype = FLOT;
    }

    bool equal(const Node& other) const override;
    FloatNode& flot() override;
    const FloatNode& flot() const override;

};


#endif //UNTITLED1_FLOATNODE_H
