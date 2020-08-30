//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_NODE_H
#define UNTITLED1_NODE_H

#include "json.hpp"
#include <string>

using json = nlohmann::json;

class Visitor;

class Node {
public:
    int start;
    int end;

    virtual void accept(Visitor& visitor) = 0;

    virtual bool equal(Node* other) const = 0;

    virtual ~Node() = default;

    friend std::ostream& operator<<(std::ostream& os, const Node& node) {
        return os << node.to_json();
    }

    virtual json to_json() const {
        json j;
        j["name"] = "johnny";
        return j;
    }
};

#endif //UNTITLED1_NODE_H
