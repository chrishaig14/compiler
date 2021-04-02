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
    StringNode(std::string str, TextPosition start, TextPosition end);

    StringNode(std::string str) : StringNode(str, {-1, -1}, {-1, -1}) {
    }

    bool equal(const Node& x) const override;

    json to_json() const override;

    StringNode& strng() override;
    const StringNode& strng() const override;
};


#endif //STRINGNODE_H
