//
// Created by chris on 1/8/20.
//

#ifndef STRINGNODE_H
#define STRINGNODE_H


#include <string>
#include "Node.h"

#include "Node.h"

class StringNode : public Node {
public:
    std::string str;

    StringNode(std::string str);
    bool equal(const Node& x) const override;

    json to_json() const override {
        json j;
        j["node"] = "string";
        j["str"] = this->str;
        return j;
    }

    StringNode& strng() override;
    const StringNode& strng() const override;
};


#endif //STRINGNODE_H
