//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_STRINGNODE_H
#define UNTITLED1_STRINGNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"

class StringNode : public Node {
public:
    std::string str;

    void accept(Visitor& visitor) override;

    StringNode(std::string str);

    bool equal(Node* other) const override;

    bool operator==(StringNode& other) const;
};


#endif //UNTITLED1_STRINGNODE_H
