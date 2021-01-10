//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"

#include "Node.h"

class NumberNode : public Node {
public:
    int num;

    NumberNode(int number) : NumberNode(number, {-1, -1}) {}

    NumberNode(int number, TextPosition start);

    bool equal(const Node& x) const override;

    json to_json() const override;

    NumberNode& number() override;
    const NumberNode& number() const override;

};


#endif //NUMBERNODE_H
