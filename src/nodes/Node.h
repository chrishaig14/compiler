//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "json.hpp"
//#include "NodeContainer.h"
#include <string>

using json = nlohmann::json;

class Visitor;
class NodeContainer;

class Node {
public:
    int start;
    int end;
    int line;
    int column;

    virtual void accept(Visitor& visitor) = 0;

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

#endif //NODE_H
