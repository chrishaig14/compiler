//
// Created by chris on 17/8/20.
//

#ifndef BOOLEANNODE_H
#define BOOLEANNODE_H


#include "Node.h"
#include "../json/json.hpp"

class BooleanNode : public Node {
public:
    bool value;

    BooleanNode(bool value, TextPosition start, TextPosition end);

    bool operator!=(const BooleanNode& other) const;

    bool equal(const Node& x) const override;

    nlohmann::json to_json() const override;
};


#endif //BOOLEANNODE_H
