//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"

#include "Node.h"

enum class NumberType {
    INTEGER, FLOAT, DOUBLE
};

class NumberNode : public Node {
public:
    std::string str;

    NumberNode(NumberType num_type, std::string str) : NumberNode(num_type, str, {-1, -1}) {
    }

    NumberNode(NumberType num_type, std::string str, TextPosition start);

    bool equal(const Node& x) const override;

    json to_json() const override;

    NumberNode& number() override;
    const NumberNode& number() const override;

    NumberType num_type;
};


#endif //NUMBERNODE_H
