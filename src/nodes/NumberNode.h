//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"

enum class NumberType {
    INTEGER, FLOAT, DOUBLE
};

class NumberNode : public Node {
public:
    std::string str;

    NumberNode(NumberType num_type, std::string str, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;

    NumberType num_type;

    nlohmann::json to_json() const override;
};


#endif //NUMBERNODE_H
