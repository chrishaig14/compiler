//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "node_types.h"
#include <string>
#include "../scanner/TextPosition.h"
#include "../json/json_fwd.hpp"
#include <iostream>

class Node {
public:
    const NodeType ntype;

    TextPosition start;
    TextPosition end;

    Node(NodeType ntype, TextPosition start, TextPosition end);

    virtual ~Node();

    virtual bool equal(const Node& other) const;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;

    virtual nlohmann::json to_json() const = 0;
};

#endif //NODE_H
