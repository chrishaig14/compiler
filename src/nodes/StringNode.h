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
    StringNode(const std::string& str, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;

    StringNode& strng() override;
    const StringNode& strng() const override;
};


#endif //STRINGNODE_H
